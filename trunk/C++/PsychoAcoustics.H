
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
#ifndef PSYCHOACOUSTICS_H_
#define PSYCHOACOUSTICS_H_

#include "CARFACCommon.H"

class PsychoAcoustics {
public:
    PsychoAcoustics() {}
    virtual ~PsychoAcoustics() {}

    /** Auditory filter nominal Equivalent Rectangular Bandwidth
        Ref: Glasberg and Moore: Hearing Research, 47 (1990), 103-138
     ERB = 24.7 * (1 + 4.37 * CF_Hz / 1000);
     \param CFHz The central frequency to find the ERB at.
     \param ERBBreakFreq The linear scaling factor for frequency
     \param ERBQ The tuning of the rectangular bandwidth
    */
    static FP_TYPE Hz2ERB(FP_TYPE CFHz, FP_TYPE ERBBreakFreq=1000./4.37 /* 228.833 */, FP_TYPE ERBQ=1000./(24.7*4.37) /* 9.2645 */);

    /** Auditory filter nominal Equivalent Rectangular Bandwidth
        Ref: Glasberg and Moore: Hearing Research, 47 (1990), 103-138
     ERB = 24.7 * (1 + 4.37 * CF_Hz / 1000);
     \param CFHz The central frequency to find the ERB at.
     \param ERBBreakFreq The linear scaling factor for frequency
     \param ERBQ The tuning of the rectangular bandwidth
    */
    static Array<FP_TYPE, Dynamic, 1> Hz2ERB(Array<FP_TYPE, Dynamic, 1> &CFHz, FP_TYPE ERBBreakFreq=1000./4.37 /* 228.833 */, FP_TYPE ERBQ=1000./(24.7*4.37) /* 9.2645 */);
};

#endif // PSYCHOACOUSTICS_H_
