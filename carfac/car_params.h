//
//  car_params.h
//  CARFAC Open Source C++ Library
//
//  Created by Alex Brandmeyer on 5/10/13.
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

#ifndef CARFAC_Open_Source_C__Library_CARParams_h
#define CARFAC_Open_Source_C__Library_CARParams_h

#include "carfac_common.h"

struct CARParams {
  // The constructor initializes using default parameter values.
  CARParams() {
    velocity_scale_ = 0.1;
    v_offset_ = 0.04;
    min_zeta_ = 0.1;
    max_zeta_ = 0.35;
    first_pole_theta_ = 0.85 * kPi;
    zero_ratio_ = sqrt(2.0);
    high_f_damping_compression_ = 0.5;
    erb_per_step_ = 0.5;
    min_pole_hz_ = 30;
    erb_break_freq_ = 165.3;  // This is the Greenwood map's break frequency.
    // This represents Glassberg and Moore's high-cf ratio.
    erb_q_ = 1000/(24.7*4.37);
  };
  FPType velocity_scale_; // This is used for the velocity nonlinearity.
  FPType v_offset_;  // The offset gives us quadratic part.
  FPType min_zeta_;  // This is the minimum damping factor in mid-freq channels.
  FPType max_zeta_;  // This is the maximum damping factor in mid-freq channels.
  FPType first_pole_theta_;
  FPType zero_ratio_;  // This is how far zero is above the pole.
  FPType high_f_damping_compression_;  // A range from 0 to 1 to compress theta.
  FPType erb_per_step_;
  FPType min_pole_hz_;
  FPType erb_break_freq_;
  FPType erb_q_;
};

#endif