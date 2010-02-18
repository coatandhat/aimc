// Copyright 2006-2010, Thomas Walters, Willem van Engen
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
 *  \brief Common includes for all AIM-C
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \author: Willem van Engen <cnbh@willem.engen.nl>
 *  \date 2010/01/30
 *  \version \$Id: Common.h 1 2010-02-02 11:04:50Z tcw $
 */

#ifndef _AIMC_SUPPORT_COMMON_H_
#define _AIMC_SUPPORT_COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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

#endif  // _AIMC_SUPPORT_COMMON_H_
