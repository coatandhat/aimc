// Copyright 2006, Willem van Engen
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Support/Common.h"
#include <string.h>
#include <stdio.h>

#include "Modules/Output/Graphics/GraphAxisSpec.h"

namespace aimc {

GraphAxisSpec::GraphAxisSpec(float fMin, float fMax, Scale::ScaleType iScale) {
  m_pScale = NULL;
  m_sLabel = NULL;
  SetDisplayRange(fMin, fMax);
  SetDisplayScale(iScale);
}

GraphAxisSpec::GraphAxisSpec() {
  m_pScale = NULL;
  m_sLabel = NULL;
  m_fMin = 0;
  m_fMax = 0;
}

GraphAxisSpec::~GraphAxisSpec() {
  DELETE_IF_NONNULL(m_pScale);
}

void GraphAxisSpec::SetDisplayRange(float fMin, float fMax) {
  AIM_ASSERT(fMin <= fMax);
  m_fMin = fMin;
  m_fMax = fMax;
  if (m_pScale)
    m_pScale->FromLinearScaledExtrema(m_fMin, m_fMax);
}

void GraphAxisSpec::SetDisplayScale(Scale::ScaleType iScale) {
  DELETE_IF_NONNULL(m_pScale);
  m_pScale = Scale::Create(iScale);
  AIM_ASSERT(m_pScale);
  m_pScale->FromLinearScaledExtrema(m_fMin, m_fMax);
}

bool GraphAxisSpec::Initialize(Parameters *parameters,
                               const char *sPrefix,
                               float fMin,
                               float fMax,
                               Scale::ScaleType iScale) {
  AIM_ASSERT(parameters);
  AIM_ASSERT(sPrefix && sPrefix[0]!='\0');
  char sParamName[Parameters::MaxParamNameLength];

  //! The following parameters are recognized for each axis:
  //!   - \c "<prefix>.min", the minimum value; a float or \c 'auto'
  snprintf(sParamName, sizeof(sParamName)/sizeof(sParamName[0]),
          "%s.min",
          sPrefix);
  if (strcmp(parameters->DefaultString(sParamName, "auto"), "auto") == 0)
    m_fMin = fMin;
  else
    m_fMin = parameters->GetFloat(sParamName);
  

  //!   - \c "<prefix>.max", the maximum value; a float or \c 'auto'
  snprintf(sParamName, sizeof(sParamName)/sizeof(sParamName[0]),
          "%s.max",
          sPrefix);
  if (strcmp(parameters->DefaultString(sParamName, "auto"), "auto")==0)
    m_fMax = fMax;
  else
    m_fMax = parameters->GetFloat(sParamName);

  // Make sure ranges are updated properly
  SetDisplayRange(m_fMin, m_fMax);

  //!   - \c "<prefix>.scale", the scale; one of \c 'auto',
  //!     \c 'linear', \c 'erb' or \c 'log'
  snprintf(sParamName, sizeof(sParamName)/sizeof(sParamName[0]),
           "%s.scale",
           sPrefix);
  // Scale change, we updated min/max values already so no need to
  Scale::ScaleType iThisScale;
  const char *sVal = parameters->DefaultString(sParamName, "auto");
  if (strcmp(sVal, "auto")==0)
    iThisScale = iScale;
  else if (strcmp(sVal, "linear")==0)
    iThisScale = Scale::SCALE_LINEAR;
  else if (strcmp(sVal, "erb")==0)
    iThisScale = Scale::SCALE_ERB;
  else if (strcmp(sVal, "log")==0)
    iThisScale = Scale::SCALE_LOG;
  else {
    LOG_ERROR(_T("Unrecognized scale type in parameter '%s': '%s'"),
                sParamName,
                sVal);
    return false;
  }
  SetDisplayScale(iThisScale);


  //!   - \c "<prefix>.label", the label; a string
  snprintf(sParamName, sizeof(sParamName)/sizeof(sParamName[0]),
           "%s.label",
           sPrefix);
  m_sLabel = parameters->DefaultString(sParamName, ""); // Assumes strings remains valid

  return true;
}
}  // namespace aimc
