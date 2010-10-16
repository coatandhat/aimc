/*!
 * \file
 * \brief ERB frequency scale for generating filter banks and their frequencies
 *
 * \author Tom Walters <tcw24@cam.ac.uk>
 * \date created 2006/09/26
 * \version \$Id: ScaleERB.h 459 2007-11-08 11:50:04Z tom $
 */
/* (c) 2006, University of Cambridge, Medical Research Council
 * http://www.pdn.cam.ac.uk/groups/cnbh/aimmanual
 */
#ifndef __MODULE_SCALE_ERB_H__
#define __MODULE_SCALE_ERB_H__

#include <math.h>

#include "Modules/Output/Graphics/Scale/Scale.h"

namespace aimc {

/*!
 * \class ScaleERB "Modules/Scale/ScaleERB.h"
 * \brief ERB frequency scale for generating filter banks and their frequencies
 *
 * It is very advisable to use Scale::Create() to an instance of this scale.
 *
 * References:
 *   - J. Smith and J. Abel (1999), "Bark and ERB bilinear transforms"
 *     http://www-ccrma.stanford.edu/~jos/bbt/
 */
class ScaleERB : public Scale {
public:
  ScaleERB(unsigned int min, unsigned int max, float density)
    : Scale(min, max, density) { m_iType = SCALE_ERB; m_sName = "erb"; };

  float FromLinear(float fFreq) {
    return 21.4f*log10(0.00437f*fFreq + 1.0f);
  };

  float ToLinear(float fFreq) {
    return (pow(10, fFreq/21.4f) - 1.0f)/0.00437f;
  };
};
}  // namespace aimc
#endif /* __MODULE_SCALE_ERB_H__ */
