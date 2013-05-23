
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

#include "CARParam.H"

CARParam::CARParam(FP_TYPE velocity_scale_, FP_TYPE v_offset_, FP_TYPE min_zeta_, FP_TYPE max_zeta_,
                   FP_TYPE first_pole_theta_, FP_TYPE zero_ratio_, FP_TYPE high_f_damping_compression_,
                   FP_TYPE ERB_per_step_, FP_TYPE min_pole_Hz_, FP_TYPE ERB_break_freq_, FP_TYPE ERB_Q_) {
    velocity_scale=velocity_scale_;
    v_offset=v_offset_;
    min_zeta=min_zeta_;
    max_zeta=max_zeta_;
    first_pole_theta=first_pole_theta_;
    zero_ratio=zero_ratio_;
    high_f_damping_compression=high_f_damping_compression_;
    ERB_per_step=ERB_per_step_;
    min_pole_Hz=min_pole_Hz_;
    ERB_break_freq=ERB_break_freq_;
    ERB_Q=ERB_Q_;
}

CARParam::~CARParam() {}
