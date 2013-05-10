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

#include "Modules/Features/ModuleGaussians.h"
//#include "Modules/Features/ModuleDCT.h"
#include "Modules/BMM/ModuleGammatone.h"
#include "Modules/BMM/ModulePZFC.h"
#include "Modules/Input/ModuleFileInput.h"
#include "Modules/NAP/ModuleHCL.h"
#include "Modules/Output/FileOutputHTK.h"
#include "Modules/Output/FileOutputAIMC.h"
#include "Modules/Output/FileOutputJSON.h"
//#include "Modules/Output/OSCOutput.h"
#include "Modules/Output/Graphics/GraphicsViewTime.h"
#include "Modules/Profile/ModuleSlice.h"
#include "Modules/Profile/ModuleScaler.h"
#include "Modules/SAI/ModuleSAI.h"
#include "Modules/SSI/ModuleSSI.h"
//#include "Modules/SNR/ModuleNoise.h"
#include "Modules/Strobes/ModuleParabola.h"
#include "Modules/Strobes/ModuleLocalMax.h"

#include "Support/ModuleFactory.h"

namespace aimc {
Module* ModuleFactory::Create(string module_name_, Parameters* params) {
  if (module_name_.compare("gaussians") == 0)
    return new ModuleGaussians(params);

  //if (module_name_.compare("dct") == 0)
  //  return new ModuleDCT(params);

  if (module_name_.compare("gt") == 0)
    return new ModuleGammatone(params);

  if (module_name_.compare("pzfc") == 0)
    return new ModulePZFC(params);

  if (module_name_.compare("file_input") == 0)
    return new ModuleFileInput(params);

  if (module_name_.compare("hcl") == 0)
    return new ModuleHCL(params);

  if (module_name_.compare("htk_out") == 0)
    return new FileOutputHTK(params);

  // if (module_name_.compare("osc_out") == 0)
  //   return new OSCOutput(params);

  if (module_name_.compare("aimc_out") == 0)
    return new FileOutputAIMC(params);

  if (module_name_.compare("json_out") == 0)
    return new FileOutputJSON(params);

  if (module_name_.compare("graphics_time") == 0)
    return new GraphicsViewTime(params);

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

  if (module_name_.compare("local_max") == 0)
    return new ModuleLocalMax(params);

  return NULL;
}
}  // namespace aimc
