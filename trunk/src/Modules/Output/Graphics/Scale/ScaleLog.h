/*!
 * \file
 * \brief Logarithmic frequency scale for generating filter banks and their frequencies
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/28
 * \version \$Id: ScaleLog.h 459 2007-11-08 11:50:04Z tom $
 */
/* (c) 2006, University of Cambridge, Medical Research Council
 * http://www.pdn.cam.ac.uk/groups/cnbh/aimmanual
 */
#ifndef __MODULE_SCALE_LOG_H__
#define __MODULE_SCALE_LOG_H__

#include <math.h>

#include "Modules/Output/Graphics/Scale/Scale.h"

namespace aimc {

/*!
 * \class ScaleLog "Modules/Scale/ScaleLog.h"
 * \brief Logarithmic frequency scale for generating filter banks and their frequencies
 *
 * It is very advisable to use Scale::Create() to an instance of this scale.
 */
class ScaleLog : public Scale {
public:
  ScaleLog(unsigned int min, unsigned int max, float density)
    : Scale(min, max, density) { m_iType = SCALE_LOG; m_sName="log"; };

  /*! The log scale has a problem, because log(0)=inf, so all values below
   *  1e-5 are truncated to 1e-5. */
  float FromLinear(float fFreq) {
    if (fFreq<1e-5f) fFreq=1e-5f;
    return log(fFreq);
  };

  float ToLinear(float fFreq) {
    return exp(fFreq);
  };
};
}  // namespace aimc
#endif /* __MODULE_SCALE_LOG_H__ */
