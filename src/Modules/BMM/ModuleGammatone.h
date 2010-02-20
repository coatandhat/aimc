// Copyright 2009-2010, Thomas Walters
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
