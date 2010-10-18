// Copyright 2009-2010, Thomas Walters
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
 *  \brief Slaney's gammatone filterbank
 *
 *  \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2009/11/13
 *  \version \$Id$
 */

#include <cmath>
#include <complex>
#include "Support/ERBTools.h"

#include "Modules/BMM/ModuleGammatone.h"

namespace aimc {
using std::vector;
using std::complex;
ModuleGammatone::ModuleGammatone(Parameters *params) : Module(params) {
  module_identifier_ = "gt";
  module_type_ = "bmm";
  module_description_ = "Gammatone filterbank (Slaney's IIR gammatone)";
  module_version_ = "$Id$";

  num_channels_ = parameters_->DefaultInt("gtfb.channel_count", 200);
  min_frequency_ = parameters_->DefaultFloat("gtfb.min_frequency", 86.0f);
  max_frequency_ = parameters_->DefaultFloat("gtfb.max_frequency", 16000.0f);
}

ModuleGammatone::~ModuleGammatone() {
}

void ModuleGammatone::ResetInternal() {
  state_1_.resize(num_channels_);
  state_2_.resize(num_channels_);
  state_3_.resize(num_channels_);
  state_4_.resize(num_channels_);
  for (int i = 0; i < num_channels_; ++i) {
    state_1_[i].clear();
    state_1_[i].resize(3, 0.0f);
    state_2_[i].clear();
    state_2_[i].resize(3, 0.0f);
    state_3_[i].clear();
    state_3_[i].resize(3, 0.0f);
    state_4_[i].clear();
    state_4_[i].resize(3, 0.0f);
  }
}

bool ModuleGammatone::InitializeInternal(const SignalBank& input) {
  // Calculate number of channels, and centre frequencies
  float erb_max = ERBTools::Freq2ERB(max_frequency_);
  float erb_min = ERBTools::Freq2ERB(min_frequency_);
  float delta_erb = (erb_max - erb_min) / (num_channels_ - 1);

  centre_frequencies_.resize(num_channels_);
  float erb_current = erb_min;

  output_.Initialize(num_channels_,
                     input.buffer_length(),
                     input.sample_rate());

  for (int i = 0; i < num_channels_; ++i) {
    centre_frequencies_[i] = ERBTools::ERB2Freq(erb_current);
    erb_current += delta_erb;
    output_.set_centre_frequency(i, centre_frequencies_[i]);
  }

  a_.resize(num_channels_);
  b1_.resize(num_channels_);
  b2_.resize(num_channels_);
  b3_.resize(num_channels_);
  b4_.resize(num_channels_);
  state_1_.resize(num_channels_);
  state_2_.resize(num_channels_);
  state_3_.resize(num_channels_);
  state_4_.resize(num_channels_);

  for (int ch = 0; ch < num_channels_; ++ch) {
    double cf = centre_frequencies_[ch];
    double erb = ERBTools::Freq2ERBw(cf);
    // LOG_INFO("%e", erb);

    // Sample interval
    double dt = 1.0f / input.sample_rate();

    // Bandwidth parameter
    double b = 1.019f * 2.0f * M_PI * erb;

    // The following expressions are derived in Apple TR #35, "An
    // Efficient Implementation of the Patterson-Holdsworth Cochlear
    // Filter Bank" and used in Malcolm Slaney's auditory toolbox, where he
    // defines this alternaltive four stage cascade of second-order filters.

    // Calculate the gain:
    double cpt = cf * M_PI * dt;
    complex<double> exponent(0.0, 2.0 * cpt);
    complex<double> ec = exp(2.0 * exponent);
    complex<double> two_cf_pi_t(2.0 * cpt, 0.0);
    complex<double> two_pow(pow(2.0, (3.0 / 2.0)), 0.0);
    complex<double> p1 = -2.0 * ec * dt;
    complex<double> p2 = 2.0 * exp(-(b * dt) + exponent) * dt;
    complex<double> b_dt(b * dt, 0.0);

    double gain = abs(
      (p1 + p2 * (cos(two_cf_pi_t) - sqrt(3.0 - two_pow) * sin(two_cf_pi_t)))
      * (p1 + p2 * (cos(two_cf_pi_t) + sqrt(3.0 - two_pow) * sin(two_cf_pi_t)))
      * (p1 + p2 * (cos(two_cf_pi_t) - sqrt(3.0 + two_pow) * sin(two_cf_pi_t)))
      * (p1 + p2 * (cos(two_cf_pi_t) + sqrt(3.0 + two_pow) * sin(two_cf_pi_t)))
      / pow((-2.0 / exp(2.0 * b_dt) - 2.0 * ec + 2.0 * (1.0 + ec)
            / exp(b_dt)), 4));
    // LOG_INFO("%e", gain);

    // The filter coefficients themselves:
    const int coeff_count = 3;
    a_[ch].resize(coeff_count, 0.0f);
    b1_[ch].resize(coeff_count, 0.0f);
    b2_[ch].resize(coeff_count, 0.0f);
    b3_[ch].resize(coeff_count, 0.0f);
    b4_[ch].resize(coeff_count, 0.0f);
    state_1_[ch].resize(coeff_count, 0.0f);
    state_2_[ch].resize(coeff_count, 0.0f);
    state_3_[ch].resize(coeff_count, 0.0f);
    state_4_[ch].resize(coeff_count, 0.0f);

    double B0 = dt;
    double B2 = 0.0f;

    double B11 = -(2.0f * dt * cos(2.0f * cf * M_PI * dt) / exp(b * dt)
                   + 2.0f * sqrt(3 + pow(2.0f, 1.5f)) * dt
                       * sin(2.0f * cf * M_PI * dt) / exp(b * dt)) / 2.0f;
    double B12 = -(2.0f * dt * cos(2.0f * cf * M_PI * dt) / exp(b * dt)
                   - 2.0f * sqrt(3 + pow(2.0f, 1.5f)) * dt
                       * sin(2.0f * cf * M_PI * dt) / exp(b * dt)) / 2.0f;
    double B13 = -(2.0f * dt * cos(2.0f * cf * M_PI * dt) / exp(b * dt)
                   + 2.0f * sqrt(3 - pow(2.0f, 1.5f)) * dt
                       * sin(2.0f * cf * M_PI * dt) / exp(b * dt)) / 2.0f;
    double B14 = -(2.0f * dt * cos(2.0f * cf * M_PI * dt) / exp(b * dt)
                   - 2.0f * sqrt(3 - pow(2.0f, 1.5f)) * dt
                       * sin(2.0f * cf * M_PI * dt) / exp(b * dt)) / 2.0f;

    a_[ch][0] = 1.0f;
    a_[ch][1] = -2.0f * cos(2.0f * cf * M_PI * dt) / exp(b * dt);
    a_[ch][2] = exp(-2.0f * b * dt);
    b1_[ch][0] = B0 / gain;
    b1_[ch][1] = B11 / gain;
    b1_[ch][2] = B2 / gain;
    b2_[ch][0] = B0;
    b2_[ch][1] = B12;
    b2_[ch][2] = B2;
    b3_[ch][0] = B0;
    b3_[ch][1] = B13;
    b3_[ch][2] = B2;
    b4_[ch][0] = B0;
    b4_[ch][1] = B14;
    b4_[ch][2] = B2;
  }
  return true;
}

void ModuleGammatone::Process(const SignalBank &input) {
  output_.set_start_time(input.start_time());
  int audio_channel = 0;

  vector<vector<double> >::iterator b1 = b1_.begin();
  vector<vector<double> >::iterator b2 = b2_.begin();
  vector<vector<double> >::iterator b3 = b3_.begin();
  vector<vector<double> >::iterator b4 = b4_.begin();
  vector<vector<double> >::iterator a = a_.begin();
  vector<vector<double> >::iterator s1 = state_1_.begin();
  vector<vector<double> >::iterator s2 = state_2_.begin();
  vector<vector<double> >::iterator s3 = state_3_.begin();
  vector<vector<double> >::iterator s4 = state_4_.begin();

  // Temporary storage between filter stages
  vector<double> out(input.buffer_length());
  for (int ch = 0; ch < num_channels_;
       ++ch, ++b1, ++b2, ++b3, ++b4, ++a, ++s1, ++s2, ++s3, ++s4) {
    for (int i = 0; i < input.buffer_length(); ++i) {
      // Direct-form-II IIR filter
      double in = input.sample(audio_channel, i);
      out[i] = (*b1)[0] * in + (*s1)[0];
      for (unsigned int stage = 1; stage < s1->size(); ++stage)
        (*s1)[stage - 1] = (*b1)[stage] * in
                           - (*a)[stage] * out[i] + (*s1)[stage];
    }
    for (int i = 0; i < input.buffer_length(); ++i) {
      // Direct-form-II IIR filter
      double in = out[i];
      out[i] = (*b2)[0] * in + (*s2)[0];
      for (unsigned int stage = 1; stage < s2->size(); ++stage)
        (*s2)[stage - 1] = (*b2)[stage] * in
                           - (*a)[stage] * out[i] + (*s2)[stage];
    }
    for (int i = 0; i < input.buffer_length(); ++i) {
      // Direct-form-II IIR filter
      double in = out[i];
      out[i] = (*b3)[0] * in + (*s3)[0];
      for (unsigned int stage = 1; stage < s3->size(); ++stage)
        (*s3)[stage - 1] = (*b3)[stage] * in
                           - (*a)[stage] * out[i] + (*s3)[stage];
    }
    for (int i = 0; i < input.buffer_length(); ++i) {
      // Direct-form-II IIR filter
      double in = out[i];
      out[i] = (*b4)[0] * in + (*s4)[0];
      for (unsigned int stage = 1; stage < s4->size(); ++stage)
        (*s4)[stage - 1] = (*b4)[stage] * in
                           - (*a)[stage] * out[i] + (*s4)[stage];
      output_.set_sample(ch, i, out[i]);
    }
  }
  PushOutput();
}

}  // namespace aimc
