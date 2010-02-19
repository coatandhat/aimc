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

#include "Modules/SSI/ModuleSSI.h"

namespace aimc {
ModuleSSI::ModuleSSI(Parameters *params) : Module(params) {
  module_description_ = "Size-shape image (aka the 'sscAI')";
  module_identifier_ = "ssi";
  module_type_ = "ssi";
  module_version_ = "$Id$";

  do_pitch_cutoff_ = parameters_->DefaultBool("ssi.pitch_cutoff", false);
}

ModuleSSI::~ModuleSSI() {
}

bool ModuleSSI::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();

  // If this module produces any output, then the output signal bank needs to
  // be initialized here.
  // Example:
  // output_.Initialize(channel_count, buffer_length, sample_rate);
  return true;
}

void ModuleSSI::ResetInternal() {
  // Reset any internal state variables to their default values here. After a
  // call to ResetInternal(), the module should be in the same state as it is
  // just after a call to InitializeInternal().
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

  // Input is read from the input signal bank using calls like
  // float value = input_.sample(channel_number, sample_index);

  // Output is fed into the output signal bank (assuming that it was
  // initialized during the call to InitializeInternal()) like this:
  // output_.set_sample(channel_number, sample_index, sample_value);

  PushOutput();
}
}  // namespace aimc

