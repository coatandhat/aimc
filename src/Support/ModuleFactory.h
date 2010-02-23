// Copyright 2008-2010, Thomas Walters
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