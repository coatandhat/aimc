// Copyright 2010, Thomas Walters
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

  pink_ = parameters_->DefaultBool("noise.pink", true);
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
  ResetInternal();
  return true;
}

void ModuleNoise::ResetInternal() {
  s0_ = 0.0f;
  s1_ = 0.0f;
  s2_ = 0.0f;
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
      float n =  gaussian_variate_();
      if (pink_) {
        // Pink noise filter coefficients from 
        // ccrma.stanford.edu/~jos/sasp/Example_Synthesis_1_F_Noise.html
        // Smith, Julius O. Spectral Audio Signal Processing, October 2008
        // Draft, http://ccrma.stanford.edu/~jos/sasp/, online book, 
        // accessed 2010-02-27.
        float f = 0.049922035 * n + s0_;
        s0_ = -0.095993537 * n - (-2.494956002 * f) + s1_;
        s1_ = 0.050612699 * n - (2.017265875 * f) + s2_;
        s2_ = -0.004408786 * n - (-0.522189400 * f);
        n = f;
      }
      s += multiplier_ * n;
      output_.set_sample(c, i, s);
    }
  }
  PushOutput();
}
}  // namespace aimc

