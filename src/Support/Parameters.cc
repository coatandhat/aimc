// Copyright 2006-2010, Willem van Engen
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

/*!
 * \file
 * \brief Main parameters store
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/21
 * \version \$Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Support/Common.h"
#include "Support/Parameters.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace aimc {
const char *Parameters::m_SDefaultIniSection = "";

Parameters::Parameters() {
  m_iNestCount = 0;
  m_pIni = new CSimpleIniCase(false, false, true);
  AIM_ASSERT(m_pIni);
}

Parameters::~Parameters() {
  DELETE_IF_NONNULL(m_pIni);
}

const char * Parameters::DefaultString(const char* sName, const char* val) {
  AIM_ASSERT(m_pIni);
  if (!IsSet(sName)) {
    SetString(sName, val);
  }
  return GetString(sName);
}

int Parameters::DefaultInt(const char* sName, int val) {
  AIM_ASSERT(m_pIni);
  if (!IsSet(sName)) {
    SetInt(sName, val);
  }
  return GetInt(sName);
}

unsigned int Parameters::DefaultUInt(const char* sName, unsigned int val) {
  AIM_ASSERT(m_pIni);
  if (!IsSet(sName)) {
    SetUInt(sName, val);
  }
  return GetUInt(sName);
}

float Parameters::DefaultFloat(const char* sName, float val) {
  AIM_ASSERT(m_pIni);
  if (!IsSet(sName)) {
    SetFloat(sName, val);
  }
  return GetFloat(sName);
}

bool Parameters::DefaultBool(const char* sName, bool val) {
  AIM_ASSERT(m_pIni);
  if (!IsSet(sName)) {
    SetBool(sName, val);
  }
  return GetBool(sName);
}

void Parameters::SetString(const char *sName, const char *val) {
  AIM_ASSERT(m_pIni);
  m_pIni->SetValue(m_SDefaultIniSection, sName, val);
}

void Parameters::SetInt(const char *sName, int val) {
  char sVal[20];
  snprintf(sVal, sizeof(sVal)/sizeof(sVal[0]), "%d", val);
  SetString(sName, sVal);
}

void Parameters::SetUInt(const char *sName, unsigned int val) {
  char sVal[20];
  snprintf(sVal, sizeof(sVal)/sizeof(sVal[0]), "%ud", val);
  SetString(sName, sVal);
}

void Parameters::SetBool(const char *sName, bool val) {
  SetString(sName, val ? "true" : "false");
}

void Parameters::SetFloat(const char *sName, float val) {
  char sVal[20];
  snprintf(sVal, sizeof(sVal)/sizeof(sVal[0]), "%f", val);
  SetString(sName, sVal);
}

const char *Parameters::GetString(const char *sName) {
  AIM_ASSERT(m_pIni);
  const char *sVal = m_pIni->GetValue(m_SDefaultIniSection, sName, NULL);
  if (!sVal) {
    LOG_ERROR(_T("Parameter not found '%s'"), sName);
    return "";
  }
  return sVal;
}

int Parameters::GetInt(const char *sName) {
  return atoi(GetString(sName));
}

unsigned int Parameters::GetUInt(const char *sName) {
  return atoi(GetString(sName));
}

float Parameters::GetFloat(const char *sName) {
  return atof(GetString(sName));
}

bool Parameters::GetBool(const char *sName) {
  const char *sVal = GetString(sName);
  if (strcmp(sVal, "true") == 0 || strcmp(sVal, "on") == 0 ||
      strcmp(sVal, "yes") == 0 || strcmp(sVal, "1") == 0 ||
      strcmp(sVal, "y") == 0 || strcmp(sVal, "t") == 0)
    return true;
  else
    return false;
}

bool Parameters::IsSet(const char *sName) {
  AIM_ASSERT(m_pIni);
  return m_pIni->GetValue(m_SDefaultIniSection, sName, NULL) != NULL;
}

bool Parameters::Parse(const char *sCmd) {
  /*! \todo There is some code duplication here from Parameters::Merge()
   */

  CSimpleIniCase *pIni2 = new CSimpleIniCase(false, false, true);
  AIM_ASSERT(pIni2);
  if (pIni2->Load(sCmd, strlen(sCmd)) < 0) {
    LOG_ERROR(_T("Could not parse option '%s'"), sCmd);
    delete pIni2;
    return false;
  }

  // if there are keys and values...
  const CSimpleIniCase::TKeyVal *pSectionData =
                                   pIni2->GetSection(m_SDefaultIniSection);
  if (pSectionData) {
    // iterate over all keys and set them in the current parameter file
    CSimpleIniCase::TKeyVal::const_iterator iKeyVal = pSectionData->begin();
    for ( ;iKeyVal != pSectionData->end(); ++iKeyVal) {
      m_pIni->SetValue(m_SDefaultIniSection,
                       iKeyVal->first.pItem,
                       iKeyVal->second);
      }
    }
  delete pIni2;
  return true;
}

