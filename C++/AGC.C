
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

#include "AGC.H"

AGC::AGC() {
}

AGC::~AGC() {
}

void AGC::designAGC(FP_TYPE fs, int n_ch) {
    int n_AGC_stages = params.n_stages;
//AGC_coeffs = struct( ...
//  'n_ch', n_ch, ...
//  'n_AGC_stages', n_AGC_stages, ...
//  'AGC_stage_gain', AGC_params.AGC_stage_gain);

// AGC1 pass is smoothing from base toward apex;
// AGC2 pass is back, which is done first now (in double exp. version)
//AGC1_scales = AGC_params.AGC1_scales;
//AGC2_scales = AGC_params.AGC2_scales;

    coeffs.AGC_epsilon = Array<FP_TYPE, 1, Dynamic>::Zero(1, n_AGC_stages);  // the 1/(tau*fs) roughly
    FP_TYPE decim = 1.;
//AGC_coeffs.decimation = AGC_params.decimation;

    FP_TYPE total_DC_gain = 0.;
    for (int stage = 1; stage<=n_AGC_stages; stage++) {
        FP_TYPE tau = params.time_constants(stage-1); // time constant in seconds
        decim = decim * params.decimation(stage-1); // net decim to this stage
        // epsilon is how much new input to take at each update step:
        coeffs.AGC_epsilon(stage-1) = 1. - exp(-decim / (tau * fs));
        // effective number of smoothings in a time constant:
        FP_TYPE ntimes = tau * (fs / decim);  // typically 5 to 50

        // decide on target spread (variance) and delay (mean) of impulse
        // response as a distribution to be convolved ntimes:
        // TODO (dicklyon): specify spread and delay instead of scales???
        FP_TYPE delay = (param.AGC2_scales(stage-1) - param.AGC1_scales(stage-1)) / ntimes;
        FP_TYPE spread_sq = (param.AGC1_scales(stage-1).pow(2.) + param.AGC2_scales(stage-1).pow(2)) / ntimes;

        // get pole positions to better match intended spread and delay of
        // [[geometric distribution]] in each direction (see wikipedia)
        FP_TYPE u = 1. + 1. / spread_sq; // these are based on off-line algebra hacking.
        FP_TYPE p = u - sqrt(pow(u,2.) - 1.); // pole that would give spread if used twice.
        FP_TYPE dp = delay * (1. - 2.*p +pow(p,2.))/2.;
                              FP_TYPE polez1 = p - dp;
                              FP_TYPE polez2 = p + dp;
                              coeffs.AGC_polez1(stage) = polez1;
                              coeffs.AGC_polez2(stage) = polez2;

                              // try a 3- or 5-tap FIR as an alternative to the double exponential:
                              Array<FP_TYPE,1, Dynamic> AGC_spatial_FIR;
                              int n_taps = 0;
                              int FIR_OK = 0;
                              int n_iterations = 1;
        while (~FIR_OK) {
        switch (n_taps) {
            case 0:
                // first attempt a 3-point FIR to apply once:
                n_taps = 3;
                break;
            case 3:
                // second time through, go wider but stick to 1 iteration
                n_taps = 5;
                break;
            case 5:
                // apply FIR multiple times instead of going wider:
                n_iterations = n_iterations + 1;
                if (n_iterations > 16) {
                    cerr<<"Too many n_iterations in CARFAC_DesignAGC"<<endl;
                    exit(AGC_DESIGN_ITERATION_ERROR);
                }
                break;
            default:
                // to do other n_taps would need changes in CARFAC_Spatial_Smooth
                // and in Design_FIR_coeffs
                cerr<<"Bad n_taps in CARFAC_DesignAGC"<<endl;
                exit(AGC_DESIGN_TAPS_OOB_ERROR);
                break;
            }
            FIR_OK = Design_FIR_coeffs(n_taps, spread_sq, delay, n_iterations,AGC_spatial_FIR);
        }
        // when FIR_OK, store the resulting FIR design in coeffs:
        coeff.AGC_spatial_iterations(stage-1) = n_iterations;
        coeff.AGC_spatial_FIR.col(stage-1).block(0,AGC_spatial_FIR.size()) = AGC_spatial_FIR;
        coeff.AGC_spatial_n_taps(stage-1) = n_taps;

        // accumulate DC gains from all the stages, accounting for stage_gain:
        total_DC_gain = total_DC_gain + params.AGC_stage_gain.pow(stage-1);

        // TODO (dicklyon) -- is this the best binaural mixing plan?
        if (stage == 1)
        coeff.AGC_mix_coeffs(stage-1) = 0.;
        else
            coeff.AGC_mix_coeffs(stage-1) = param.AGC_mix_coeff / (tau * (fs / decim));
        }

coeff.AGC_gain = total_DC_gain;

// adjust the detect_scale to be the reciprocal DC gain of the AGC filters:
AGC_coeffs.detect_scale = 1. / total_DC_gain;

}

int OK AGC::Design_FIR_coeffs(int n_taps, FP_TYPE var, FP_TYPE mn, int n_iter, Array<FP_TYPE,Dynamic,1> &FIR) {
// reduce mean and variance of smoothing distribution by n_iterations:
    mn = mn / (FP_TYPE)n_iter;
    var = var / (FP_TYPE)n_iter;
    switch (n_taps) {
    case 3:
        // based on solving to match mean and variance of [a, 1-a-b, b]:
        a = (var + mn*mn - mn) / 2.;
        b = (var + mn*mn + mn) / 2.;
        FIR.resize(3,1);
        FIR<<a, 1. - a - b, b;
        OK = FIR(2) >= 0.2;
    case 5
            // based on solving to match [a/2, a/2, 1-a-b, b/2, b/2]:
            a = ((var + mn*mn)*2./5. - mn*2./3.) / 2.;
        b = ((var + mn*mn)*2./5. + mn*2./3.) / 2.;
        // first and last coeffs are implicitly duplicated to make 5-point FIR:
        FIR.resize(5,1);
        FIR<<a/2., 1. - a - b, b/2.;
        OK = FIR(2) >= 0.1;
    default:
        cerr<<"Bad n_taps in AGC_spatial_FIR"<<endl;
        exit(AGC_FIR_TAP_COUNT_ERROR);
    }
}
