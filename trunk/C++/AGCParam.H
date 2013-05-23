
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
#ifndef AGCPARAMS_H_
#define AGCPARAMS_H_

#include "Parameters.H"

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

    Parameters for the automatic gain control
*/
class AGCParam : public Parameters {
    int n_stages; ///< Stage count
    Array<FP_TYPE, AGC_STAGE_COUNT,1> time_constants; ///< seconds
    FP_TYPE AGC_stage_gain; ///< gain from each stage to next slower stage
    Array<int,AGC_STAGE_COUNT, 1> decimation; ///< how often to update the AGC states
    Array<FP_TYPE, AGC_STAGE_COUNT,1> AGC1_scales; ///< in units of channels
    Array<FP_TYPE, AGC_STAGE_COUNT,1> AGC2_scales; ///< spread more toward base
    FP_TYPE AGC_mix_coeff; ///< Dick, whats this ?
public:
    /// Constructor with default parameter values
    AGCParam(int n_stages_=AGC_STAGE_COUNT,
             Array<FP_TYPE, AGC_STAGE_COUNT,1> time_constants_=Array<FP_TYPE, AGC_STAGE_COUNT,1>(1., 4., 16., 64.)*0.002,
             FP_TYPE AGC_stage_gain_=2.,
             Array<int,AGC_STAGE_COUNT, 1> decimation_=Array<int,AGC_STAGE_COUNT, 1>(8, 2, 2, 2),
             Array<FP_TYPE, AGC_STAGE_COUNT,1> AGC1_scales_=Array<FP_TYPE, AGC_STAGE_COUNT,1>(1.0, 1.4,  2.0, 2.8),
             Array<FP_TYPE, AGC_STAGE_COUNT,1> AGC2_scales_=Array<FP_TYPE, AGC_STAGE_COUNT,1>(1.6, 2.25, 3.2, 4.5),
             FP_TYPE AGC_mix_coeff_=0.5);

    virtual ~AGCParam();
};

#endif // AGCPARAMS_H_
