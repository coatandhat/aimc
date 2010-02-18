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
 * \brief SAI 2003 module - for any output frame rate
 *
 * \author Tom Walters <tcw24@cam.ac.uk>
 * \date created 2007/03/28
 * \version \$Id$
 */

#ifndef _AIMC_MODULE_STROBES_PARABOLA_H_
#define _AIMC_MODULE_STROBES_PARABOLA_H_

#include <vector>

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

/*!
 * \class ModuleParabola "Modules/SAI/ModuleParabola.h"
 * \brief SF 2003
 */
namespace aimc {
using std::vector;
class ModuleParabola : public Module {
 public:
  ModuleParabola(Parameters *params);
  virtual ~ModuleParabola();
  void Process(const SignalBank& input);
private:
  /*! \brief Prepare the module
   *  \param input Input signal bank
   *  \param output true on success false on failure
   */
  virtual bool InitializeInternal(const SignalBank& input);

  virtual void ResetInternal();


  //! \brief Number of samples over which the strobe should be decayed to zero
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

  //! \brief Parabola height parameter
  float height_;

  //! \brief Parabola width paramters
  float parabw_;

  //! \brief Parabola a value
  vector<float> parab_a_;

  //! \brief Parabola b value
  vector<float> parab_b_;

  //! \brief Parabola calculation variable
  vector<float> parab_wnull_;

  //! \brief Parabola calculation variable
  vector<int> parab_var_samples_;

  //! \brief Storage for the number of samples since the last strobe
  vector<int> samples_since_last_strobe_;

  vector<float> prev_sample_;
  vector<float> curr_sample_;
  vector<float> next_sample_;

  float alpha_;
  int channel_count_;
  float sample_rate_;
  float strobe_decay_time_;
  int max_strobes_;
};
}  // namespace aimc

#endif  // _AIM_MODULE_STROBES_PARABOLA_H_
