// Copyright 2006-2010, Thomas Walters, Willem van Engen
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

/*! \file
 *  \brief Common includes for all AIM-C
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \author: Willem van Engen <cnbh@willem.engen.nl>
 *  \date 2010/01/30
 *  \version \$Id$
 */

#ifndef AIMC_SUPPORT_COMMON_H_
#define AIMC_SUPPORT_COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

// Defines for windows
#ifdef _WINDOWS
#define M_PI 3.14159265359
#define isnan _isnan
#define isinf(x) (!_finite(x))
#define snprintf _snprintf
#define PATH_MAX _MAX_PATH
#endif

#define AIM_NAME "AIM-C"
#define AIM_VERSION_STRING "version_number"

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#if !defined(_T)
#  ifdef _UNICODE
#    define _T(x) L ## x
#  else
#    define _T(x) x
#  endif
#endif

#if !defined(_S)
#  ifdef _UNICODE
#    define _S(x) L ## x
#  else
#    define _S(x) x
#  endif
#endif

/*! \brief C++ delete if != NULL
 *
 *  This was used so often, that is was moved to a macro.
 */
#define DELETE_IF_NONNULL(x) { \
  if ( (x) ) { \
    delete (x); \
    (x) = NULL; \
  } \
}

/*! \brief C++ delete[] if != NULL
 *
 *  This was used so often, that is was moved to a macro.
 */
#define DELETE_ARRAY_IF_NONNULL(x) { \
  if ( (x) ) { \
    delete[] (x); \
    (x) = NULL; \
  } \
}

/*! \brief C free if != NULL
 *
 *  This was used so often, that is was moved to a macro.
 */
#define FREE_IF_NONNULL(x) { \
  if ( (x) ) { \
    free(x); \
    (x) = NULL; \
  } \
}

#ifdef DEBUG
#  define AIM_VERIFY(x) AIM_ASSERT(x)
#  define AIM_ASSERT(x) { \
     if (!(x)) { \
       LOG_ERROR("Assertion failed.\n"); \
       *(reinterpret_cast<char*>(0)) = 0; \
     } \
  }
#else
#  define AIM_VERIFY(x) {x;}
#  define AIM_ASSERT(...)
#endif

namespace aimc {
void LOG_ERROR(const char *sFmt, ...);
void LOG_INFO(const char *sFmt, ...);
void LOG_INFO_NN(const char *sFmt, ...);
}  // namespace aimc

#endif  // AIMC_SUPPORT_COMMON_H_
