
// Author Matt Flax <flatmax@>
//
// This C++ file is part of an implementation of Lyon's cochlear model:
// "Cascade of Asymmetric Resonators with Fast-Acting Compression"
// to supplement Lyon's upcoming book "Human and Machine Hearing"
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
#ifndef EARCOMPONENT_H_
#define EARCOMPONENT_H_

#include "CARCoeff.H" // includes are for template definitions in the .C file.
#include "CARParam.H" // handy to include here to avoid double linking when only in the EarComponent.C file
#include "CARState.H"
#include "AGCCoeff.H"
#include "AGCParam.H"
#include "AGCState.H"
#include "IHCCoeff.H"
#include "IHCParam.H"
#include "IHCState.H"

#include "CARFACCommon.H"

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

This class is inherited by the CAR, AGC and IHC classes.
This class groups Parameters, Coefficient and State classes.
*/
template <class COEFFTYPE, class PARAMTYPE, class STATETYPE>
class EarComponent {
protected:
    COEFFTYPE coeff;
    PARAMTYPE param;
    STATETYPE state;
public:
    EarComponent() {}
    virtual ~EarComponent(){}
};

#endif // EARCOMPONENT_H_
