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

#include <limits.h>
#include "Support/FileList.h"

namespace aimc {
vector<pair<string, string> > FileList::Load(string filename) {
  FILE* file_handle;
  vector<pair<string, string> > file_list;
  if ((file_handle = fopen(filename.c_str(), "r"))==NULL) {
    LOG_ERROR(_T("Couldn't open file '%s' for reading."), filename.c_str());
    return file_list;
  }

  string out_1;
  string out_2;
  char n1[PATH_MAX];
  char n2[PATH_MAX];
  while (fscanf(file_handle, "%s\t%s", n1, n2) != EOF) {
    out_1 = n1;
    out_2 = n2;
    file_list.push_back(make_pair(out_1, out_2));
  }
  fclose(file_handle);
  return file_list;
}
}  // namespace aimc