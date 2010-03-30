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

void LOG_INFO_NN(const char *sFmt, ...) {
  va_list args;
  va_start(args, sFmt);
  // Just print message to console (will be lost on windows with gui)
  vprintf(sFmt, args);
  va_end(args);
}
}  // namespace aimc
