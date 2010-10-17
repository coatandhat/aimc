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
#include "Support/Module.h"
#include "Support/Parameters.h"

namespace aimc {
bool ModuleTree::LoadConfigFile(const string &filename) {
  config_.Load(filename.c_str());
  return ConstructTree();
}

bool ModuleTree::LoadConfigText(const string &config) {
  config_.Parse(config.c_str());
  return ConstructTree();
}

bool ModuleTree::ConstructTree() {
  // Make two passes over the configuration file.
  // The first pass creates all the named modules with their parameters.
  bool done = false;
  bool error = false;
  char module_name_var[AimParameters::MaxParamNameLength];
  char module_id_var[AimParameters::MaxParamNameLength];
  char module_parameters_var[AimParameters::MaxParamNameLength];
  int module_number = 1;
  while (!done) {
    sprintf(module_name_var, "module%d.name", module_number);
    sprintf(module_id_var, "module%d.id", module_number);
    sprintf(module_parameters_var, "module%d.parameters", module_number);
    if (config_.IsSet(module_name_var) {
      char* name = config_.GetString(module_name_var));
      if (module_number == 1) {
        root_name_ = name;
      }
      if (config_.IsSet(module_id_var)) {
        char* id = config_.GetString(module_id_var));
        char* parameters = config_.DefaultString(module_parameters_var), "");
        parameters_[name] = new Parameters();
        parameters_[name]->Parse(parameters);
        modules_[name] = factory_.Create(id, parameters_[name].get());
      } else {
        LOG_ERROR("id field missing for module named %s", name);
        error = true;
        done = true;
      }
    } else {
      done = true;
    }
    ++module_number;
  }
  // The second pass connects up all the modules into a tree.
  char module_child_var[AimParameters::MaxParamNameLength];
  for (int i = 0; i < modules_.size(); ++i) {
    int child_number = 1;
    done = false;
    sprintf(module_name_var, "module%d.name", module_number);
    if (config_.IsSet(module_name_var) {
      char* name = config_.GetString(module_name_var));
      while (!done) {
        sprintf(module_child_var, "module%d.child%d", i, child_number);
        if (config_.IsSet(module_child_var) {
          char* child = config_.GetString(module_child_var);
          modules_[name]->AddTarget(modules_[child].get());
        } else {
          done = true;
        }
      }
      ++child_number;
    } else {
      error = true;
      break;
    }
  }
  return error;
}

bool ModuleTree::Initialize(Parameters *global_parameters) {
  SignalBank s(1,1,1);
  modules_[root_name_]->Initialize(s, global_parameters);
}

}  // namespace aimc
