// Copyright 2006-2010, Thomas Walters
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
 *  \brief SAI module
 */

/*
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2007/08/29
 * \version \$Id$
 */
#include <cmath>

#include "Modules/SAI/ModuleSAI.h"

namespace aimc {
ModuleSAI::ModuleSAI(Parameters *parameters) : Module(parameters) {
  module_identifier_ = "weighted_sai";
  module_type_ = "sai";
  module_description_ = "Stabilised auditory image";
  module_version_ = "$Id$";

  min_delay_ms_ = parameters_->DefaultFloat("sai.min_delay_ms", 0.0f);
  max_delay_ms_ = parameters_->DefaultFloat("sai.max_delay_ms", 35.0f);
  strobe_weight_alpha_ = parameters_->DefaultFloat("sai.strobe_weight_alpha",
                                                   0.5f);
  buffer_memory_decay_ = parameters_->DefaultFloat("sai.buffer_memory_decay",
                                                   0.03f);
  frame_period_ms_ = parameters_->DefaultFloat("sai.frame_period_ms", 20.0f);

  max_concurrent_strobes_
    = parameters_->DefaultInt("sai.max_concurrent_strobes", 50);

  min_strobe_delay_idx_ = 0;
  max_strobe_delay_idx_ = 0;
  sai_decay_factor_ = 0.0f;
  fire_counter_ = 0;
}

bool ModuleSAI::InitializeInternal(const SignalBank &input) {
  // The SAI output bank must be as long as the SAI's Maximum delay.
  // One sample is added to the SAI buffer length to account for the
  // zero-lag point
  int sai_buffer_length = 1 + floor(input.sample_rate() * max_delay_ms_
                                    / 1000.0f);
  channel_count_ = input.channel_count();

  // Make an output SignalBank with the same number of channels and centre
  // frequencies as the input, but with a different buffer length
  if (!output_.Initialize(input.channel_count(),
                          sai_buffer_length,
                          input.sample_rate())) {
    LOG_ERROR("Failed to create output buffer in SAI module");
    return false;
  }
  for (int i = 0; i < input.channel_count(); ++i) {
    output_.set_centre_frequency(i, input.centre_frequency(i));
  }

  // sai_temp_ will be initialized to zero
  if (!sai_temp_.Initialize(output_)) {
    LOG_ERROR("Failed to create temporary buffer in SAI module");
    return false;
  }

  frame_period_samples_ = floor(input.sample_rate() * frame_period_ms_
                                / 1000.0f);
  min_strobe_delay_idx_ = floor(input.sample_rate() * min_delay_ms_
                                / 1000.0f);
  max_strobe_delay_idx_ = floor(input.sample_rate() * max_delay_ms_
                                / 1000.0f);

  // Make sure we don't go past the output buffer's upper bound
  if (max_strobe_delay_idx_ > output_.buffer_length()) {
    max_strobe_delay_idx_ = output_.buffer_length();
  }

  // Define decay factor from time since last sample (see ti2003)
  sai_decay_factor_ = pow(0.5f, 1.0f / (buffer_memory_decay_
                                        * input.sample_rate()));

  // Precompute strobe weights
  strobe_weights_.resize(max_concurrent_strobes_);
  for (int n = 0; n < max_concurrent_strobes_; ++n) {
    strobe_weights_[n] = pow(1.0f / (n + 1), strobe_weight_alpha_);
  }

  ResetInternal();

  return true;
}

void ModuleSAI::ResetInternal() {
  // Active Strobes
  output_.Clear();
  sai_temp_.Clear();
  active_strobes_.clear();
  active_strobes_.resize(channel_count_);
  fire_counter_ = frame_period_samples_ - 1;
}

void ModuleSAI::Process(const SignalBank &input) {
  // Reset the next strobe times
  next_strobes_.clear();
  next_strobes_.resize(output_.channel_count(), 0);

  // Offset the times on the strobes from the previous buffer
  for (int ch = 0; ch < input.channel_count(); ++ch) {
    active_strobes_[ch].ShiftStrobes(input.buffer_length());
  }

  // Loop over samples to make the SAI
  for (int i = 0; i < input.buffer_length(); ++i) {
    float decay_factor = pow(sai_decay_factor_, fire_counter_);
    // Loop over channels
    for (int ch = 0; ch < input.channel_count(); ++ch) {
      // Local convenience variables
      StrobeList &active_strobes = active_strobes_[ch];
      int next_strobe_index = next_strobes_[ch];

      // Update strobes
      // If we are up to or beyond the next strobe...
      if (next_strobe_index < input.strobe_count(ch)) {
        if (i == input.strobe(ch, next_strobe_index)) {
          // A new strobe has arrived.
          // If there are too many strobes active, then get rid of the
          // earliest one
          if (active_strobes.strobe_count() >= max_concurrent_strobes_) {
            active_strobes.DeleteFirstStrobe();
          }

          // Add the active strobe to the list of current strobes and
          // calculate the strobe weight
          float weight = 1.0f;
          if (active_strobes.strobe_count() > 0) {
            int last_strobe_time = active_strobes.Strobe(
              active_strobes.strobe_count() - 1).time;

            // If the strobe occured within 10 impulse-response
            // cycles of the previous strobe, then lower its weight
            weight = (i - last_strobe_time) / input.sample_rate()
                     * input.centre_frequency(ch) / 10.0f;
            if (weight > 1.0f)
              weight = 1.0f;
          }
          active_strobes.AddStrobe(i, weight);
          next_strobe_index++;


          // Update the strobe weights
          float total_strobe_weight = 0.0f;
          for (int si = 0; si < active_strobes.strobe_count(); ++si) {
            total_strobe_weight += (active_strobes.Strobe(si).weight
              * strobe_weights_[active_strobes.strobe_count() - si - 1]);
          }
          for (int si = 0; si < active_strobes.strobe_count(); ++si) {
            active_strobes.SetWorkingWeight(si,
              (active_strobes.Strobe(si).weight
               * strobe_weights_[active_strobes.strobe_count() - si - 1])
              / total_strobe_weight);
          }
        }
      }

      // Remove inactive strobes
      while (active_strobes.strobe_count() > 0) {
        // Get the relative time of the first strobe, and see if it exceeds
        // the maximum allowed time.
        if ((i - active_strobes.Strobe(0).time) > max_strobe_delay_idx_)
          active_strobes.DeleteFirstStrobe();
        else
          break;
      }

      // Update the SAI buffer with the weighted effect of all the active
      // strobes at the current sample
      for (int si = 0; si < active_strobes.strobe_count(); ++si) {
        // Add the effect of active strobe at correct place in the SAI buffer
        // Calculate 'delay', the time from the strobe event to now
        int delay = i - active_strobes.Strobe(si).time;

        // If the delay is greater than the (user-set)
        // minimum strobe delay, the strobe can be used
        if (delay >= min_strobe_delay_idx_ && delay < max_strobe_delay_idx_) {
          // The value at be added to the SAI
          float sig = input.sample(ch, i);

          // Weight the sample correctly
          sig *= active_strobes.Strobe(si).working_weight;

          // Adjust the weight acording to the number of samples until the
          // next output frame
          sig *= decay_factor;

          // Update the temporary SAI buffer
          sai_temp_.set_sample(ch, delay, sai_temp_.sample(ch, delay) + sig);
        }
      }

      next_strobes_[ch] = next_strobe_index;
    }  // End loop over channels

    fire_counter_--;

    // Check to see if we need to output an SAI frame on this sample
    if (fire_counter_ <= 0) {
      // Decay the SAI by the correct amount and add the current output frame
      float decay = pow(sai_decay_factor_, frame_period_samples_);

      for (int ch = 0; ch < input.channel_count(); ++ch) {
        for (int i = 0; i < output_.buffer_length(); ++i) {
          output_.set_sample(ch, i,
                             sai_temp_[ch][i] + output_[ch][i] * decay);
        }
      }

      // Zero the temporary signal
      for (int ch = 0; ch < sai_temp_.channel_count(); ++ch) {
        for (int i = 0; i < sai_temp_.buffer_length(); ++i) {
          sai_temp_.set_sample(ch, i, 0.0f);
        }
      }

      fire_counter_ = frame_period_samples_ - 1;

      // Transfer the current time to the output buffer
      output_.set_start_time(input.start_time() + i);
      PushOutput();
    }
  }  // End loop over samples
}

ModuleSAI::~ModuleSAI() {
}
}  // namespace aimc
