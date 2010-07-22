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

#include <stdlib.h>

#include <string>

#include "Modules/Input/ModuleFileInput.h"
#include "Modules/BMM/ModuleGammatone.h"
#include "Modules/BMM/ModulePZFC.h"
#include "Modules/NAP/ModuleHCL.h"
#include "Modules/Strobes/ModuleLocalMax.h"
#include "Modules/SAI/ModuleSAI.h"
#include "Modules/SSI/ModuleSSI.h"
#include "Modules/Profile/ModuleSlice.h"
#include "Modules/Profile/ModuleScaler.h"
#include "Modules/Features/ModuleGaussians.h"
#include "Modules/Output/FileOutputHTK.h"
#include "Modules/Output/FileOutputAIMC.h"

int main(int argc, char* argv[]) {
  aimc::Parameters params;

  int buffer_length = 480;
  params.SetInt("input.buffersize", buffer_length);

  aimc::ModuleFileInput input(&params);
  aimc::ModulePZFC bmm(&params);
  aimc::FileOutputAIMC output(&params);

  std::string parameters_string = params.WriteString();
  printf("%s", parameters_string.c_str());

  input.AddTarget(&bmm);
  bmm.AddTarget(&output);

  output.OpenFile("test_output.aimc", params.GetFloat("sai.frame_period_ms"));
  if (input.LoadFile("test.wav")) {
    input.Process();
  } else {
    printf("LoadFile failed");
  }
}
