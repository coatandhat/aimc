// Copyright 2010, Thomas Walters
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
 * \brief Convert a file containing a list of pairs of tab-separated
 * items, one per line, and convert it to a vector<pair<string, string> >
 *
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2010/02/23
 * \version \$Id$
 */

#if defined(_WINDOWS)
#  include <windows.h>
#  ifndef PATH_MAX
#    define PATH_MAX _MAX_PATH
#  endif
#endif
#include <limits.h>
#include <string>
#include <utility>
#include <vector>

#include "Support/Common.h"

namespace aimc {
using std::vector;
using std::pair;
using std::string;
class FileList {
 public:
  static vector<pair<string, string> > Load(string filename);
};
}  // namespace aimc