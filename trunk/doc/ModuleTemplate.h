// Copyright #YEAR#, #AUTHOR_NAME#
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
 * \author #AUTHOR_NAME# <#AUTHOR_EMAIL_ADDRESS#>
 * \date created #TODAYS_DATE#
 * \version \$Id$
 */

#ifndef AIMC_MODULES_#MODULE_TYPE_CAPS#_#MODULE_ID_CAPS#_H_
#define AIMC_MODULES_#MODULE_TYPE_CAPS#_#MODULE_ID_CAPS#_H_

#include "Support/Module.h"

namespace aimc {
class #MODULE_NAME# : public Module {
 public:
  explicit #MODULE_NAME#(Parameters *pParam);
  virtual ~#MODULE_NAME#();

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
};
}  // namespace aimc

#endif  // AIMC_MODULES_#MODULE_TYPE_CAPS#_#MODULE_ID_CAPS#_H_
