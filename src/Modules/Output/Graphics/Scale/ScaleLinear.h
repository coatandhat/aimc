/*!
 * \file
 * \brief Linear frequency scale for generating filter banks and their frequencies
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/26
 * \version \$Id: ScaleLinear.h 459 2007-11-08 11:50:04Z tom $
 */
/* (c) 2006, University of Cambridge, Medical Research Council
 * http://www.pdn.cam.ac.uk/groups/cnbh/aimmanual
 */
#ifndef __MODULE_SCALE_LINEAR_H__
#define __MODULE_SCALE_LINEAR_H__

#include "Modules/Output/Graphics/Scale/Scale.h"

namespace aimc {

/*!
 * \class ScaleLinear "Modules/Scale/ScaleLinear.h"
 * \brief Linear frequency scale for generating filter banks and their frequencies
 *
 * It is very advisable to use Scale::Create() to an instance of this scale.
 *
 * In terms of scaling, this is of course an identity transformation.
 */
class ScaleLinear : public Scale {
public:
  ScaleLinear(unsigned int min, unsigned int max, float density)
    : Scale(min, max, density) { m_iType = SCALE_LINEAR; m_sName = "linear"; };

  float FromLinear(float fFreq) {
    return fFreq;
  };

  float ToLinear(float fFreq) {
    return fFreq;
  };
};
}  // namepspace aimc
#endif /* __MODULE_SCALE_LINEAR_H__ */
