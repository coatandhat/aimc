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

%module aimc
%include "std_string.i"
%include stl.i
%include "std_vector.i"

namespace std {
   %template(FloatVector) vector<float>;
}

%{
#include "Support/Common.h"
#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"
#include "Modules/BMM/ModuleGammatone.h"
#include "Modules/BMM/ModulePZFC.h"
#include "Modules/NAP/ModuleHCL.h"
#include "Modules/Strobes/ModuleParabola.h"
#include "Modules/Strobes/ModuleLocalMax.h"
#include "Modules/SAI/ModuleSAI.h"
#include "Modules/SSI/ModuleSSI.h"
#include "Modules/Profile/ModuleSlice.h"
#include "Modules/Profile/ModuleScaler.h"
#include "Modules/Features/ModuleGaussians.h"
%}

%include "Support/Parameters.h"
using namespace std;
%include "Support/SignalBank.h"

namespace aimc {
using std::ostream;
using std::set;
using std::string;
class Module {
 public:
  explicit Module(Parameters *parameters);
  virtual ~Module();
  virtual bool Initialize(const SignalBank &input,
                          Parameters *global_parameters);
  bool initialized() const;
  bool AddTarget(Module* target_module);
  bool RemoveTarget(Module* target_module);
  void RemoveAllTargets();
  virtual void Process(const SignalBank &input) = 0;
  void Reset();
  const SignalBank* GetOutputBank() const;
  string version() const;
  string id() const;
  string description() const;
  string type() const;
};
}

%include "Modules/BMM/ModuleGammatone.h"
%include "Modules/BMM/ModulePZFC.h"
%include "Modules/NAP/ModuleHCL.h"
%include "Modules/Strobes/ModuleParabola.h"
%include "Modules/Strobes/ModuleLocalMax.h"
%include "Modules/SAI/ModuleSAI.h"
%include "Modules/SSI/ModuleSSI.h"
%include "Modules/Profile/ModuleSlice.h"
%include "Modules/Profile/ModuleScaler.h"
%include "Modules/Features/ModuleGaussians.h"
