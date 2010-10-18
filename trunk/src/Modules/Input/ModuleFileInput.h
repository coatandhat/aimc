// Copyright 2006-2010, Willem van Engen, Thomas Walters
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
 *  \brief Audio file input
 *
 *  \author Willem van Engen <cnbh@willem.engen.nl>
 *  \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2006/09/21
 *  \version \$Id$
 */

#ifndef AIMC_MODULES_INPUT_FILEINPUT_H_
#define AIMC_MODULES_INPUT_FILEINPUT_H_

#include <sndfile.h>

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
class ModuleFileInput : public Module {
 public:
  explicit ModuleFileInput(Parameters *pParam);
  virtual ~ModuleFileInput();

  virtual void Process(const SignalBank &input);

 private:
  /*! \brief Prepare the module
   *  \param input Input SignalBank
   *  \param output true on success false on failure
   */
  virtual bool InitializeInternal(const SignalBank &input);

  /*! \brief Rewind to the start of the file
   */
  virtual void ResetInternal();

  /*! \brief File descriptor
   */
  SNDFILE *file_handle_;

  /*! \brief Current position in time of the file
   */
  int file_position_samples_;
  bool file_loaded_;
  int audio_channels_;
  int buffer_length_;
  float sample_rate_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_INPUT_FILEINPUT_H_
