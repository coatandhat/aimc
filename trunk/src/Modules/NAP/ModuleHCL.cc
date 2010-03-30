// Copyright 2007-2010, Thomas Walters
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
 * \file
 * \brief Halfwave rectification, compression and lowpass filtering.
 *
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2007/03/07
 * \version \$Id$
 */

#include <cmath>

#include "Modules/NAP/ModuleHCL.h"

namespace aimc {
ModuleHCL::ModuleHCL(Parameters *parameters) : Module(parameters) {
  module_identifier_ = "hcl";
  module_type_ = "nap";
  module_description_ = "Halfwave rectification, compression "
                        "and lowpass filtering";
  module_version_ = "$Id$";

  do_lowpass_ = parameters_->DefaultBool("nap.do_lowpass", true);
  do_log_ = parameters_->DefaultBool("nap.do_log_compression", false);
  lowpass_cutoff_ = parameters_->DefaultFloat("nap.lowpass_cutoff", 1200.0);
  lowpass_order_ = parameters_->DefaultInt("nap.lowpass_order", 2);
}

ModuleHCL::~ModuleHCL() {
}

bool ModuleHCL::InitializeInternal(const SignalBank &input) {
  time_constant_ = 1.0f / (2.0f * M_PI * lowpass_cutoff_);
  channel_count_ = input.channel_count();
  output_.Initialize(input);
  ResetInternal();
  return true;
}

void ModuleHCL::ResetInternal() {
  xn_ = 0.0f;
  yn_ = 0.0f;
  yns_.clear();
  yns_.resize(channel_count_);
  for (int c = 0; c < channel_count_; ++c) {
    yns_[c].resize(lowpass_order_, 0.0f);
  }
}

/* With do_log, the signal is first scaled up so that values <1.0 become
 * negligible. This just rescales the sample values to fill the range of a
 * 16-bit signed integer, then we lose the bottom bit of resolution. If the
 * signal was sampled at 16-bit resolution, there shouldn't be anything to
 * speak of there anyway. If it was sampled using a higher resolution, then
 * some data will be discarded.
 */
void ModuleHCL::Process(const SignalBank &input) {
  output_.set_start_time(input.start_time());
  for (int c = 0; c < input.channel_count(); ++c) {
    for (int i = 0; i < input.buffer_length(); ++i) {
      if (input[c][i] < 0.0f) {
        output_.set_sample(c, i, 0.0f);
      } else {
        float s = input[c][i];
        if (do_log_) {
          s *= pow(2.0f, 15);
          if (s < 1.0f) s = 1.0f;
          s = 20.0f * log10(s);
        }
        output_.set_sample(c, i, s);
      }
    }
    if (do_lowpass_) {
      float b = exp(-1.0f / (input.sample_rate() * time_constant_));
      float gain = 1.0f / (1.0f - b);
      for (int j = 0; j < lowpass_order_; j++) {
        for (int k = 0; k < output_.buffer_length(); ++k) {
          xn_ = output_[c][k];
          yn_ = xn_ + b * yns_[c][j];
          yns_[c][j] = yn_;
          output_.set_sample(c, k, yn_ / gain);
        }
      }
    }
  }
  PushOutput();
}
}  // namespace aimc
