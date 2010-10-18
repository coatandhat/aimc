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

#include <iostream>
#include <map>
#include <string>

#include "Support/Common.h"
#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"
#include "Support/linked_ptr.h"

namespace aimc {
using std::string;
using std::map;
using std::ostream;

class ModuleTree {
 public:
  ModuleTree();
  bool LoadConfigFile(const string &filename);
  bool LoadConfigText(const string &config_text);
  string GetFullConfig();
  bool Initialize(Parameters *global_parameters);
  void Reset();
  void PrintConfiguration(ostream &out);
  void Process();
  void MakeDotGraph(ostream &out);
  void set_output_filename_prefix(const string &prefix) {
    output_filename_prefix_ = prefix;
  };
  string output_filename_prefix() {
    return output_filename_prefix_;
  };
 private:
  bool ConstructTree();
  Parameters config_;
  SignalBank s_;
  string output_filename_prefix_;
  map<string, linked_ptr<Module> > modules_;
  Module *root_module_;
  map<string, linked_ptr<Parameters> > parameters_;
  bool initialized_;
  DISALLOW_COPY_AND_ASSIGN(ModuleTree);
};
}  // namespace aimc