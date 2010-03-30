// Copyright 2006-2010, Thomas Walters
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
 *  \brief SAI module
 */

/*! \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2007/08/29
 *  \version \$Id$
 */
#ifndef AIMC_MODULES_SAI_SAI_H_
#define AIMC_MODULES_SAI_SAI_H_

#include <vector>

#include "Support/Module.h"
#include "Support/SignalBank.h"
#include "Support/StrobeList.h"

namespace aimc {
using std::vector;
class ModuleSAI : public Module {
 public:
  explicit ModuleSAI(Parameters *parameters);
  virtual ~ModuleSAI();
  void Process(const SignalBank &input);

 private:
  /*! \brief Prepare the module
   *  \param input Input signal bank
   *  \param output true on success false on failure
   */
  bool InitializeInternal(const SignalBank &input);

  virtual void ResetInternal();

  /*! \brief Temporary buffer for constructing the current SAI frame
   */
  SignalBank sai_temp_;

  /*! \brief List of strobes for each channel
   */
  vector<StrobeList> active_strobes_;

  /*! \brief Buffer decay parameter
   */
  float buffer_memory_decay_;

  /*! \brief Sample index of minimum strobe delay
   */
  int min_strobe_delay_idx_;

  /*! \brief Sample index of maximum strobe delay
   */
  int max_strobe_delay_idx_;

  /*! \brief Factor with which the SAI should be decayed
   */
  float sai_decay_factor_;

  /*! \brief Precomputed 1/n^alpha values for strobe weighting
   */
  vector<float> strobe_weights_;

  /*! \brief Next Strobe for each channels
   */
  vector<int> next_strobes_;

  float strobe_weight_alpha_;

  /*! \brief The maximum number strobes that can be active at the same time.
   *
   */
  int max_concurrent_strobes_;

  int fire_counter_;

  /*! \brief Period in milliseconds between output frames
   */
  float frame_period_ms_;
  int frame_period_samples_;

  float min_delay_ms_;
  float max_delay_ms_;
  int channel_count_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_SAI_SAI_H_
