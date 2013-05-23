
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

#include "Ear.H"

Ear::Ear(FP_TYPE fs_) {
    fs=fs_; // set the specified sample rate
    design();
}

Ear::Ear(void) {
    fs=DEFAULT_SAMPLERATE; // Use the default sample rate
    design();
}

Ear::~Ear(void) {
}

void Ear::design(void) {

    // first figure out how many filter stages (PZFC/car.AC channels):
    FP_TYPE pole_Hz = car.param.first_pole_theta * fs / (2.*M_PI);
    n_ch = 0;
    while (pole_Hz > car.param.min_pole_Hz) {
        n_ch = n_ch + 1;
        pole_Hz = pole_Hz - car.param.ERB_per_step *
                  PsychoAcoustics::Hz2ERB(pole_Hz, car.param.ERB_break_freq, car.param.ERB_Q);
    }
    // Now we have n_ch, the number of channels, so can make the array
    // and compute all the frequencies again to put into it:
    car.pole_freqs.resize(n_ch, NoChange);
    pole_Hz = car.param.first_pole_theta * fs / (2.*M_PI);
    for (int ch = 0; ch<n_ch; ch++) {
        car.pole_freqs[ch] = pole_Hz;
        pole_Hz = pole_Hz - car.param.ERB_per_step *
                  PsychoAcoustics::Hz2ERB(pole_Hz, car.param.ERB_break_freq, car.param.ERB_Q);
    }
    // now we have n_ch, the number of channels, and pole_freqs array

    max_channels_per_octave = (FP_TYPE)(log(2.) / log(car.pole_freqs[0]/car.pole_freqs[1]));

    // convert to include an ear_array, each w coeffs and state...
    car.designFilters(fs, n_ch);
    AGC.designAGC(fs, n_ch);
    //IHC.designIHC(CF_IHC_params, fs, n_ch);
}
