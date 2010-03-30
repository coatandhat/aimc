// Copyright 2008-2010, Thomas Walters
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
 *  \brief Factory class for AIM-C modules.
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \date 2010/02/23
 *  \version \$Id$
 */

#ifndef AIMC_SUPPORT_MODULE_FACTORY_H_
#define AIMC_SUPPORT_MODULE_FACTORY_H_

#include <string>

#include "Support/Module.h"
#include "Support/Parameters.h"

namespace aimc {
/*! \brief Factory class for AIM-C modules.
 *
 * This class is the basis for a more complete module registration scheme to
 * be implemented in future. For now, all modules which are created have to
 * be added to this class individually. The goal is to eventally replace this
 * with a REGISTER_MODULE macro which can be added to the header file for 
 * every module.
 */

class ModuleFactory {
 public:
  static Module* Create(string module_name_, Parameters *params);
 private:
  DISALLOW_COPY_AND_ASSIGN(ModuleFactory);
};
}

#endif  // AIMC_SUPPORT_MODULE_FACTORY_H_