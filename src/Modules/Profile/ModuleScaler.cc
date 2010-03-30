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
 * \date created 2010/02/22
 * \version \$Id$
 */

#include "Modules/Profile/ModuleScaler.h"

namespace aimc {
ModuleScaler::ModuleScaler(Parameters *params) : Module(params) {
  module_description_ = "Scale each value by the channel centre frequency";
  module_identifier_ = "scaler";
  module_type_ = "profile";
  module_version_ = "$Id$";
}

ModuleScaler::~ModuleScaler() {
}

bool ModuleScaler::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();
  output_.Initialize(channel_count_, buffer_length_, sample_rate_);
  return true;
}

void ModuleScaler::ResetInternal() {
}

void ModuleScaler::Process(const SignalBank &input) {
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

  for (int ch = 0; ch < input.channel_count(); ++ch) {
    float cf = input.centre_frequency(ch);
    for (int i = 0; i < input.buffer_length(); ++i) {
      output_.set_sample(ch, i, cf * input.sample(ch, i));
    }
  }
  PushOutput();
}
}  // namespace aimc

