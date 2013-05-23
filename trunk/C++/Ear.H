
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
#ifndef EAR_H_
#define EAR_H_

#include "CAR.H"
#include "IHC.H"
#include "AGC.H"

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

Class which encapsulates an ear, using the CAR, IHC and AGC classes
*/
class Ear {
    FP_TYPE fs; ///< The sample rate
    FP_TYPE max_channels_per_octave; ///< How many channels in each octave
    int n_ch; ///< The number of channels (in the ear)

    CAR car; ///< The cascade of asymmetric resontaors for this ear
    IHC ihc; ///< The inner hair cell for this ear
    AGC agc; ///< The automatic gain control for this ear

public:
    /** Constructor specifying the sample rate
    \param fs_ The sample rate to use
    */
    Ear(FP_TYPE fs_);

    /** Constructor using the DEFAULT_SAMPLERATE as the sample rate
    */
    Ear(void);
    virtual ~Ear(void);

    /** Estimates the number of filter stages and executes the dsign method for each of CAR, AGC and IHC
    */
    void design(void);
};

#endif // EAR_H_
