// Copyright 2007-2010, Thomas Walters
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
 * \file
 * \brief SAI 2003 module - for any output frame rate
 *
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2007/03/28
 * \version \$Id$
 */

#ifndef AIMC_MODULES_STROBES_PARABOLA_H_
#define AIMC_MODULES_STROBES_PARABOLA_H_

#include <vector>

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
using std::vector;
class ModuleParabola : public Module {
 public:
  explicit ModuleParabola(Parameters *params);
  virtual ~ModuleParabola();
  void Process(const SignalBank& input);
 private:
  /*! \brief Prepare the module
   *  \param input Input signal bank
   *  \param output true on success false on failure
   */
  virtual bool InitializeInternal(const SignalBank& input);

  virtual void ResetInternal();


  /*! \brief Number of samples over which the strobe should be decayed to
   *  zero
   */
  int strobe_decay_samples_;

  /*! \brief Current strobe thresholds, one for each bank channel.
   *
   *  This value is decayed over time.
   */
  vector<float> threshold_;

  /*! \brief Signal value at the last strobe, one for each bank channel.
   *
   *  This value is not decayed over time.
   */
  vector<float> last_threshold_;

  /*! \brief Parabola height parameter
   */
  float height_;

  /*! \brief Parabola width paramters
   */
  float parabw_;

  /*! \brief Parabola a value
   */
  vector<float> parab_a_;

  /*! \brief Parabola b value
   */
  vector<float> parab_b_;

  /*! \brief Parabola calculation variable
   */
  vector<float> parab_wnull_;

  /*! \brief Parabola calculation variable
   */
  vector<int> parab_var_samples_;

  /*! \brief Storage for the number of samples since the last strobe
   */
  vector<int> samples_since_last_strobe_;

  vector<float> prev_sample_;
  vector<float> curr_sample_;
  vector<float> next_sample_;

  float alpha_;
  int channel_count_;
  float sample_rate_;
  float strobe_decay_time_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_STROBES_PARABOLA_H_
