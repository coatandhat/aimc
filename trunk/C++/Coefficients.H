
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
#ifndef COEFFICIENTS_H_
#define COEFFICIENTS_H_

#include "CARFACCommon.H"

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

    This class is common to the CAR, AGC and IHC classes.
    It provides virtual methods to give a common framework.

*/
class Coefficients {
public:
    Coefficients();
    virtual ~Coefficients();
};

#endif // COEFFICIENTS_H_
