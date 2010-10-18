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
 *  \brief Base class for all AIM-C modules.
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \date 2010/01/23
 *  \version \$Id$
 */

#include "Support/Module.h"

#include <utility>

namespace aimc {
using std::pair;
using std::ostream;
using std::endl;
Module::Module(Parameters *parameters) {
  initialized_ = false;
  targets_.clear();
  parameters_ = parameters;
  module_identifier_ = "MODULE IDENTIFIER NOT SET";
  module_type_ = "MODULE TYPE NOT SET";
  module_description_ = "MODULE DESCRIPTION NOT SET";
  module_version_ = "MODULE VERSION NOT SET";
  instance_name_ = "";
  done_ = false;
};

Module::~Module() {
};

bool Module::Initialize(const SignalBank &input,
                        Parameters *global_parameters) {
  if (global_parameters == NULL) {
    return false;
  }
  global_parameters_ = global_parameters;
  // Validate the input
  if (!input.Validate()) {
    LOG_ERROR(_T("Input SignalBank not valid"));
    return false;
  }
  if (!InitializeInternal(input)) {
    LOG_ERROR(_T("Initialization failed in module %s"),
              module_identifier_.c_str());
    return false;
  }
  // If the module has an output bank, then we can set up the targets
  // of this module.
  if (output_.initialized()) {
    // Check that the output SignalBank has been set up correctly
    if (!output_.Validate()) {
      LOG_ERROR(_T("Output SignalBank not valid in module %s"),
                module_identifier_.c_str());
      return false;
    }
    // Iterate through all the targets of this module, initializing
    // them if they have not already been initialized. If they have
    // already been initialized, then they are assumed to have been
    // set up to accept an input SignalBank of the correct form, but
    // this is not checked.
    set<Module*>::const_iterator it;
    for (it = targets_.begin(); it != targets_.end(); ++it) {
      //if (!(*it)->initialized())
        if (!(*it)->Initialize(output_, global_parameters_))
          return false;
    }
  }
  initialized_ = true;
  return true;
}

void Module::Reset() {
  if (!initialized_)
    return;

  ResetInternal();

  // Iterate through all the targets of this module, resetting
  // them.
  set<Module*>::const_iterator it;
  for (it = targets_.begin(); it != targets_.end(); ++it)
    (*it)->Reset();
}

bool Module::initialized() const {
  return initialized_;
}

bool Module::AddTarget(Module* target_module) {
  bool result = false;
  if (target_module) {
    pair<set<Module*>::iterator, bool> ret;
    ret = targets_.insert(target_module);
    result = ret.second;
    if (result) {
      if (initialized_) {
        if (output_.initialized()) {
          if (!target_module->initialized()) {
            target_module->Initialize(output_, global_parameters_);
          }
        }
      }
    }
  }
  return result;
}

bool Module::RemoveTarget(Module* target_module) {
  if (targets_.erase(target_module) != 0)
    return true;
  return false;
}

void Module::RemoveAllTargets() {
  targets_.clear();
}

const SignalBank* Module::GetOutputBank() const {
  return &output_;
}

void Module::PushOutput() {
  if (output_.initialized()) {
    set<Module*>::const_iterator it;
    for (it = targets_.begin(); it != targets_.end(); ++it) {
      (*it)->Process(output_);
    }
  }
}

void Module::PrintTargetsForDot(ostream &out) {
  //string parameters_string = parameters_->WriteString();
  out << "  " << instance_name() << " [shape = none, margin = 0, label = <" << endl;
  out << "  <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\"> " << endl;
  out << " <TR><TD>" << instance_name() << "</TD></TR><TR><TD>" << id();
  out << "</TD></TR></TABLE>>]" << ";" << endl;
  // <TD><TR>" << parameters_string << "</TD></TR>
  set<Module*>::const_iterator it;
  for (it = targets_.begin(); it != targets_.end(); ++it) {
    out << "  " << instance_name() << " -> " << (*it)->instance_name() << ";" << endl;
    (*it)->PrintTargetsForDot(out);
  }
}

void Module::PrintTargets(ostream &out) {
  set<Module*>::const_iterator it;
  for (it = targets_.begin(); it != targets_.end(); ++it) {
    out << "  " << instance_name() << " -> " << (*it)->instance_name() << ";" << endl;
    (*it)->PrintTargets(out);
  }
}

void Module::PrintConfiguration(ostream &out) {
  out << "# " << id() << endl;
  out << "# " << instance_name() << endl;
  out << "# " << version() << endl;
  string parameters_string = parameters_->WriteString();
  out << parameters_string << endl;
  set<Module*>::const_iterator it;
  for (it = targets_.begin(); it != targets_.end(); ++it) {
     (*it)->PrintConfiguration(out);
  }
}
}  // namespace aimc

