// Copyright 2007-2010, Thomas Walters
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

/*!
 * \file
 * \brief Halfwave rectification, compression and lowpass filtering
 */

/* \author Thomas Walters <tom@acousticscale.org>
 * \date created 2007/03/07
 * \version \$Id: ModuleHCL.h 4 2010-02-03 18:44:58Z tcw $
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
