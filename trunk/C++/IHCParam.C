
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

#include "IHCParam.H"

IHCParam::IHCParam(bool just_hwr_, bool one_cap_, FP_TYPE tau_lpf_,
                   FP_TYPE tau_out_, FP_TYPE tau_in_,
                   FP_TYPE tau1_out_, FP_TYPE tau1_in_, FP_TYPE tau2_out_,
                   FP_TYPE tau2_in_, FP_TYPE ac_corner_Hz_) {
    just_hwr=just_hwr_;
    one_cap=one_cap_;
    tau_lpf=tau_lpf_;
    tau_out=tau_out_;
    tau_in=tau_in_;
    tau1_out=tau1_out_;
    tau1_in=tau1_in_;
    tau2_out=tau2_out_;
    tau2_in=tau2_in_;
    ac_corner_Hz=ac_corner_Hz_;
}

IHCParam::~IHCParam() {
    //dtor
}
