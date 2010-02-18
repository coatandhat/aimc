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
 *  \brief ERB calculations
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \date 2010/01/23
 *  \version \$Id: ERBTools.h 1 2010-02-02 11:04:50Z tcw $
 */

#ifndef _AIMC_SUPPORT_ERBTOOLS_H_
#define _AIMC_SUPPORT_ERBTOOLS_H_

#include <math.h>

namespace aimc {
class ERBTools {
 public:
  static float Freq2ERB(float freq) {
    return 21.4f * log10(4.37f * freq / 1000.0f + 1.0f);
  }

  static float Freq2ERBw(float freq) {
    return 24.7f * (4.37f * freq / 1000.0f + 1.0f);
  }

  static float ERB2Freq(float erb) {
    return (pow(10, (erb / 21.4f)) - 1.0f) / 4.37f * 1000.0f;
  }
};
}

#endif  // _AIMC_SUPPORT_ERBTOOLS_H_
