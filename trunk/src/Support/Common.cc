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

#include "Support/Common.h"

namespace aimc {
void LOG_ERROR(const char *sFmt, ...) {
  va_list args;
  va_start(args, sFmt);
  vfprintf(stderr, sFmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

void LOG_INFO(const char *sFmt, ...) {
  va_list args;
  va_start(args, sFmt);
  // Just print message to console (will be lost on windows with gui)
  vprintf(sFmt, args);
  printf("\n");
  va_end(args);
}
}  // namespace aimc
