// Copyright 2009-2010, Thomas Walters
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
 *  \brief Slaney's gammatone filterbank
 *
 *  \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2009/11/13
 *  \version \$Id$
 *
 * This is the version of the IIR gammatone used in Slaney's Auditory toolbox.
 * The original verison as described in Apple Tech. Report #35 has a problem
 * with the high-order coefficients at low centre frequencies and high sample
 * rates. Since it is important that AIM-C can deal with these cases (for
 * example for the Gaussian features), I've reiplemeted Slaney's alternative
 * version which uses a cascade of four second-order filters in place of the 
 * eighth-order filter.
 */
#ifndef _AIMC_MODULES_BMM_GAMMATONE_H_
#define _AIMC_MODULES_BMM_GAMMATONE_H_

#include <vector>

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
using std::vector;
class ModuleGammatone : public Module {
 public:
  explicit ModuleGammatone(Parameters *params);
  virtual ~ModuleGammatone();
  /*! \brief Process a buffer
   */
  virtual void Process(const SignalBank &input);

 private:
  virtual bool InitializeInternal(const SignalBank& input);
  virtual void ResetInternal();

  // Filter coefficients
  vector<vector<double> > b1_;
  vector<vector<double> > b2_;
  vector<vector<double> > b3_;
  vector<vector<double> > b4_;
  vector<vector<double> > a_;

  vector<vector<double> > state_1_;
  vector<vector<double> > state_2_;
  vector<vector<double> > state_3_;
  vector<vector<double> > state_4_;

  vector<double> centre_frequencies_;
  int num_channels_;
  double max_frequency_;
  double min_frequency_;
};
}  // namespace aimc
#endif  // _AIMC_MODULES_BMM_GAMMATONE_H_
