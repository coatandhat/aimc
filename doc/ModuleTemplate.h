// Copyright #YEAR#, #AUTHOR_NAME#
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
 * \author #AUTHOR_NAME# <#AUTHOR_EMAIL_ADDRESS#>
 * \date created #TODAYS_DATE#
 * \version \$Id$
 */

#ifndef AIMC_MODULES_#MODULE_TYPE_CAPS#_#MODULE_ID_CAPS#_H_
#define AIMC_MODULES_#MODULE_TYPE_CAPS#_#MODULE_ID_CAPS#_H_

#include "Support/Module.h"

namespace aimc {
using std::vector;
class #MODULE_NAME# : public Module {
 public:
  explicit #MODULE_NAME#(Parameters *pParam);
  virtual ~#MODULE_NAME#();

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
};
}  // namespace aimc

#endif  // AIMC_MODULES_#MODULE_TYPE_CAPS#_#MODULE_ID_CAPS#_H_
