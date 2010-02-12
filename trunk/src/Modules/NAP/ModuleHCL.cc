// Copyright 2007-2010, Thomas Walters
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
 * \file
 * \brief Halfwave rectification, compression and lowpass filtering.
 *
 * \author Tom Walters <tcw24@cam.ac.uk>
 * \date created 2007/03/07
 * \version \$Id: ModuleHCL.cc 4 2010-02-03 18:44:58Z tcw $
 */

#include <math.h>

#include "Modules/NAP/ModuleHCL.h"

namespace aimc {
ModuleHCL::ModuleHCL(Parameters *parameters) : Module(parameters) {
  module_identifier_ = "hcl";
  module_type_ = "nap";
  module_description_ = "Halfwave rectification, compression "
                        "and lowpass filtering";
  module_version_ = "$Id: ModuleHCL.cc 4 2010-02-03 18:44:58Z tcw $";

  parameters_->SetDefault("nap.do_lowpass", "false");
  parameters_->SetDefault("nap.do_log_compression", "false");
  parameters_->SetDefault("nap.lowpass_cutoff", "1200.0");
  parameters_->SetDefault("nap.lowpass_order", "2");

  do_lowpass_ = parameters_->GetBool("nap.do_lowpass");
  do_log_ = parameters_->GetBool("nap.do_log_compression");
  lowpass_cutoff_ = parameters_->GetFloat("nap.lowpass_cutoff");
  lowpass_order_ = parameters_->GetInt("nap.lowpass_order");
}

ModuleHCL::~ModuleHCL() {
}

bool ModuleHCL::InitializeInternal(const SignalBank &input) {
  time_constant_ = 1.0f / (2.0f * M_PI * lowpass_cutoff_);
  channel_count_ = input.channel_count();
  output_.Initialize(input);
  Reset();
  return true;
}

void ModuleHCL::Reset() {
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
          s *= pow(2.0f,15);
          if (s < 1.0f) s = 1.0f;
          s = 20.0f * log10(s);
        }
        output_.set_sample(c, i, s);
      }
    }
    if (do_lowpass_) {
      float b = exp( -1.0f / (input.sample_rate() * time_constant_));
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
