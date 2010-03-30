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
 * \date created 2010/02/19
 * \version \$Id$
 */

#ifndef AIMC_MODULES_PROFILE_SCALER_H_
#define AIMC_MODULES_PROFILE_SCALER_H_

#include "Support/Module.h"

namespace aimc {
using std::vector;
class ModuleSlice : public Module {
 public:
  explicit ModuleSlice(Parameters *pParam);
  virtual ~ModuleSlice();

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

  bool temporal_profile_;
  bool take_all_;
  int lower_limit_;
  int upper_limit_;
  bool normalize_slice_;
  int slice_length_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_PROFILE_SCALER_H_