bool Parameters::Delete(const char *sName) {
  AIM_ASSERT(m_pIni);
  return(m_pIni->Delete(m_SDefaultIniSection, sName));
}

bool Parameters::LoadFile(const char *sParamFilename) {
  AIM_ASSERT(m_pIni);
  SI_Error siErr;
  bool bRet = true;

  // Avoid inclusion loops
  if (m_iNestCount >= m_iNestCountMaximum) {
    LOG_ERROR(_T("Possible inclusion loop in file '%s' (%d times)"),
             sParamFilename, m_iNestCount);
    return false;
  }
  m_iNestCount++;

  if ((siErr=m_pIni->LoadFile(sParamFilename)) < 0) {
    // Don't complain if file not found, but do return error
    if (siErr != SI_FILE)
      LOG_ERROR(_T("Couldn't parse parameters from '%s'"), sParamFilename);
    m_iNestCount--;
    return false;
  }

  m_iNestCount--;
  return bRet;
}

bool Parameters::MergeFile(const char *sParamFilename) {
  Parameters *pParam2 = new Parameters();
  AIM_ASSERT(pParam2);
  if (!pParam2->LoadFile(sParamFilename)) {
    LOG_ERROR(_T("Could not load parameter file '%s' for merging"),
              sParamFilename);
    delete pParam2;
    return false;
  }

  // if there are keys and values...
  const CSimpleIniCase::TKeyVal *pSectionData =
          pParam2->m_pIni->GetSection(m_SDefaultIniSection);
  if (pSectionData) {
    // iterate over all keys and set them in the current parameter file
    CSimpleIniCase::TKeyVal::const_iterator iKeyVal = pSectionData->begin();
    for ( ;iKeyVal != pSectionData->end(); ++iKeyVal) {
      m_pIni->SetValue(m_SDefaultIniSection,
                       iKeyVal->first.pItem,
                       iKeyVal->second);
      }
  }

  delete pParam2;
  return true;
}

bool Parameters::Merge(const char *sParamFilename) {
  return MergeFile(sParamFilename);
}

bool Parameters::Load(const char *sParamFilename) {
  bool bRet = true;
  // Load parameter file
  bRet &= Merge(sParamFilename);
  return bRet;
}

bool Parameters::Save(const char *sParamFilename) {
  AIM_ASSERT(m_pIni);
  SI_Error siErr;
  FILE *pSaveFile;

    pSaveFile = fopen(sParamFilename, "wb");
    if (pSaveFile == NULL) {
      LOG_ERROR(_T("Couldn't create parameters file '%s' to save to"),
                sParamFilename);
      return false;
    }

  if ((siErr = m_pIni->SaveFile(pSaveFile)) < 0) {
    LOG_ERROR(_T("Couldn't save parameters in file '%s'"), sParamFilename);
    return false;
  }

  fclose(pSaveFile);

  return true;
}

std::string Parameters::WriteString() {
  AIM_ASSERT(m_pIni);
  SI_Error siErr;
  std::string output_string;

  if ((siErr = m_pIni->Save(output_string)) < 0) {
    LOG_ERROR(_T("Couldn't write parameters to string"));
    return false;
  }
  return output_string;
}

}  // namespace aimc
