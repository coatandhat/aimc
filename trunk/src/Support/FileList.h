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