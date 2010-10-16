/*!
 * \file
 * \brief Frequency scale for generating filter banks and their frequencies
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/28
 * \version \$Id: Scale.cpp 459 2007-11-08 11:50:04Z tom $
 */
/* (c) 2006, University of Cambridge, Medical Research Council
 * http://www.pdn.cam.ac.uk/groups/cnbh/aimmanual
 */
#include "Support/Common.h"
#include "Modules/Output/Graphics/Scale/Scale.h"
#include "Modules/Output/Graphics/Scale/ScaleLinear.h"
#include "Modules/Output/Graphics/Scale/ScaleERB.h"
#include "Modules/Output/Graphics/Scale/ScaleLog.h"
#include "Modules/Output/Graphics/Scale/ScaleLogScaled.h"

namespace aimc {

Scale *Scale::Create(ScaleType iType,
		     unsigned int min,
		     unsigned int max,
		     float density) {
  switch(iType) {
  case SCALE_LINEAR:
    return static_cast<Scale*>(new ScaleLinear(min, max, density));
  case SCALE_ERB:
    return static_cast<Scale*>(new ScaleERB(min, max, density));
  case SCALE_LOG:
    return static_cast<Scale*>(new ScaleLog(min, max, density));
  case SCALE_LOGSCALED:
    return static_cast<Scale*>(new ScaleLogScaled(min, max, density));
  default:
    AIM_ASSERT(0);
    break;
  }
  // Unreachable code
  AIM_ASSERT(0);
  return NULL;
}

Scale *Scale::Create(ScaleType iType) {
  return Create(iType, 0, 0, 0);
}

Scale *Scale::Clone() {
  Scale *pScale = Create(m_iType, m_iMin, m_iMax, m_fDensity);
  AIM_ASSERT(pScale);
  pScale->m_fScaledCurHalfSum = m_fScaledCurHalfSum;
  pScale->m_fScaledCurDiff = m_fScaledCurDiff;
  return pScale;
}

float Scale::FromLinearScaled(float fVal) {
  /*! This function returns
   *    ( FromLinear(fVal) - (fMinScaled+fMaxScaled)/2 ) / (fMaxScaled-fMinScaled)
   */
  float fValScaled = FromLinear(fVal);
  return (fValScaled - m_fScaledCurHalfSum) / m_fScaledCurDiff;
}

void Scale::FromLinearScaledExtrema(float fMin, float fMax) {
  float fMinScaled = FromLinear(fMin);
  float fMaxScaled = FromLinear(fMax);
  m_fScaledCurHalfSum = (fMinScaled+fMaxScaled)/2;
  m_fScaledCurDiff = fMaxScaled-fMinScaled;
  m_fMin = fMin;
  m_fMax = fMax;
}

void Scale::FromLinearScaledExtrema(Scale *pScale) {
  AIM_ASSERT(pScale);
  FromLinearScaledExtrema(pScale->m_fMin, pScale->m_fMax);
}
}  // namespace aimc
