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

#include "Modules/Input/ModuleFileInput.h"
#include "Modules/BMM/ModuleGammatone.h"
#include "Modules/BMM/ModulePZFC.h"
#include "Modules/NAP/ModuleHCL.h"
#include "Modules/Strobes/ModuleParabola.h"
#include "Modules/SAI/ModuleSAI.h"
#include "Modules/SSI/ModuleSSI.h"
#include "Modules/Profile/ModuleSlice.h"
#include "Modules/Profile/ModuleScaler.h"
#include "Modules/Features/ModuleGaussians.h"
#include "Modules/Output/FileOutputHTK.h"

#include "Support/ModuleFactory.h"

namespace aimc {
Module* ModuleFactory::Create(string module_name_, Parameters* params) {
  if (module_name_.compare("gt") == 0)
    return new ModuleGammatone(params);

  if (module_name_.compare("pzfc") == 0)
    return new ModulePZFC(params);

  if (module_name_.compare("gaussians") == 0)
    return new ModuleGaussians(params);

  if (module_name_.compare("file_input") == 0)
    return new ModuleFileInput(params);

  if (module_name_.compare("hcl") == 0)
    return new ModuleHCL(params);

  if (module_name_.compare("htk_out") == 0)
    return new FileOutputHTK(params);

  if (module_name_.compare("scaler") == 0)
    return new ModuleScaler(params);

  if (module_name_.compare("slice") == 0)
    return new ModuleSlice(params);

  if (module_name_.compare("weighted_sai") == 0)
    return new ModuleSAI(params);

  if (module_name_.compare("ssi") == 0)
    return new ModuleSSI(params);

  if (module_name_.compare("parabola") == 0)
    return new ModuleParabola(params);

  return NULL;
}
}  // namespace aimc
