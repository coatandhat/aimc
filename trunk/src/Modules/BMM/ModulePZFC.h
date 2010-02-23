// Copyright 2008-2010, Thomas Walters
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
 *  \brief Dick Lyon's Pole-Zero Filter Cascade - implemented in C++ by Tom
 *  Walters from the AIM-MAT module based on Dick Lyon's code.
 *
 *  \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2008/02/05
 * \version \$Id$
 */

#ifndef _AIMC_MODULES_BMM_PZFC_H_
#define _AIMC_MODULES_BMM_PZFC_H_

#include <vector>

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
using std::vector;
class ModulePZFC : public Module {
 public:
  explicit ModulePZFC(Parameters *pParam);
  virtual ~ModulePZFC();

  /*! \brief Process a buffer
   */
  virtual void Process(const SignalBank &input);

 private:
  /*! \brief Reset all internal state variables to their initial values
   */
  virtual void ResetInternal();

  /*! \brief Prepare the module
   *  \param input Input SignalBank
   *  \param output true on success false on failure
   */
  virtual bool InitializeInternal(const SignalBank &input);

  /*! \brief Set the filterbank parameters according to a fit matrix from Unoki
   *  and Lyon's fitting routine
   */
  bool SetPZBankCoeffsERBFitted();

  /*! \brief Sets the general filterbank coefficients
   */
  bool SetPZBankCoeffs();

  /*! \brief Automatic Gain Control
   */
  void AGCDampStep();

  /*! \brief Detector function - halfwave rectification etc. Used internally,
   *  but not applied to the output.
   */
  float DetectFun(float fIN);

  /*! \brief Minimum
   */
  inline float Minimum(float a, float b);

  int channel_count_;
  int buffer_length_;
  int agc_stage_count_;
  float sample_rate_;
  float last_input_;

  // Parameters
  // User-settable scalars
  float pole_damping_;
  float zero_damping_;
  float zero_factor_;
  float step_factor_;
  float bandwidth_over_cf_;
  float min_bandwidth_hz_;
  float agc_factor_;
  float cf_max_;
  float cf_min_;
  float mindamp_;
  float maxdamp_;
  bool do_agc_step_;

  // Internal Buffers
  // Initialised once
  vector<float> pole_dampings_;
  vector<float> agc_epsilons_;
  vector<float> agc_gains_;
  vector<float> pole_frequencies_;
  vector<float> za0_;
  vector<float> za1_;
  vector<float> za2_;
  vector<float> rmin_;
  vector<float> rmax_;
  vector<float> xmin_;
  vector<float> xmax_;

  // Modified by algorithm at each time step
  vector<float> detect_;
  vector<vector<float> > agc_state_;
  vector<float> state_1_;
  vector<float> state_2_;
  vector<float> previous_out_;
  vector<float> pole_damps_mod_;
  vector<float> inputs_;
};
}

#endif  // _AIMC_MODULES_BMM_PZFC_H_
