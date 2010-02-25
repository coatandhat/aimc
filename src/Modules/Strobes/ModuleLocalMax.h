// Copyright 2010, Thomas Walters
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
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2010/02/23
 * \version \$Id$
 */

#ifndef AIMC_MODULES_STROBES_LOCAL_MAX_H_
#define AIMC_MODULES_STROBES_LOCAL_MAX_H_

#include <vector>
#include "Support/Module.h"

namespace aimc {
using std::vector;
class ModuleLocalMax : public Module {
 public:
  explicit ModuleLocalMax(Parameters *pParam);
  virtual ~ModuleLocalMax();

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

  float sample_rate_;
  int buffer_length_;
  int channel_count_;

  float decay_time_ms_;
  float timeout_ms_;
  int strobe_timeout_samples_;
  int strobe_decay_samples_;

  vector<float> threshold_;
  vector<float> decay_constant_;

  vector<float> prev_sample_;
  vector<float> curr_sample_;
  vector<float> next_sample_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_STROBES_LOCAL_MAX_H_
