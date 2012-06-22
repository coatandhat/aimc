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

#ifndef AIMC_MODULES_OUTPUT_OSC_H_
#define AIMC_MODULES_OUTPUT_OSC_H_

#include "Support/Module.h"

#include <string>

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

namespace aimc {
class OSCOutput : public Module {
 public:
  explicit OSCOutput(Parameters *pParam);
  virtual ~OSCOutput();

  /*! \brief Process a buffer
   */
  virtual void Process(const SignalBank &input);

 private:
  /*! \brief Reset the internal state of the module
   */
  virtual void ResetInternal();

  /*! \brief Prepare the module
   *  \param input Input signal
   *  \param output true on success false on failure
   */
  virtual bool InitializeInternal(const SignalBank &input);

  float sample_rate_;
  int buffer_length_;
  int channel_count_;
  string address_;
  int port_;
  int output_buffer_size_;
  
  UdpTransmitSocket* transmit_socket_;
  osc::OutboundPacketStream* packet_stream_;
  char* buffer_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_OUTPUT_OSC_H_
