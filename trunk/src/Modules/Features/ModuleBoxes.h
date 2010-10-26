// Copyright 2010, Google
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
 * \date created 2010-06-14
 * \version \$Id$
 */

#ifndef AIMC_MODULES_FEATURES_BOXES_H_
#define AIMC_MODULES_FEATURES_BOXES_H_

#include <utility>
#include <vector>
#include "Support/Module.h"

namespace aimc {
using std::pair;
class ModuleBoxes : public Module {
 public:
  explicit ModuleBoxes(Parameters *pParam);
  virtual ~ModuleBoxes();

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
  int box_size_spectral_;
  int box_size_temporal_;
  vector<int> box_limits_time_;
  vector<pair<int, int> > box_limits_channels_;
  int box_count_;
  int feature_size_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_FEATURES_BOXES_H_
