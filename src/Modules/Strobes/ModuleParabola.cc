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
 * \brief Parabola strobe detection module - Using the 'parabloa' strobe
 * criterion from the AIM-MAT sf2003 module
 *
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2007/08/01
 * \version \$Id$
 */

#include <limits.h>
#include <cmath>

#include "Modules/Strobes/ModuleParabola.h"

namespace aimc {
ModuleParabola::ModuleParabola(Parameters *params) : Module(params) {
  module_description_ = "sf2003 parabola algorithm";
  module_identifier_ = "parabola";
  module_type_ = "strobes";
  module_version_ = "$Id$";

  // Get data from parameters
  height_ = parameters_->DefaultFloat("parabola.height", 1.2f);
  parabw_ = parameters_->DefaultFloat("parabola.width_cycles", 1.5f);
  strobe_decay_time_ = parameters_->DefaultFloat("parabla.strobe_decay_time",
                                                 0.02f);
  channel_count_ = 0;
}

bool ModuleParabola::InitializeInternal(const SignalBank &input) {
  output_.Initialize(input);
  channel_count_ = input.channel_count();
  sample_rate_ = input.sample_rate();

  // Parameters for the parabola
  parab_a_.resize(channel_count_);
  parab_b_.resize(channel_count_);
  parab_wnull_.resize(channel_count_);
  parab_var_samples_.resize(channel_count_);

  for (int ch = 0; ch < channel_count_; ++ch) {
    parab_wnull_[ch] = parabw_ / input.centre_frequency(ch);
    parab_var_samples_[ch] = floor(parab_wnull_[ch] * sample_rate_);
    parab_a_[ch] = 4.0f * (1.0f - height_)
                   / (parab_wnull_[ch] * parab_wnull_[ch]);
    parab_b_[ch] = -parab_wnull_[ch] / 2.0f;
  }

  // Number of samples over which the threshold should decay
  strobe_decay_samples_ = floor(sample_rate_ * strobe_decay_time_);

  // Prepare internal buffers
  ResetInternal();

  return true;
}

void ModuleParabola::ResetInternal() {
  threshold_.resize(channel_count_, 0.0f);
  last_threshold_.resize(channel_count_, 0.0f);
  samples_since_last_strobe_.resize(channel_count_, 0);

  prev_sample_.resize(channel_count_, 10000.0f);
  curr_sample_.resize(channel_count_, 5000.0f);
  next_sample_.resize(channel_count_, 0.0f);
}

void ModuleParabola::Process(const SignalBank &input) {
  float decay_constant;

  for (int ch = 0; ch < output_.channel_count(); ch++) {
    output_.ResetStrobes(ch);
  }
  output_.set_start_time(input.start_time());

  // Loop across samples first, then channels
  for (int i = 0; i < input.buffer_length(); i++) {
    // Find strobes in each channel first
    for (int ch = 0; ch < input.channel_count(); ++ch) {
      // Shift all the samples by one
      // curr_sample is the sample at time (i - 1)
      prev_sample_[ch] = curr_sample_[ch];
      curr_sample_[ch] = next_sample_[ch];
      next_sample_[ch] = input.sample(ch, i);

      // Copy input signal to output signal
      output_.set_sample(ch, i, input.sample(ch, i));

      if (curr_sample_[ch] >= threshold_[ch]) {
        threshold_[ch] = curr_sample_[ch];
        if (prev_sample_[ch] < curr_sample_[ch]
            && next_sample_[ch] < curr_sample_[ch]) {
          // We have a strobe: set threshold and add strobe to the list
          output_.AddStrobe(ch, i - 1);
          last_threshold_[ch] = threshold_[ch];
          parab_var_samples_[ch] =
            floor(input.sample_rate()
                  * (parab_wnull_[ch] - (threshold_[ch]
                                         - 2.0f * parab_a_[ch]  *parab_b_[ch])
                                        / (2.0f * parab_a_[ch])));
        }
      }
      if (output_.strobe_count(ch) > 0) {
        samples_since_last_strobe_[ch] = (i - 1)
             - output_.strobe(ch, output_.strobe_count(ch) - 1);
      } else {
        samples_since_last_strobe_[ch] = UINT_MAX;
      }

      if (samples_since_last_strobe_[ch] > parab_var_samples_[ch]) {
        decay_constant = last_threshold_[ch] / strobe_decay_samples_;
        if (threshold_[ch] > decay_constant)
          threshold_[ch] -= decay_constant;
        else
          threshold_[ch] = 0.0f;
      } else {
        threshold_[ch] = last_threshold_[ch]
            * (parab_a_[ch] * pow((samples_since_last_strobe_[ch]
                                   / input.sample_rate() + parab_b_[ch]),
                                  2.0f) + height_);
      }
    }
  }

  PushOutput();
}



ModuleParabola::~ModuleParabola() {
}
}  // namespace aimc
