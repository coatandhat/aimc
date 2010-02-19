// Copyright 2009-2010, Thomas Walters
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
  state_.resize(num_channels_);
  for (int i = 0; i < num_channels_; ++i) {
    state_[i].resize(9, 0.0f);
  }
}

bool ModuleGammatone::InitializeInternal(const SignalBank& input) {
  // Calculate number of channels, and centre frequencies
  float erb_max = ERBTools::Freq2ERB(max_frequency_);
  float erb_min = ERBTools::Freq2ERB(min_frequency_);
  float delta_erb = (erb_max - erb_min) / (num_channels_ - 1);

  centre_frequencies_.resize(num_channels_);
  float erb_current = erb_min;

  for (int i = 0; i < num_channels_; ++i) {
    centre_frequencies_[i] = ERBTools::ERB2Freq(erb_current);
    erb_current += delta_erb;
  }

  forward_.resize(num_channels_);
  back_.resize(num_channels_);
  state_.resize(num_channels_);

  for (int ch = 0; ch < num_channels_; ++ch) {
    float cf = centre_frequencies_[ch];
    float erb = ERBTools::Freq2ERBw(cf);

    // Sample interval
    float dt = 1.0f / input.sample_rate();

    // Bandwidth parameter
    float b = 1.019f * 2.0f * M_PI * erb;

    // All of the following expressions are derived in Apple TR #35, "An
    // Efficient Implementation of the Patterson-Holdsworth Cochlear
    // Filter Bank".

    // Calculate the gain:
    float cpt = cf * M_PI * dt;
    complex<float> exponent(0.0f, 2.0f * cpt);
    complex<float> ec = exp(2.0f * exponent);
    complex<float> two_cf_pi_t(2.0f * cpt, 0.0f);
    complex<float> two_pow(pow(2.0f, (3.0f / 2.0f)), 0.0f);
    complex<float> p = -2.0f * ec * dt
                       + 2.0f * exp(-(b * dt) + exponent) * dt;
    complex<float> b_dt(b * dt, 0.0f);

    float gain = abs(
      (p * (cos(two_cf_pi_t) - sqrt(3.0f - two_pow) * sin(two_cf_pi_t)))
      * (p * (cos(two_cf_pi_t) + sqrt(3.0f - two_pow) * sin(two_cf_pi_t)))
      * (p * (cos(two_cf_pi_t) - sqrt(3.0f + two_pow) * sin(two_cf_pi_t)))
      * (p * (cos(two_cf_pi_t) + sqrt(3.0f + two_pow) * sin(two_cf_pi_t)))
      / pow(-2.0f / exp(2.0f * b_dt) - 2.0f * ec + 2.0f * (1.0f + ec)
            / exp(b_dt), 4.0f));

    // The filter coefficients themselves:
    const int coeff_count = 9;
    forward_[ch].resize(coeff_count, 0.0f);
    back_[ch].resize(coeff_count, 0.0f);
    state_[ch].resize(coeff_count, 0.0f);

    forward_[ch][0] = pow(dt, 4.0f) / gain;
    forward_[ch][1] = (-4.0f * pow(dt, 4.0f) * cos(2.0f * cpt)
                      / exp(b * dt) / gain);
    forward_[ch][2] = (6.0f * pow(dt, 4.0f) * cos(4.0f * cpt)
                      / exp(2.0f * b * dt) / gain);
    forward_[ch][3] = (-4.0f * pow(dt, 4.0f) * cos(6.0f * cpt)
                      / exp(3.0f * b * dt) / gain);
    forward_[ch][4] = (pow(dt, 4.0f) * cos(8.0f * cpt)
                      / exp(4.0f * b * dt) / gain);
    // Note: the remainder of the forward vector is zero-padded

    back_[ch][0] = 1.0f;
    back_[ch][1] = -8.0f * cos(2.0f * cpt) / exp(b * dt);
    back_[ch][2] = (4.0f * (4.0f + 3.0f * cos(4.0f * cpt))
                    / exp(2.0f * b * dt));
    back_[ch][3] = (-8.0f * (6.0f * cos(2.0f * cpt) + cos(6.0f * cpt))
                    / exp(3.0f * b * dt));
    back_[ch][4] = (2.0f * (18.0f + 16.0f * cos(4.0f * cpt) + cos(8.0f * cpt))
                    / exp(4.0f * b * dt));
    back_[ch][5] = (-8.0f * (6.0f * cos(2.0f * cpt) + cos(6.0f * cpt))
                    / exp(5.0f * b * dt));
    back_[ch][6] = (4.0f * (4.0f + 3.0f * cos(4.0f * cpt))
                    / exp(6.0f * b * dt));
    back_[ch][7] = -8.0f * cos(2.0f * cpt) / exp(7.0f * b * dt);
    back_[ch][8] = exp(-8.0f * b * dt);
  }
  output_.Initialize(num_channels_,
                     input.buffer_length(),
                     input.sample_rate());
  return true;
}

void ModuleGammatone::Process(const SignalBank &input) {
  output_.set_start_time(input.start_time());
  int audio_channel = 0;

  vector<vector<float> >::iterator b = forward_.begin();
  vector<vector<float> >::iterator a = back_.begin();
  vector<vector<float> >::iterator s = state_.begin();

  for (int ch = 0; ch < num_channels_; ++ch, ++a, ++b, ++s) {
    for (int i = 0; i < input.buffer_length(); ++i) {
      // Direct-form-II IIR filter
      float in = input.sample(audio_channel, i);
      float out = (*b)[0] * in + (*s)[0];
      for (unsigned int stage = 1; stage < s->size(); ++stage)
        (*s)[stage - 1] = (*b)[stage] * in - (*a)[stage] * out + (*s)[stage];
      output_.set_sample(ch, i, out);
    }
  }
  PushOutput();
}

}  // namespace aimc
