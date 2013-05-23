
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

#include "CAR.H"

CAR::CAR() {
    //ctor
}

CAR::~CAR() {
    //dtor
}

void CAR::designFilters(FP_TYPE fs, int n_ch) {
    // don't really need these zero arrays, but it's a clue to what fields
    // and types are need in ohter language implementations:
    coeff.r1_coeffs=Matrix<FP_TYPE, Dynamic, 1>::Zero(n_ch,1); // resize and zero
    coeff.a0_coeffs=Matrix<FP_TYPE, Dynamic, 1>::Zero(n_ch,1);
    coeff.c0_coeffs=Matrix<FP_TYPE, Dynamic, 1>::Zero(n_ch,1);
    coeff.h_coeffs=Matrix<FP_TYPE, Dynamic, 1>::Zero(n_ch,1);
    coeff.g0_coeffs=Matrix<FP_TYPE, Dynamic, 1>::Zero(n_ch,1);
    // zr_coeffs is not zeroed ... perhaps it should be ?

// zero_ratio comes in via h.  In book's circuit D, zero_ratio is 1/sqrt(a),
// and that a is here 1 / (1+f) where h = f*c.
// solve for f:  1/zero_ratio^2 = 1 / (1+f)
// zero_ratio^2 = 1+f => f = zero_ratio^2 - 1
    FP_TYPE f = pow(param.zero_ratio,2.) - 1.;  // nominally 1 for half-octave

// Make pole positions, s and c coeffs, h and g coeffs, etc.,
// which mostly depend on the pole angle theta:
    Array<FP_TYPE, Dynamic, 1> theta = pole_freqs * (2. * M_PI / fs);


// undamped coupled-form coefficients:
    coeff.c0_coeffs = theta.sin();
    coeff.a0_coeffs = theta.cos();

// different possible interpretations for min-damping r:
// r = exp(-theta * CF_CAR_params.min_zeta).
// Compress theta to give somewhat higher Q at highest thetas:
    FP_TYPE ff = param.high_f_damping_compression; // 0 to 1 typ. 0.5
    Array<FP_TYPE, Dynamic,1> x = theta/M_PI;

    coeff.zr_coeffs = M_PI * (x - ff * x.pow(3.));  // when ff is 0, this is just theta,
//                       and when ff is 1 it goes to zero at theta = pi.
    coeff.r1_coeffs = (1. - coeff.zr_coeffs.array() * param.max_zeta);  // "r1" for the max-damping condition

// Increase the min damping where channels are spaced out more, by pulling
// 25% of the way toward ERB_Hz/pole_freqs (close to 0.1 at high f)
    Array<FP_TYPE, Dynamic, 1> min_zetas = param.min_zeta + 0.25*(PsychoAcoustics::Hz2ERB(pole_freqs, param.ERB_break_freq, param.ERB_Q).array() / pole_freqs - param.min_zeta);
    coeff.zr_coeffs = coeff.zr_coeffs.array() * (param.max_zeta - min_zetas); // how r relates to undamping

// the zeros follow via the h_coeffs
    coeff.h_coeffs = coeff.c0_coeffs * f;

// for unity gain at min damping, radius r; only used in CARFAC_Init:
    Array<FP_TYPE, Dynamic,1> relative_undamping(n_ch, 1);
    relative_undamping=Array<FP_TYPE, Dynamic, 1>::Zero(n_ch, 1).cos();

// this function needs to take CAR_coeffs even if we haven't finished
// constucting it by putting in the g0_coeffs:
    coeff.g0_coeffs = stageG(relative_undamping);
}

Array<FP_TYPE, Dynamic, 1> CAR::stageG(Array<FP_TYPE, Dynamic, 1> &relative_undamping) {
    // at max damping
    Array<FP_TYPE, Dynamic, 1> r  = coeff.r1_coeffs.array() + coeff.zr_coeffs.array() * relative_undamping;
    return (1. - 2.*r*coeff.a0_coeffs.array() + r.pow(2.)) / (1. - 2.*r*coeff.a0_coeffs.array() + coeff.h_coeffs.array()*r*coeff.c0_coeffs.array() + pow(r,2.));
}
