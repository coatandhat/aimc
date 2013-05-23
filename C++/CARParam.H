
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
#ifndef CARPARAMS_H_
#define CARPARAMS_H_

#include "Parameters.H"

class CAR; // required for decleration as a friend
class Ear;

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

Parameters for the Cascaded Auditory Resonators
*/
class CARParam : public Parameters {
    FP_TYPE velocity_scale; ///< for the velocity nonlinearity
    FP_TYPE v_offset; ///<  offset gives a quadratic part
    FP_TYPE min_zeta; ///<  minimum damping factor in mid-freq channels
    FP_TYPE max_zeta; ///<  maximum damping factor in mid-freq channels
    FP_TYPE first_pole_theta; ///< Dick, whats this ?
    FP_TYPE zero_ratio; ///<  how far zero is above pole
    FP_TYPE high_f_damping_compression; ///<  0 to 1 to compress zeta
    FP_TYPE ERB_per_step; ///<  assume G&M's ERB formula
    FP_TYPE min_pole_Hz; ///< Dick, whats this ?
    FP_TYPE ERB_break_freq; ///<  Greenwood map's break freq.
    FP_TYPE ERB_Q; ///<  Glasberg and Moore's high-cf ratio

public:

    /// Constructor with default parameter values
    CARParam(FP_TYPE velocity_scale_=0.1, FP_TYPE v_offset_=0.04, FP_TYPE min_zeta_=0.10, FP_TYPE max_zeta_=0.35,
             FP_TYPE first_pole_theta_=0.85*M_PI, FP_TYPE zero_ratio_=sqrt(2.), FP_TYPE high_f_damping_compression_=0.5,
             FP_TYPE ERB_per_step_=0.5, FP_TYPE min_pole_Hz_=30., FP_TYPE ERB_break_freq_=165.3,
             FP_TYPE ERB_Q_=1000./(24.7*4.37));
    virtual ~CARParam(); ///< Destructor

    friend class CAR;
    friend class Ear;
};

#endif // CARPARAMS_H_
