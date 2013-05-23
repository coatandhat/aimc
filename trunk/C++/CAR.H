
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
#ifndef CAR_H_
#define CAR_H_

#include "EarComponent.H" // includes the various {CAR, IHC, AGC} {Coeff, Param, State} .H files

class Ear;

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

Cascade of asymmetric resonators (CAR).
*/
class CAR : public EarComponent<CARCoeff, CARParam, CARState> {
    Array<FP_TYPE, Dynamic,1> pole_freqs; ///< The vector of pole frequencies used in filter design and variable estimation

    protected:

    /** Method to design the auditory filter coefficients
    \param fs The sample rate in Hz
    \param n_ch The number of channels. The CAR.coeff are one row per channel
    */
    void designFilters(FP_TYPE fs, int n_ch);

    /** Return the stage gain g needed to get unity gain at DC
    \param relative_undamping Dick whats this ?
    \return The stage gain required to obtain 0 dB at DC.
    */
    Array<FP_TYPE, Dynamic, 1> stageG(Array<FP_TYPE, Dynamic, 1> &relative_undamping);

    public:
        CAR();
        virtual ~CAR();

    friend class Ear;
};

#endif // CAR_H_
