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
 * \file
 * \brief 
 *
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2010/02/23
 * \version \$Id$
 */

#include <math.h>
#include "Modules/Strobes/ModuleLocalMax.h"

namespace aimc {
ModuleLocalMax::ModuleLocalMax(Parameters *params) : Module(params) {
  module_description_ = "Local maximum strobe criterion: decaying threshold "
                        "with timeout";
  module_identifier_ = "local_max";
  module_type_ = "strobes";
  module_version_ = "$Id: $";

  decay_time_ms_ = parameters_->DefaultFloat("strobes.decay_time_ms", 20.0f);
  timeout_ms_ = parameters_->DefaultFloat("strobes.timeout_ms", 3.0f);
}

ModuleLocalMax::~ModuleLocalMax() {
}

bool ModuleLocalMax::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();
  output_.Initialize(input);
  strobe_timeout_samples_ = floor(timeout_ms_ * sample_rate_ / 1000.0f);
  strobe_decay_samples_ = floor(decay_time_ms_ * sample_rate_ / 1000.0f);
  ResetInternal();
  return true;
}

void ModuleLocalMax::ResetInternal() {
  threshold_.clear();
  threshold_.resize(channel_count_, 0.0f);

  decay_constant_.clear();
  decay_constant_.resize(channel_count_, 1.0f);

  prev_sample_.clear();
  prev_sample_.resize(channel_count_, 10000.0f);
  curr_sample_.clear();
  curr_sample_.resize(channel_count_, 5000.0f);
  next_sample_.clear();
  next_sample_.resize(channel_count_, 0.0f);
}

void ModuleLocalMax::Process(const SignalBank &input) {
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

  for (int ch = 0; ch < output_.channel_count(); ch++) {
    output_.ResetStrobes(ch);
  }
  output_.set_start_time(input.start_time());
  for (int i = 0; i < input.buffer_length(); i++) {
    for (int ch = 0; ch < input.channel_count(); ++ch) {
      // curr_sample is the sample at time (i - 1)
      prev_sample_[ch] = curr_sample_[ch];
      curr_sample_[ch] = next_sample_[ch];
      next_sample_[ch] = input.sample(ch, i);
      // Copy input signal to output signal
      output_.set_sample(ch, i, input.sample(ch, i));

      // If the current sample is above threshold, the threshold is raised to
      // the level of the current sample, and decays from there.
      if (curr_sample_[ch] >= threshold_[ch]) {
        threshold_[ch] = curr_sample_[ch];
        decay_constant_[ch] = threshold_[ch] / strobe_decay_samples_;

        // If the current sample is also a peak, then it is a potential strobe
        // point.
        if (prev_sample_[ch] < curr_sample_[ch]
            && next_sample_[ch] < curr_sample_[ch]) {
          // If there are no strobes so far in this channel, then the sample
          // is definitely a strobe (this means that the timeout is not 
          // respected across frame boundaries. This is a minor bug, but I
          // don't believe that it's serious enough to warrant updating the
          // samples since last strobe all the time.)
          int count = output_.strobe_count(ch);
          if (count > 0) {
            // If there are previous strobes, then calculate the time since
            // the last one. If it's long enough, then this is a strobe point,
            // if not, then just move on.
            int samples_since_last = (i - 1) - output_.strobe(ch, count - 1);
            if (samples_since_last > strobe_timeout_samples_) {
              output_.AddStrobe(ch, i - 1);
            }
          } else {
            output_.AddStrobe(ch, i - 1);
          }
        }
      }

      // Update the threshold, decaying as necessary
      if (threshold_[ch] > decay_constant_[ch])
        threshold_[ch] -= decay_constant_[ch];
      else
        threshold_[ch] = 0.0f;
    }
  }
  PushOutput();
}
}  // namespace aimc

