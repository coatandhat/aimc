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
  CARParams();  // The constructor initializes using default parameter values.
  // This is a method to set non-default params.
  CARParams(FPType velocity_scale, FPType v_offset, FPType min_zeta,
            FPType max_zeta, FPType first_pole_theta, FPType zero_ratio,
            FPType high_f_damping_compression, FPType erb_per_step,
            FPType min_pole_hz_, FPType erb_break_freq, FPType erb_q_);
  FPType velocity_scale_; // This is used for the velocity nonlinearity.
  FPType v_offset_;  // The offset gives us quadratic part.
  FPType min_zeta_;  // This is the minimum damping factor in mid-freq channels.
  FPType max_zeta_;  // This is the maximum damping factor in mid-freq channels.
  FPType first_pole_theta_;
  FPType zero_ratio_;  // This is how far zero is above the pole.
  FPType high_f_damping_compression_;  // A range from 0 to 1 to compress theta.
  FPType erb_per_step_;  // Here we assume G&M's ERB formula.
  FPType min_pole_hz_;
  FPType erb_break_freq_;  // This is the Greenwood map's break frequency.
  FPType erb_q_;  // This represents Glassberg and Moore's high-cf ratio.
};

#endif