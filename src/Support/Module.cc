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
Module::Module(Parameters *parameters) {
  initialized_ = false;
  targets_.clear();
  parameters_ = parameters;
  module_identifier_ = "MODULE IDENTIFIER NOT SET";
  module_type_ = "MODULE TYPE NOT SET";
  module_description_ = "MODULE DESCRIPTION NOT SET";
  module_version_ = "MODULE VERSION NOT SET";
};

Module::~Module() {
};

bool Module::Initialize(const SignalBank &input) {
  // LOG_INFO_NN(_T("-> %s "), module_identifier_.c_str());
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
  // of this module..
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
      if (!(*it)->initialized())
        if (!(*it)->Initialize(output_))
          return false;
    }
  } else {
    // LOG_INFO(_T("|"));
  }
  initialized_ = true;
  return true;
}

void Module::Reset() {
  if (!initialized_)
    return;

  // LOG_INFO("Resetting module %s", module_identifier_.c_str());
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
  if (target_module) {
    pair<set<Module*>::iterator, bool> ret;
    ret = targets_.insert(target_module);
    return ret.second;
  }
  return false;
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
}  // namespace aimc

