
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
#ifndef AGC_H_
#define AGC_H_

#include "EarComponent.H" // includes the various {CAR, IHC, AGC} {Coeff, Param, State} .H files

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

    Automatic Gain Control
*/
class AGC : EarComponent<AGCCoeff, AGCParam, AGCState> {
    public:
        AGC();
        virtual ~AGC();

    /** Method to design the automatic gain control coefficients
    \param fs The sample rate in Hz
    \param n_ch The number of channels. The CAR.coeff are one row per channel
    */
    void designAGC(FP_TYPE fs, int n_ch);
};

#endif // AGC_H_
