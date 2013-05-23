
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
/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08
*/

#include "AGCParam.H"

AGCParam::AGCParam(int n_stages_, Array<FP_TYPE, AGC_STAGE_COUNT,1> time_constants_,
             FP_TYPE AGC_stage_gain_, Array<int,AGC_STAGE_COUNT, 1> decimation_,
             Array<FP_TYPE, AGC_STAGE_COUNT,1> AGC1_scales_, Array<FP_TYPE, AGC_STAGE_COUNT,1> AGC2_scales_,
             FP_TYPE AGC_mix_coeff_) {
    n_stages=n_stages_;
    time_constants=time_constants_;
    AGC_stage_gain=AGC_stage_gain_;
    decimation=decimation_;
    AGC1_scales=AGC1_scales_;
    AGC2_scales=AGC2_scales_;
    AGC_mix_coeff=AGC_mix_coeff_;
}

AGCParam::~AGCParam() {
}
