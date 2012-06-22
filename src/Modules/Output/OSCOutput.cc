// Copyright 2012, Tom Walters
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
 * \author Tom Walters <tom@acousticscale.org>
 * \date created 2012/02/17
 * \version \$Id$
 */

#include "Modules/Output/OSCOutput.h"

namespace aimc {
OSCOutput::OSCOutput(Parameters *params) : Module(params) {
  module_description_ = "OSC output";
  module_identifier_ = "osc_out";
  module_type_ = "output";
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
  // boolean_param_ = parameters_->DefaultBool("module.param_name", true);
  // float_param_ = parameters_->DefaultFloat("module.param_name", 4.4f);
  
  address_ = parameters_->DefaultString("osc.send_address", "127.0.0.1");
  port_ = parameters_->DefaultInt("osc.send_port", 6448);
  output_buffer_size_ = parameters_->DefaultInt("osc.output_min_buffer_size", 1024);
}

OSCOutput::~OSCOutput() {
}

bool OSCOutput::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();
  
  int output_byte_count = buffer_length_ * channel_count_ * 4 + 2048;  // TODO(tom) Find out what the real byte overhead is.
  if (output_buffer_size_ < output_byte_count) {
    output_buffer_size_ = output_byte_count;
  }
  LOG_INFO("Output buffer size is %d", output_buffer_size_);
  LOG_INFO("Feature count is %d", buffer_length_ * channel_count_);
  transmit_socket_ = new UdpTransmitSocket(IpEndpointName(address_.c_str(), port_));
  buffer_ = new char[output_buffer_size_];
  packet_stream_ = new osc::OutboundPacketStream(buffer_, output_buffer_size_);

  // If this module produces any output, then the output signal bank needs to
  // be initialized here.
  // Example:
  // output_.Initialize(channel_count, buffer_length, sample_rate);
  return true;
}

void OSCOutput::ResetInternal() {
  // Reset any internal state variables to their default values here. After a
  // call to ResetInternal(), the module should be in the same state as it is
  // just after a call to InitializeInternal().
}

void OSCOutput::Process(const SignalBank &input) {
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

  (*packet_stream_) << osc::BeginMessage( "/oscCustomFeatures" );
  for (int ch = 0; ch < input.channel_count(); ch++) {
    for (int i = 0; i < input.buffer_length(); i++) {
      float s = input.sample(ch, i);
      (*packet_stream_) << s;  
    }
  }
  (*packet_stream_) << osc::EndMessage;
  transmit_socket_->Send(packet_stream_->Data(), packet_stream_->Size());
  packet_stream_->Clear();

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

