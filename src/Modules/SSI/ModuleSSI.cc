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
 * \date created 2010/02/19
 * \version \$Id$
 */

#include <cmath>

#include "Modules/SSI/ModuleSSI.h"

namespace aimc {
ModuleSSI::ModuleSSI(Parameters *params) : Module(params) {
  module_description_ = "Size-shape image (aka the 'sscAI')";
  module_identifier_ = "ssi";
  module_type_ = "ssi";
  module_version_ = "$Id$";

  // do_pitch_cutoff_ = parameters_->DefaultBool("ssi.pitch_cutoff", false);
  ssi_width_cycles_ = parameters_->DefaultFloat("ssi.width_cycles", 20.0f);
}

ModuleSSI::~ModuleSSI() {
}

bool ModuleSSI::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();

  float lowest_cf = input.centre_frequency(0);
  ssi_width_samples_ = sample_rate_ * ssi_width_cycles_ / lowest_cf;
  if (ssi_width_samples_ > buffer_length_) {
    ssi_width_samples_ = buffer_length_;
    float cycles = ssi_width_samples_ * lowest_cf / sample_rate_;
    LOG_INFO(_T("Requested SSI width of %f cycles is too long for the "
                "input buffer length of %d samples. The SSI will be "
                "truncated at %d samples wide. This corresponds to a width "
                "of %f cycles."), ssi_width_cycles_, buffer_length_,
                ssi_width_samples_, cycles);
    ssi_width_cycles_ = cycles;
  }
  output_.Initialize(channel_count_, ssi_width_samples_, sample_rate_);
  return true;
}

void ModuleSSI::ResetInternal() {
}

void ModuleSSI::Process(const SignalBank &input) {
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

  output_.set_start_time(input.start_time());

  for (int ch = 0; ch < channel_count_; ++ch) {
    // Copy the buffer from input to output, addressing by h-value
    for (int i = 0; i < ssi_width_samples_; ++i) {
      float h = static_cast<float>(i) * ssi_width_cycles_
                / static_cast<float>(ssi_width_samples_);
      float cycle_samples = sample_rate_ / input.centre_frequency(ch);

      // The index into the input array is a floating-point number, which is
      // split into a whole part and a fractional part. The whole part and
      // fractional part are found, and are used to linearly interpolate
      // between input samples to yield an output sample.
      double whole_part;
      float frac_part = modf(h * cycle_samples, &whole_part);
      int sample = static_cast<int>(whole_part);

      float val;
      if (sample < buffer_length_ - 1) {
        float curr_sample = input.sample(ch, sample);
        float next_sample = input.sample(ch, sample + 1);
        val = curr_sample + frac_part * (next_sample - curr_sample);
      } else {
        val = 0.0f;
      }
      output_.set_sample(ch, i, val);
    }
  }
  PushOutput();
}
}  // namespace aimc

