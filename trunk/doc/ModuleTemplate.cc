// Copyright #YEAR#, #AUTHOR_NAME#
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
 * \author #AUTHOR_NAME# <#AUTHOR_EMAIL_ADDRESS#>
 * \date created #TODAYS_DATE#
 * \version \$Id$
 */

#include "Modules/#MODULE_TYPE#/#MODULE_NAME#.h"

namespace aimc {
#MODULE_NAME#::#MODULE_NAME#(Parameters *params) : Module(params) {
  module_description_ = "TODO: Short Description of the module";
  module_identifier_ = "TODO: one-word id for the module";
  module_type_ = "TODO: type code eg. bmm, sai";
  module_version_ = "$Id$";

  // Read parameter values from the parameter store. Setting any default
  // values as necessary. The module should set defaults for all parameters
  // that is uses here. The parameters_->DefaultType() methods look for a
  // parameter with a given name. If it already exists in the parameter
  // store, they return the current value. If the parameter doesn't already
  // exist, it is added, set to the default value given, and that value is
  // returned.
  // Examples:
  // integer_param_ = parameters_->DefaultInt("module.param_name", 4);
  // boolean_param_ = parameters_->DefaultBool("module.param_name", True);
  // float_param_ = parameters_->DefaultFloat("module.param_name", 4.4f);
}

#MODULE_NAME#::~#MODULE_NAME#() {
}

bool #MODULE_NAME#::InitializeInternal(const SignalBank &input) {
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

void #MODULE_NAME#::ResetInternal() {
  // Reset any internal state variables to their default values here. After a
  // call to ResetInternal(), the module should be in the same state as it is
  // just after a call to InitializeInternal().
}

void #MODULE_NAME#::Process(const SignalBank &input) {
  // Check to see if the module has been initialized. If not, processing
  // should not continue.
  if (!initialized_) {
    LOG_ERROR(_T("Module #MODULE_NAME# not initialized."));
    return;
  }

  // Check that ths input this time is the same as the input passed to
  // Initialize()
  if (buffer_length_ != input.buffer_length()
      || channel_count_ != input.channel_count()) {
    LOG_ERROR(_T("Mismatch between input to Initialize() and input to "
                 "Process() in module %s", module_identifier_));
    return;
  }

  // Input is read from the input signal bank using calls like
  // float value = input_.sample(channel_number, sample_index);

  // Output is fed into the output signal bank (assuming that it was
  // initialized during the call to InitializeInternal()) like this:
  // output_.set_sample(channel_number, sample_index, sample_value);

  // If the output bank is set up, a call to PushOutput() will pass the output
  // on to all the target modules of this module. PushOutput() can be called
  // multiple times within each call to Process().
  // Example:
  // PushOutput();
}
}  // namespace aimc

