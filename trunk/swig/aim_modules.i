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

%module aimc
%include "std_string.i"
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
#include "Modules/SNR/ModuleNoise.h"
#include "Modules/Profile/ModuleSlice.h"
#include "Modules/Profile/ModuleScaler.h"
#include "Modules/Features/ModuleGaussians.h"
#include "Modules/Output/FileOutputHTK.h"
%}

%include "Support/Parameters.h"
%include "Support/SignalBank.h"
%include "Support/Module.h"
%include "Modules/BMM/ModuleGammatone.h"
%include "Modules/BMM/ModulePZFC.h"
%include "Modules/NAP/ModuleHCL.h"
%include "Modules/Strobes/ModuleParabola.h"
%include "Modules/Strobes/ModuleLocalMax.h"
%include "Modules/SAI/ModuleSAI.h"
%include "Modules/SSI/ModuleSSI.h"
%include "Modules/SNR/ModuleNoise.h"
%include "Modules/Profile/ModuleSlice.h"
%include "Modules/Profile/ModuleScaler.h"
%include "Modules/Features/ModuleGaussians.h"
%include "Modules/Output/FileOutputHTK.h"
