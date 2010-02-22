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

#include "Modules/Profile/ModuleSlice.h"

namespace aimc {
ModuleSlice::ModuleSlice(Parameters *params) : Module(params) {
  module_description_ = "Temporal or spectral slice of a 2D image";
  module_identifier_ = "slice";
  module_type_ = "profile";
  module_version_ = "$Id$";

  // This module will compute the spectral profile unless told otherwise here
  temporal_profile_ = parameters_->DefaultBool("slice.temporal", false);
  // Set slice.all to true to take the profile of the entire image
  take_all_ = parameters_->DefaultBool("slice.all", true);
  // If not taking all, then these give the lower and upper indices of the
  // section to take. They are bounds-checked.
  lower_limit_ = parameters_->DefaultInt("slice.lower_index", 0);
  upper_limit_ = parameters_->DefaultInt("slice.upper_index", 1000);
  // Set to true to normalize the slice taken (ie take the mean value)
  normalize_slice_ = parameters_->DefaultBool("slice.normalize", false);
}

ModuleSlice::~ModuleSlice() {
}

bool ModuleSlice::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();

  if (lower_limit_ < 0) {
    lower_limit_ = 0;
  }

  if (upper_limit_ < 0) {
    upper_limit_ = 0;
  }

  if (temporal_profile_) {
    if (upper_limit_ > channel_count_) {
      upper_limit_ = channel_count_;
    }
    if (lower_limit_ > channel_count_) {
      lower_limit_ = channel_count_;
    }
  } else {
    if (upper_limit_ > buffer_length_) {
      upper_limit_ = buffer_length_;
    }
    if (lower_limit_ > buffer_length_) {
      lower_limit_ = buffer_length_;
    }
  }

  slice_length_ = upper_limit_ - lower_limit_;
  if (slice_length_ < 1) {
    slice_length_ = 1;
  }

  if (temporal_profile_) {
    output_.Initialize(1, buffer_length_, sample_rate_);
  } else {
    output_.Initialize(channel_count_, 1, sample_rate_);
  }
  return true;
}

void ModuleSlice::ResetInternal() {
}

void ModuleSlice::Process(const SignalBank &input) {
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

  if (temporal_profile_) {
    for (int i = 0; i < input.buffer_length(); ++i) {
      float val = 0.0f;
      for (int ch = lower_limit_; ch < upper_limit_; ++ch) {
        val += input.sample(ch, i);
      }
      if (normalize_slice_) {
        val /= static_cast<float>(slice_length_);
      }
      output_.set_sample(0, i, val);
    }
  } else {
    for (int ch = 0; ch < input.channel_count(); ++ch) {
      output_.set_centre_frequency(ch, input.centre_frequency(ch));
      float val = 0.0f;
      for (int i = lower_limit_; i < upper_limit_; ++i) {
        val += input.sample(ch, i);
      }
      if (normalize_slice_) {
        val /= static_cast<float>(slice_length_);
      }
      output_.set_sample(ch, 0, val);
    }
  }
  PushOutput();
}
}  // namespace aimc

