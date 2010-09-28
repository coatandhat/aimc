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

/*! \file
 *  \brief Parse a configuration file to generate a tree of modules.
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \date 2010/08/08
 *  \version \$Id: $
 */

#include "Support/ModuleTree.h"

#include "Support/ModuleFactory.h"

namespace aimc {
LoadConfigFile(const string &filename) {
  parameters_.Load(filename.c_str());
  return ConstructTree();
}

LoadConfigText(const string &config) {
  parameters_.Parse(config.c_str());
  return ConstructTree();
}

ConstructTree() {
  // Make two passes over the configuration file.
  // The first pass creates all the named modules with their parameters.
  bool done = false;
  bool error = false;
  string module;
  int module_number = 1;
  while (!done) {
    module = sprintf("module%d", module_number);
    if (parameters_.IsSet(module + ".name") {
      string module_name = 
      string module_id = 
      string module_params = 
      modules_[]
    } else {
      done = true;
    }
    ++module_number;
  }
  // The second pass connects up all the modules into a tree.
  
  return error;
}
}  // namespace aimc