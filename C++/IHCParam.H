
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
#ifndef IHCPARAMS_H_
#define IHCPARAMS_H_

#include "Parameters.H"

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

    Parameters for the inner hair cells
*/
class IHCParam : public Parameters {
        bool just_hwr; ///< not just a simple HWR
        FP_TYPE ac_corner_Hz; ///< Hz (the only param. used if just_hwr=true)
        bool one_cap; ///< 0 for new two-cap hack
        FP_TYPE tau_lpf; ///< 80 microseconds smoothing twice
        FP_TYPE tau_out; ///< depletion tau is pretty fast (used when one_cap=1)
        FP_TYPE tau_in; ///< recovery tau is slower (used when one_cap=1)
        FP_TYPE tau1_out; ///< depletion tau is pretty fast (used when one_cap=0)
        FP_TYPE tau1_in; ///< recovery tau is slower (used when one_cap=0)
        FP_TYPE tau2_out; ///< depletion tau is pretty fast (used when one_cap=0)
        FP_TYPE tau2_in; ///< recovery tau is slower (used when one_cap=0)
public:
    IHCParam(bool just_hwr_=0, bool one_cap_=0, FP_TYPE tau_lpf_=0.000080,
        FP_TYPE tau_out_=0.0005, FP_TYPE tau_in_=0.010,
        FP_TYPE tau1_out_=0.010, FP_TYPE tau1_in_=0.020, FP_TYPE tau2_out_=0.0025,
        FP_TYPE tau2_in_=0.005, FP_TYPE ac_corner_Hz_=20.);

    virtual ~IHCParam();
};

#endif // IHCPARAMS_H_
