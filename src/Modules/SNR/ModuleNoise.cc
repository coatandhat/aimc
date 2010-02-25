// Copyright 2010, Thomas Walters
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

/*!
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2010/02/24
 * \version \$Id$
 */

// Use the boost RNGs to generate Gaussian noise
#include <boost/random.hpp>
#include <math.h>

#include "Modules/SNR/ModuleNoise.h"

namespace aimc {
ModuleNoise::ModuleNoise(Parameters *params) :
    Module(params),
    gaussian_variate_(boost::mt19937(),
                      boost::normal_distribution<float>(0.0f, 1.0f)) {
  module_description_ = "Adds noise to a signal";
  module_identifier_ = "noise";
  module_type_ = "snr";
  module_version_ = "$Id$";

  // Noise level relative to unit-variance Gaussian noise (ie. 0dB will give a
  // noise with an RMS level of 1.0)
  float snr_db = parameters_->DefaultFloat("noise.level_db", 0.0f);
  multiplier_ = pow(10.0f, snr_db / 20.0f);
}

ModuleNoise::~ModuleNoise() {
}

bool ModuleNoise::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();

  output_.Initialize(input);
  return true;
}

void ModuleNoise::ResetInternal() {

}

void ModuleNoise::Process(const SignalBank &input) {
  // Check to see if the module has been initialized. If not, processing
  // should not continue.
  if (!initialized_) {
    LOG_ERROR(_T("Module %s not initialized."), module_identifier_.c_str());
    return;
  }

  // Check that ths input this time is the same as the input passed to
  // Initialize()
  if (buffer_length_ != input.buffer_length()
      || channel_count_ != input.channel_count()) {
    LOG_ERROR(_T("Mismatch between input to Initialize() and input to "
                 "Process() in module %s."), module_identifier_.c_str());
    return;
  }

  for (int c = 0; c < input.channel_count(); ++c) {
    for (int i = 0; i < input.buffer_length(); ++i) {
      float s = input[c][i];
      s += (multiplier_ * gaussian_variate_());
      output_.set_sample(c, i, s);
    }
  }
  PushOutput();
}
}  // namespace aimc

