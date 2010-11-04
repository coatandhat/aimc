// Copyright 2008-2010, Thomas Walters
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*! \file
 *  \brief Dick Lyon's Pole-Zero Filter Cascade - implemented as an AIM-C
 *  module by Tom Walters from the AIM-MAT module based on Dick Lyon's code
 */

/*! \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2008/02/05
 *  \version \$Id$
 */

#include "Support/ERBTools.h"

#include "Modules/BMM/ModulePZFC.h"

namespace aimc {
ModulePZFC::ModulePZFC(Parameters *parameters) : Module(parameters) {
  module_identifier_ = "pzfc";
  module_type_ = "bmm";
  module_description_ = "Pole-Zero Filter Cascade";
  module_version_ = "$Id$";

  // Get parameter values, setting default values where necessary
  // Each parameter is set here only if it has not already been set elsewhere.
  cf_max_ = parameters_->DefaultFloat("pzfc.highest_frequency", 6000.0f);
  cf_min_ = parameters_->DefaultFloat("pzfc.lowest_frequency", 100.0f);
  pole_damping_ = parameters_->DefaultFloat("pzfc.pole_damping", 0.12f);
  zero_damping_ = parameters_->DefaultFloat("pzfc.zero_damping", 0.2f);
  zero_factor_ = parameters_->DefaultFloat("pzfc.zero_factor", 1.4f);
  step_factor_ = parameters_->DefaultFloat("pzfc.step_factor", 1.0f/3.0f);
  bandwidth_over_cf_ = parameters_->DefaultFloat("pzfc.bandwidth_over_cf",
                                                 0.11f);
  min_bandwidth_hz_ = parameters_->DefaultFloat("pzfc.min_bandwidth_hz",
                                                27.0f);
  agc_factor_ = parameters_->DefaultFloat("pzfc.agc_factor", 12.0f);
  do_agc_step_ = parameters_->DefaultBool("pzfc.do_agc", true);
  use_fitted_parameters_ = parameters_->DefaultBool("pzfc.use_fit", false);

  detect_.resize(0);
}

ModulePZFC::~ModulePZFC() {
}

bool ModulePZFC::InitializeInternal(const SignalBank &input) {
  // Make local convenience copies of some variables
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = 0;

  // Prepare the coefficients and also the output SignalBank
  if (!SetPZBankCoeffs())
    return false;

  // The output signal bank should be set up by now.
  if (!output_.initialized())
    return false;

  // This initialises all buffers which can be modified by Process()
  ResetInternal();

  return true;
}

void ModulePZFC::ResetInternal() {
  // These buffers may be actively modified by the algorithm
  agc_state_.clear();
  agc_state_.resize(channel_count_);
  for (int i = 0; i < channel_count_; ++i) {
    agc_state_[i].clear();
    agc_state_[i].resize(agc_stage_count_, 0.0f);
  }

  state_1_.clear();
  state_1_.resize(channel_count_, 0.0f);

  state_2_.clear();
  state_2_.resize(channel_count_, 0.0f);

  previous_out_.clear();
  previous_out_.resize(channel_count_, 0.0f);

  pole_damps_mod_.clear();
  pole_damps_mod_.resize(channel_count_, 0.0f);

  inputs_.clear();
  inputs_.resize(channel_count_, 0.0f);

  // Init AGC
  AGCDampStep();
  // pole_damps_mod_ and agc_state_ are now be initialized

  // Modify the pole dampings and AGC state slightly from their values in
  // silence in case the input is abuptly loud.
  for (int i = 0; i < channel_count_; ++i) {
    pole_damps_mod_[i] += 0.05f;
    for (int j = 0; j < agc_stage_count_; ++j)
      agc_state_[i][j] += 0.05f;
  }

  last_input_ = 0.0f;
}

bool ModulePZFC::SetPZBankCoeffsOrig() {
  // This function sets the following variables:
  // channel_count_
  // pole_dampings_
  // pole_frequencies_
  // za0_, za1_, za2
  // output_
  
  // TODO(tomwalters): There's significant code-duplication between this function
  // and SetPZBankCoeffsERBFitted, and SetPZBankCoeffs
  
  // Normalised maximum pole frequency
  float pole_frequency = cf_max_ / sample_rate_ * (2.0f * M_PI);
  channel_count_ = 0;
  while ((pole_frequency / (2.0f * M_PI)) * sample_rate_ > cf_min_) {
  float bw = bandwidth_over_cf_ * pole_frequency + 2 * M_PI * min_bandwidth_hz_ / sample_rate_;
    pole_frequency -= step_factor_ * bw;
    channel_count_++;
  }
  
  // Now the number of channels is known, various buffers for the filterbank
  // coefficients can be initialised
  pole_dampings_.clear();
  pole_dampings_.resize(channel_count_, pole_damping_);
  pole_frequencies_.clear();
  pole_frequencies_.resize(channel_count_, 0.0f);

  // Direct-form coefficients
  za0_.clear();
  za0_.resize(channel_count_, 0.0f);
  za1_.clear();
  za1_.resize(channel_count_, 0.0f);
  za2_.clear();
  za2_.resize(channel_count_, 0.0f);

  // The output signal bank
  output_.Initialize(channel_count_, buffer_length_, sample_rate_);
  
  // Reset the pole frequency to maximum
  pole_frequency = cf_max_ / sample_rate_ * (2.0f * M_PI);

  for (int i = channel_count_ - 1; i > -1; --i) {
    // Store the normalised pole frequncy
    pole_frequencies_[i] = pole_frequency;

    // Calculate the real pole frequency from the normalised pole frequency
    float frequency = pole_frequency / (2.0f * M_PI) * sample_rate_;

    // Store the real pole frequency as the 'centre frequency' of the filterbank
    // channel
    output_.set_centre_frequency(i, frequency);

    float zero_frequency = Minimum(M_PI, zero_factor_ * pole_frequency);

    // Impulse-invariance mapping
    float z_plane_theta = zero_frequency * sqrt(1.0f - pow(zero_damping_, 2));
    float z_plane_rho = exp(-zero_damping_ * zero_frequency);

    // Direct-form coefficients from z-plane rho and theta
    float a1 = -2.0f * z_plane_rho * cos(z_plane_theta);
    float a2 = z_plane_rho * z_plane_rho;

    // Normalised to unity gain at DC
    float a_sum = 1.0f + a1 + a2;
    za0_[i] = 1.0f / a_sum;
    za1_[i] = a1 / a_sum;
    za2_[i] = a2 / a_sum;

    // Subtract step factor (1/n2) times current bandwidth from the pole
    // frequency
  float bw = bandwidth_over_cf_ * pole_frequency + 2 * M_PI * min_bandwidth_hz_ / sample_rate_;
    pole_frequency -= step_factor_ * bw;
  }
  return true;
}


bool ModulePZFC::SetPZBankCoeffsERB() {
  // This function sets the following variables:
  // channel_count_
  // pole_dampings_
  // pole_frequencies_
  // za0_, za1_, za2
  // output_
  
  // TODO(tomwalters): There's significant code-duplication between here,
  // SetPZBankCoeffsERBFitted, and SetPZBankCoeffs
  
  // Normalised maximum pole frequency
  float pole_frequency = cf_max_ / sample_rate_ * (2.0f * M_PI);
  channel_count_ = 0;
  while ((pole_frequency / (2.0f * M_PI)) * sample_rate_ > cf_min_) {
    float bw = ERBTools::Freq2ERBw(pole_frequency
                                  / (2.0f * M_PI) * sample_rate_);
    pole_frequency -= step_factor_ * (bw * (2.0f * M_PI) / sample_rate_);
    channel_count_++;
  }
  
  // Now the number of channels is known, various buffers for the filterbank
  // coefficients can be initialised
  pole_dampings_.clear();
  pole_dampings_.resize(channel_count_, pole_damping_);
  pole_frequencies_.clear();
  pole_frequencies_.resize(channel_count_, 0.0f);

  // Direct-form coefficients
  za0_.clear();
  za0_.resize(channel_count_, 0.0f);
  za1_.clear();
  za1_.resize(channel_count_, 0.0f);
  za2_.clear();
  za2_.resize(channel_count_, 0.0f);

  // The output signal bank
  output_.Initialize(channel_count_, buffer_length_, sample_rate_);
  
  // Reset the pole frequency to maximum
  pole_frequency = cf_max_ / sample_rate_ * (2.0f * M_PI);

  for (int i = channel_count_ - 1; i > -1; --i) {
    // Store the normalised pole frequncy
    pole_frequencies_[i] = pole_frequency;

    // Calculate the real pole frequency from the normalised pole frequency
    float frequency = pole_frequency / (2.0f * M_PI) * sample_rate_;

    // Store the real pole frequency as the 'centre frequency' of the filterbank
    // channel
    output_.set_centre_frequency(i, frequency);

  float zero_frequency = Minimum(M_PI, zero_factor_ * pole_frequency);

    // Impulse-invariance mapping
    float z_plane_theta = zero_frequency * sqrt(1.0f - pow(zero_damping_, 2));
    float z_plane_rho = exp(-zero_damping_ * zero_frequency);

    // Direct-form coefficients from z-plane rho and theta
    float a1 = -2.0f * z_plane_rho * cos(z_plane_theta);
    float a2 = z_plane_rho * z_plane_rho;

    // Normalised to unity gain at DC
    float a_sum = 1.0f + a1 + a2;
    za0_[i] = 1.0f / a_sum;
    za1_[i] = a1 / a_sum;
    za2_[i] = a2 / a_sum;

    float bw = ERBTools::Freq2ERBw(pole_frequency
                                  / (2.0f * M_PI) * sample_rate_);
    pole_frequency -= step_factor_ * (bw * (2.0f * M_PI) / sample_rate_);
  }
  return true;
}

bool ModulePZFC::SetPZBankCoeffsERBFitted() {
  //float parameter_values[3 * 7] = {
    //// Filed, Nfit = 524, 11-3 parameters, PZFC, cwt 0, fit time 9915 sec
    //1.14827,   0.00000,   0.00000,  // % SumSqrErr=  10125.41
    //0.53571,  -0.70128,   0.63246,  // % RMSErr   =   2.81586
    //0.76779,   0.00000,   0.00000,  // % MeanErr  =   0.00000
    //// Inf   0.00000   0.00000 % RMSCost  = NaN
    //0.00000,   0.00000,   0.00000,
    //6.00000,   0.00000,   0.00000,
    //1.08869,  -0.09470,   0.07844,
    //10.56432,   2.52732,   1.86895
    //// -3.45865  -1.31457   3.91779 % Kv
  //};
  
  float parameter_values[3 * 7] = {
  // Fit 515 from Dick
  // Final, Nfit = 515, 9-3 parameters, PZFC, cwt 0
     1.72861,   0.00000,   0.00000,  // SumSqrErr =  13622.24
     0.56657,  -0.93911,   0.89163,  // RMSErr    =  3.26610
     0.39469,   0.00000,   0.00000,  // MeanErr   =  0.00000
  // Inf,       0.00000,   0.00000,  // RMSCost   =  NaN - would set coefc to infinity, but this isn't passed on
     0.00000,   0.00000,   0.00000,
     2.00000,   0.00000,   0.00000,  //
     1.27393,   0.00000,   0.00000,
    11.46247,  5.46894,   0.11800
  // -4.15525,  1.54874,   2.99858   // Kv
  };

  // Precalculate the number of channels required - this method is ugly but it
  // was the quickest way of converting from MATLAB as the step factor between
  // channels can vary quadratically with pole frequency...

  // Normalised maximum pole frequency
  float pole_frequency = cf_max_ / sample_rate_ * (2.0f * M_PI);

  channel_count_ = 0;
  while ((pole_frequency / (2.0f * M_PI)) * sample_rate_ > cf_min_) {
    float frequency = pole_frequency / (2.0f * M_PI) * sample_rate_;
    float f_dep = ERBTools::Freq2ERB(frequency)
                  / ERBTools::Freq2ERB(1000.0f) - 1.0f;
    float bw = ERBTools::Freq2ERBw(pole_frequency
                                  / (2.0f * M_PI) * sample_rate_);
    float step_factor = 1.0f
      / (parameter_values[4*3] + parameter_values[4 * 3 + 1]
      * f_dep + parameter_values[4 * 3 + 2] * f_dep * f_dep);  // 1/n2
    pole_frequency -= step_factor * (bw * (2.0f * M_PI) / sample_rate_);
    channel_count_++;
  }

  // Now the number of channels is known, various buffers for the filterbank
  // coefficients can be initialised
  pole_dampings_.clear();
  pole_dampings_.resize(channel_count_, 0.0f);
  pole_frequencies_.clear();
  pole_frequencies_.resize(channel_count_, 0.0f);

  // Direct-form coefficients
  za0_.clear();
  za0_.resize(channel_count_, 0.0f);
  za1_.clear();
  za1_.resize(channel_count_, 0.0f);
  za2_.clear();
  za2_.resize(channel_count_, 0.0f);

  // The output signal bank
  output_.Initialize(channel_count_, buffer_length_, sample_rate_);

  // Reset the pole frequency to maximum
  pole_frequency = cf_max_ / sample_rate_ * (2.0f * M_PI);

  for (int i = channel_count_ - 1; i > -1; --i) {
    // Store the normalised pole frequncy
    pole_frequencies_[i] = pole_frequency;

    // Calculate the real pole frequency from the normalised pole frequency
    float frequency = pole_frequency / (2.0f * M_PI) * sample_rate_;

    // Store the real pole frequency as the 'centre frequency' of the filterbank
    // channel
    output_.set_centre_frequency(i, frequency);

    // From PZFC_Small_Signal_Params.m { From PZFC_Params.m {
    float DpndF = ERBTools::Freq2ERB(frequency)
                  / ERBTools::Freq2ERB(1000.0f) - 1.0f;

    float p[8];  // Parameters (short name for ease of reading)

    // Use parameter_values to recover the parameter values for this frequency
    for (int param = 0; param < 7; ++param)
      p[param] = parameter_values[param * 3]
                 + parameter_values[param * 3 + 1] * DpndF
                 + parameter_values[param * 3 + 2] * DpndF * DpndF;

    // Calculate the final parameter
    p[7] = p[1] * pow(10.0f, (p[2] / (p[1] * p[4])) * (p[6] - 60.0f) / 20.0f);
    if (p[7] < 0.2f)
      p[7] = 0.2f;

    // Nominal bandwidth at this frequency
    float fERBw = ERBTools::Freq2ERBw(frequency);

    // Pole bandwidth
    float fPBW = ((p[7] * fERBw * (2 * M_PI) / sample_rate_) / 2)
                 * pow(p[4], 0.5f);

    // Pole damping
    float pole_damping = fPBW / sqrt(pow(pole_frequency, 2) + pow(fPBW, 2));

    // Store the pole damping
    pole_dampings_[i] = pole_damping;

    // Zero bandwidth
    float fZBW = ((p[0] * p[5] * fERBw * (2 * M_PI) / sample_rate_) / 2)
                 * pow(p[4], 0.5f);

    // Zero frequency
    float zero_frequency = p[5] * pole_frequency;

    if (zero_frequency > M_PI)
      LOG_ERROR(_T("Warning: Zero frequency is above the Nyquist frequency "
                   "in ModulePZFC(), continuing anyway but results may not "
                   "be accurate."));

    // Zero damping
    float fZDamp = fZBW / sqrt(pow(zero_frequency, 2) + pow(fZBW, 2));

    // Impulse-invariance mapping
    float fZTheta = zero_frequency * sqrt(1.0f - pow(fZDamp, 2));
    float fZRho = exp(-fZDamp * zero_frequency);

    // Direct-form coefficients
    float fA1 = -2.0f * fZRho * cos(fZTheta);
    float fA2 = fZRho * fZRho;

    // Normalised to unity gain at DC
    float fASum = 1.0f + fA1 + fA2;
    za0_[i] = 1.0f / fASum;
    za1_[i] = fA1 / fASum;
    za2_[i] = fA2 / fASum;

    // Subtract step factor (1/n2) times current bandwidth from the pole
    // frequency
    pole_frequency -= ((1.0f / p[4])
                       * (fERBw * (2.0f * M_PI) / sample_rate_));
  }
return true;
}

bool ModulePZFC::SetPZBankCoeffs() {
  /*! \todo Re-implement the alternative parameter settings
   */
  if (use_fitted_parameters_) {
    if (!SetPZBankCoeffsERBFitted())
      return false;
  } else {
    if (!SetPZBankCoeffsOrig())
    return false;
  }

  /*! \todo Make fMindamp and fMaxdamp user-settable?
   */
  mindamp_ = 0.18f;
  maxdamp_ = 0.4f;

  rmin_.resize(channel_count_);
  rmax_.resize(channel_count_);
  xmin_.resize(channel_count_);
  xmax_.resize(channel_count_);

  for (int c = 0; c < channel_count_; ++c) {
    // Calculate maximum and minimum damping options
    rmin_[c] = exp(-mindamp_ * pole_frequencies_[c]);
    rmax_[c] = exp(-maxdamp_ * pole_frequencies_[c]);

    xmin_[c] = rmin_[c] * cos(pole_frequencies_[c]
                                      * pow((1-pow(mindamp_, 2)), 0.5f));
    xmax_[c] = rmax_[c] * cos(pole_frequencies_[c]
                                      * pow((1-pow(maxdamp_, 2)), 0.5f));
  }

  // Set up AGC parameters
  agc_stage_count_ = 4;
  agc_epsilons_.resize(agc_stage_count_);
  agc_epsilons_[0] = 0.0064f;
  agc_epsilons_[1] = 0.0016f;
  agc_epsilons_[2] = 0.0004f;
  agc_epsilons_[3] = 0.0001f;

  agc_gains_.resize(agc_stage_count_);
  agc_gains_[0] = 1.0f;
  agc_gains_[1] = 1.4f;
  agc_gains_[2] = 2.0f;
  agc_gains_[3] = 2.8f;

  float mean_agc_gain = 0.0f;
  for (int c = 0; c < agc_stage_count_; ++c)
    mean_agc_gain += agc_gains_[c];
  mean_agc_gain /= static_cast<float>(agc_stage_count_);

  for (int c = 0; c < agc_stage_count_; ++c)
    agc_gains_[c] /= mean_agc_gain;

  return true;
}

void ModulePZFC::AGCDampStep() {
  if (detect_.size() == 0) {
    // If  detect_ is not initialised, it means that the AGC is not set up.
    // Set up now.
    /*! \todo Make a separate InitAGC function which does this.
     */
    detect_.clear();
    float detect_zero = DetectFun(0.0f);
    detect_.resize(channel_count_, detect_zero);

    for (int c = 0; c < channel_count_; c++)
      for (int st = 0; st < agc_stage_count_; st++)
        agc_state_[c][st] = (1.2f * detect_[c] * agc_gains_[st]);
  }

  float fAGCEpsLeft = 0.3f;
  float fAGCEpsRight = 0.3f;

  for (int c = channel_count_ - 1; c > -1; --c) {
    for (int st = 0; st < agc_stage_count_; ++st) {
      // This bounds checking is ugly and wasteful, and in an inner loop.
      // If this algorithm is slow, this is why!
      /*! \todo Proper non-ugly bounds checking in AGCDampStep()
       */
      float fPrevAGCState;
      float fCurrAGCState;
      float fNextAGCState;

      if (c < channel_count_ - 1)
        fPrevAGCState = agc_state_[c + 1][st];
      else
        fPrevAGCState = agc_state_[c][st];

      fCurrAGCState = agc_state_[c][st];

      if (c > 0)
        fNextAGCState = agc_state_[c - 1][st];
      else
        fNextAGCState = agc_state_[c][st];

      // Spatial smoothing
      /*! \todo Something odd is going on here
       *  I think this line is not quite right.
       */
      float agc_avg = fAGCEpsLeft * fPrevAGCState
                      + (1.0f - fAGCEpsLeft - fAGCEpsRight) * fCurrAGCState
                      + fAGCEpsRight * fNextAGCState;
      // Temporal smoothing
      agc_state_[c][st] = agc_avg * (1.0f - agc_epsilons_[st])
                          + agc_epsilons_[st] * detect_[c] * agc_gains_[st];
    }
  }

  float offset = 1.0f - agc_factor_ * DetectFun(0.0f);

  for (int i = 0; i < channel_count_; ++i) {
    float fAGCStateMean = 0.0f;
    for (int j = 0; j < agc_stage_count_; ++j)
     fAGCStateMean += agc_state_[i][j];

    fAGCStateMean /= static_cast<float>(agc_stage_count_);

    pole_damps_mod_[i] = pole_dampings_[i] *
                           (offset + agc_factor_ * fAGCStateMean);
  }
}

float ModulePZFC::DetectFun(float fIN) {
  if (fIN < 0.0f)
    fIN = 0.0f;
  float fDetect = Minimum(1.0f, fIN);
  float fA = 0.25f;
  return fA * fIN + (1.0f - fA) * (fDetect - pow(fDetect, 3) / 3.0f);
}

inline float ModulePZFC::Minimum(float a, float b) {
  if (a < b)
    return a;
  else
    return b;
}

void ModulePZFC::Process(const SignalBank& input) {
  // Set the start time of the output buffer
  output_.set_start_time(input.start_time());

  for (int s = 0; s < input.buffer_length(); ++s) {
    float input_sample = input.sample(0, s);

    // Lowpass filter the input with a zero at PI
    input_sample = 0.5f * input_sample + 0.5f * last_input_;
    last_input_ = input.sample(0, s);

    inputs_[channel_count_ - 1] = input_sample;
    for (int c = 0; c < channel_count_ - 1; ++c)
      inputs_[c] = previous_out_[c + 1];

    // PZBankStep2
    // to save a bunch of divides
    float damp_rate = 1.0f / (maxdamp_ - mindamp_);

    for (int c = channel_count_ - 1; c > -1; --c) {
      float interp_factor = (pole_damps_mod_[c] - mindamp_) * damp_rate;

      float x = xmin_[c] + (xmax_[c] - xmin_[c]) * interp_factor;
      float r = rmin_[c] + (rmax_[c] - rmin_[c]) * interp_factor;

      // optional improvement to constellation adds a bit to r
      float fd = pole_frequencies_[c] * pole_damps_mod_[c];
      // quadratic for small values, then linear
      r = r + 0.25f * fd * Minimum(0.05f, fd);

      float zb1 = -2.0f * x;
      float zb2 = r * r;

      /* canonic poles but with input provided where unity DC gain is assured
       * (mean value of state is always equal to mean value of input)
       */
      float new_state = inputs_[c] - (state_1_[c] - inputs_[c]) * zb1
                                   - (state_2_[c] - inputs_[c]) * zb2;

      // canonic zeros part as before:
      float output = za0_[c] * new_state + za1_[c] * state_1_[c]
                                         + za2_[c] * state_2_[c];

      // cubic compression nonlinearity
      output -= 0.0001f * pow(output, 3);

      output_.set_sample(c, s, output);
      detect_[c] = DetectFun(output);
      state_2_[c] = state_1_[c];
      state_1_[c] = new_state;
    }

    if (do_agc_step_)
      AGCDampStep();

    for (int c = 0; c < channel_count_; ++c)
      previous_out_[c] = output_[c][s];
  }
  PushOutput();
}
}  // namespace aimc
