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

#include "Support/ModuleFactory.h"
#include "Support/Module.h"
#include "Support/ModuleTree.h"

namespace aimc {
using std::endl;
ModuleTree::ModuleTree() : root_module_(NULL),
                           initialized_(false) {
  
}
  
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
  int module_number = 1;
  LOG_INFO("Parsing tree...");
  while (!done) {  
    char module_name_var[Parameters::MaxParamNameLength];
    char module_id_var[Parameters::MaxParamNameLength];
    char module_parameters_var[Parameters::MaxParamNameLength];
    sprintf(module_name_var, "module%d.name", module_number);
    sprintf(module_id_var, "module%d.id", module_number);
    sprintf(module_parameters_var, "module%d.parameters", module_number);
    if (config_.IsSet(module_name_var)) {
      string module_name(config_.GetString(module_name_var));
      LOG_INFO("Module number %d, name %s", module_number, module_name.c_str());
      if (config_.IsSet(module_id_var)) {
        const char* id = config_.GetString(module_id_var);
        const char* parameters = config_.DefaultString(module_parameters_var, "");
        linked_ptr<Parameters> params(new Parameters);
        parameters_[module_name] = params;
        parameters_[module_name]->Parse(parameters);
        linked_ptr<Module> module(ModuleFactory::Create(id, parameters_[module_name].get()));
        if (module.get() != NULL) {
          module->set_instance_name(module_name);
          modules_[module_name] = module;
        } else {
          LOG_ERROR("Module name: %s of type %s not created", module_name.c_str(), id);
          done = true;
          error = true;
        }
        if (module_number == 1) {
          root_module_ = module.get();
        }
      } else {
        LOG_ERROR("id field missing for module named %s", module_name.c_str());
        error = true;
        done = true;
      }
    } else {
      done = true;
    }
    ++module_number;
  }
  // The second pass connects up all the modules into a tree.
  char module_child_var[Parameters::MaxParamNameLength];
  char module_name_var[Parameters::MaxParamNameLength];
  LOG_INFO("A total of %d modules", modules_.size());
  for (unsigned int i = 1; i < modules_.size() + 1; ++i) {
    int child_number = 1;
    done = false;
    sprintf(module_name_var, "module%d.name", i);
    if (config_.IsSet(module_name_var)) {
      string module_name(config_.GetString(module_name_var));
      while (!done) {
        sprintf(module_child_var, "module%d.child%d", i, child_number);
        LOG_INFO("Trying %s", module_child_var);
        if (config_.IsSet(module_child_var)) {
          string child(config_.GetString(module_child_var));
          if ((modules_.find(module_name) != modules_.end())
              && (modules_.find(child) != modules_.end())) {
            modules_[module_name]->AddTarget(modules_[child].get());
          } else {
            LOG_ERROR("Module name not found");
          }
        } else {
          done = true;
        }
        ++child_number;
      }
    } else {
      LOG_ERROR("field missing for entry %s", module_name_var);
      error = true;
      break;
    }
  }
  return !error;
}

bool ModuleTree::Initialize(Parameters *global_parameters) {
  if (root_module_ == NULL) {
    return false;
  }
  // Dummy signal bank for the root module.
  s_.Initialize(1, 1, 1);
  initialized_ = root_module_->Initialize(s_, global_parameters);
  return initialized_;
}

void ModuleTree::Reset() {
  if (root_module_ == NULL) {
    return;
  }
  root_module_->Reset();
}

void ModuleTree::PrintConfiguration(ostream &out) {
  if (root_module_ == NULL) {
    return;
  }
  root_module_->PrintTargets(out);
  root_module_->PrintConfiguration(out);
}

void ModuleTree::Process() {
  if (root_module_ == NULL) {
    return;
  }
  if (!initialized_) {
    LOG_ERROR(_T("Module tree not initialized."));
    return;
  }
  while (!root_module_->done()) {
    root_module_->Process(s_);
  }
}

void ModuleTree::MakeDotGraph(ostream &out) {
  if (root_module_ == NULL) {
    return;
  }
  out << "digraph G {" << endl;
  root_module_->PrintTargetsForDot(out);
  out << "}" << endl;
}

}  // namespace aimc
