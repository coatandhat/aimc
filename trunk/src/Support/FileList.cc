// Copyright 2010, Thomas Walters
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