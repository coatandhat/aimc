// Copyright 2006-2010, Thomas Walters
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
 *  \brief SAI module
 */

/*! \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2007/08/29
 *  \version \$Id: ModuleSAI.h 4 2010-02-03 18:44:58Z tcw $
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
