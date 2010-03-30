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
 * \brief Halfwave rectification, compression and lowpass filtering
 */

/* \author Thomas Walters <tom@acousticscale.org>
 * \date created 2007/03/07
 * \version \$Id$
 */

#ifndef AIMC_MODULES_NAP_HCL_H_
#define AIMC_MODULES_NAP_HCL_H_

#include <vector>

#include "Support/Module.h"
#include "Support/SignalBank.h"

namespace aimc {
using std::vector;
class ModuleHCL : public Module {
 public:
  explicit ModuleHCL(Parameters *parameters);
  virtual ~ModuleHCL();

  virtual void Process(const SignalBank &input);

 private:
  /*! \brief Prepare the module
   *  \param input Input signal bank
   *  \param output true on success false on failure
   */
  virtual bool InitializeInternal(const SignalBank &input);

  virtual void ResetInternal();

  /*! \brief Do lowpass filtering?
   */
  bool do_lowpass_;

  /*! \brief Do log compression?
   */
  bool do_log_;

  /*! \brief Cutoff frequency for lowpass filter
   */
  float lowpass_cutoff_;

  /*! \brief Order of Lowpass Filter
   */
  int lowpass_order_;

  /*! \brief Internal record of the number of channels in the input
   */
  int channel_count_;

  /*! \brief Time constant corresponsing to the lowpass filter 
   *  cutoff freqency
   */
  float time_constant_;

  /*! \brief Lowpass filter state variables
   */
  float xn_;
  float yn_;
  vector<vector<float> > yns_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_NAP_HCL_H_
