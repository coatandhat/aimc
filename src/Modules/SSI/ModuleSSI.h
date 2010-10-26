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

#ifndef AIMC_MODULES_SSI_SSI_H_
#define AIMC_MODULES_SSI_SSI_H_

#include <vector>
#include "Support/Module.h"

namespace aimc {
using std::vector;
class ModuleSSI : public Module {
 public:
  explicit ModuleSSI(Parameters *pParam);
  virtual ~ModuleSSI();

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

  int ExtractPitchIndex(const SignalBank &input) const;

  float sample_rate_;
  int buffer_length_;
  int channel_count_;
  int ssi_width_samples_;
  float ssi_width_cycles_;
  float pivot_cf_;
  vector<float> h_;

  bool do_pitch_cutoff_;
  bool weight_by_cutoff_;
  bool weight_by_scaling_;
  bool log_cycles_axis_;
  float pitch_search_start_ms_;
  bool do_smooth_offset_;
  float smooth_offset_cycles_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_SSI_SSI_H_
