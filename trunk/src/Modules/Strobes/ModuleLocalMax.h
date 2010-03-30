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
 * \date created 2010/02/23
 * \version \$Id$
 */

#ifndef AIMC_MODULES_STROBES_LOCAL_MAX_H_
#define AIMC_MODULES_STROBES_LOCAL_MAX_H_

#include <vector>
#include "Support/Module.h"

namespace aimc {
using std::vector;
class ModuleLocalMax : public Module {
 public:
  explicit ModuleLocalMax(Parameters *pParam);
  virtual ~ModuleLocalMax();

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

  float decay_time_ms_;
  float timeout_ms_;
  int strobe_timeout_samples_;
  int strobe_decay_samples_;

  vector<float> threshold_;
  vector<float> decay_constant_;

  vector<float> prev_sample_;
  vector<float> curr_sample_;
  vector<float> next_sample_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_STROBES_LOCAL_MAX_H_
