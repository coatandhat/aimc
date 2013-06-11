//
//  ear.cc
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

#include "ear.h"

#include <assert.h>

#include "carfac_util.h"

Ear::Ear(const int num_channels, const CARCoeffs& car_coeffs,
         const IHCCoeffs& ihc_coeffs,
         const std::vector<AGCCoeffs>& agc_coeffs) {
  Reset(num_channels, car_coeffs, ihc_coeffs, agc_coeffs);
}

void Ear::Reset(const int num_channels, const CARCoeffs& car_coeffs,
                const IHCCoeffs& ihc_coeffs,
                const std::vector<AGCCoeffs>& agc_coeffs) {
  num_channels_ = num_channels;
  car_coeffs_ = car_coeffs;
  ihc_coeffs_ = ihc_coeffs;
  agc_coeffs_ = agc_coeffs;
  ResetCARState();
  ResetIHCState();
  ResetAGCState();
}

void Ear::ResetCARState() {
  car_state_.z1_memory.setZero(num_channels_);
  car_state_.z2_memory.setZero(num_channels_);
  car_state_.za_memory.setZero(num_channels_);
  car_state_.zb_memory = car_coeffs_.zr_coeffs;
  car_state_.dzb_memory.setZero(num_channels_);
  car_state_.zy_memory.setZero(num_channels_);
  car_state_.g_memory = car_coeffs_.g0_coeffs;
  car_state_.dg_memory.setZero(num_channels_);
}

void Ear::ResetIHCState() {
  ihc_state_.ihc_accum = ArrayX::Zero(num_channels_);
  if (!ihc_coeffs_.just_half_wave_rectify) {
    ihc_state_.ac_coupler.setZero(num_channels_);
    ihc_state_.lpf1_state.setConstant(num_channels_, ihc_coeffs_.rest_output);
    ihc_state_.lpf2_state.setConstant(num_channels_, ihc_coeffs_.rest_output);
    if (ihc_coeffs_.one_capacitor) {
      ihc_state_.cap1_voltage.setConstant(num_channels_, ihc_coeffs_.rest_cap1);
    } else {
      ihc_state_.cap1_voltage.setConstant(num_channels_, ihc_coeffs_.rest_cap1);
      ihc_state_.cap2_voltage.setConstant(num_channels_, ihc_coeffs_.rest_cap2);
    }
  }
}

void Ear::ResetAGCState() {
  int n_agc_stages = agc_coeffs_.size();
  agc_state_.resize(n_agc_stages);
  for (auto& stage_state : agc_state_) {
    stage_state.decim_phase = 0;
    stage_state.agc_memory.setZero(num_channels_);
    stage_state.input_accum.setZero(num_channels_);
  }
}

void Ear::CARStep(const FPType input) {
  // Interpolates g.
  car_state_.g_memory = car_state_.g_memory + car_state_.dg_memory;
  // Calculates the AGC interpolation state.
  car_state_.zb_memory = car_state_.zb_memory + car_state_.dzb_memory;
  // This updates the nonlinear function of 'velocity' along with zA, which is
  // a delay of z2.
  ArrayX nonlinear_fun(num_channels_);
  ArrayX velocities = car_state_.z2_memory - car_state_.za_memory;
  OHCNonlinearFunction(velocities, &nonlinear_fun);
  // Here, zb_memory_ * nonlinear_fun is "undamping" delta r.
  ArrayX r = car_coeffs_.r1_coeffs + (car_state_.zb_memory * nonlinear_fun);
  car_state_.za_memory = car_state_.z2_memory;
  // Here we reduce the CAR state by r and rotate with the fixed cos/sin coeffs.
  ArrayX z1  = r * ((car_coeffs_.a0_coeffs * car_state_.z1_memory) -
                    (car_coeffs_.c0_coeffs * car_state_.z2_memory));
  car_state_.z2_memory = r *
    ((car_coeffs_.c0_coeffs * car_state_.z1_memory) +
     (car_coeffs_.a0_coeffs * car_state_.z2_memory));
  car_state_.zy_memory = car_coeffs_.h_coeffs * car_state_.z2_memory;
  // This section ripples the input-output path, to avoid delay...
  // It's the only part that doesn't get computed "in parallel":
  FPType in_out = input;
  for (int channel = 0; channel < num_channels_; channel++) {
    z1(channel) = z1(channel) + in_out;
    // This performs the ripple, and saves the final channel outputs in zy.
    in_out = car_state_.g_memory(channel) *
      (in_out + car_state_.zy_memory(channel));
    car_state_.zy_memory(channel) = in_out;
  }
  car_state_.z1_memory = z1;
}

// We start with a quadratic nonlinear function, and limit it via a
// rational function. This makes the result go to zero at high
// absolute velocities, so it will do nothing there.
void Ear::OHCNonlinearFunction(const ArrayX& velocities,
                               ArrayX* nonlinear_fun) const {
  *nonlinear_fun = (1 + ((velocities * car_coeffs_.velocity_scale) +
                         car_coeffs_.v_offset).square()).inverse();
}

// This step is a one sample-time update of the inner-hair-cell (IHC) model,
// including the detection nonlinearity and either one or two capacitor state
// variables.
void Ear::IHCStep(const ArrayX& car_out) {
  ArrayX ac_diff = car_out - ihc_state_.ac_coupler;
  ihc_state_.ac_coupler = ihc_state_.ac_coupler +
    (ihc_coeffs_.ac_coeff * ac_diff);
  if (ihc_coeffs_.just_half_wave_rectify) {
    ArrayX output(num_channels_);
    for (int channel = 0; channel < num_channels_; ++channel) {
      FPType a = (ac_diff(channel) > 0.0) ? ac_diff(channel) : 0.0;
      output(channel) = (a < 2) ? a : 2;
    }
    ihc_state_.ihc_out = output;
  } else {
    ArrayX conductance = CARFACDetect(ac_diff);
    if (ihc_coeffs_.one_capacitor) {
      ihc_state_.ihc_out = conductance * ihc_state_.cap1_voltage;
      ihc_state_.cap1_voltage = ihc_state_.cap1_voltage -
        (ihc_state_.ihc_out * ihc_coeffs_.out1_rate) +
        ((1 - ihc_state_.cap1_voltage) * ihc_coeffs_.in1_rate);
    } else {
      ihc_state_.ihc_out = conductance * ihc_state_.cap2_voltage;
      ihc_state_.cap1_voltage = ihc_state_.cap1_voltage -
        ((ihc_state_.cap1_voltage - ihc_state_.cap2_voltage)
         * ihc_coeffs_.out1_rate) + ((1 - ihc_state_.cap1_voltage) *
                                      ihc_coeffs_.in1_rate);
      ihc_state_.cap2_voltage = ihc_state_.cap2_voltage -
        (ihc_state_.ihc_out * ihc_coeffs_.out2_rate) +
        ((ihc_state_.cap1_voltage - ihc_state_.cap2_voltage)
         * ihc_coeffs_.in2_rate);
    }
    // Smooth the output twice using an LPF.
    ihc_state_.ihc_out *= ihc_coeffs_.output_gain;
    ihc_state_.lpf1_state += ihc_coeffs_.lpf_coeff *
      (ihc_state_.ihc_out - ihc_state_.lpf1_state);
    ihc_state_.lpf2_state += ihc_coeffs_.lpf_coeff *
      (ihc_state_.lpf1_state - ihc_state_.lpf2_state);
    ihc_state_.ihc_out = ihc_state_.lpf2_state - ihc_coeffs_.rest_output;
  }
  ihc_state_.ihc_accum += ihc_state_.ihc_out;
}

bool Ear::AGCStep(const ArrayX& ihc_out) {
  int stage = 0;
  int num_stages = agc_coeffs_[0].num_agc_stages;
  FPType detect_scale = agc_coeffs_[num_stages - 1].detect_scale;
  bool updated = AGCRecurse(stage, detect_scale * ihc_out);
  return updated;
}

bool Ear::AGCRecurse(const int stage, ArrayX agc_in) {
  bool updated = true;
  const auto& agc_coeffs = agc_coeffs_[stage];
  auto& agc_state = agc_state_[stage];
  // This is the decim factor for this stage, relative to input or prev. stage:
  int decim = agc_coeffs.decimation;
  // This is the decim phase of this stage (do work on phase 0 only):
  int decim_phase = agc_state.decim_phase + 1;
  decim_phase = decim_phase % decim;
  agc_state.decim_phase = decim_phase;
  // Here we accumulate input for this stage from the previous stage:
  agc_state.input_accum += agc_in;
  // We don't do anything if it's not the right decim_phase.
  if (decim_phase == 0) {
    // Now we do lots of work, at the decimated rate.
    // These are the decimated inputs for this stage, which will be further
    // decimated at the next stage.
    agc_in = agc_state.input_accum / decim;
    // This resets the accumulator.
    agc_state.input_accum = ArrayX::Zero(num_channels_);
    if (stage < (agc_coeffs_.size() - 1)) {
      // Now we recurse to evaluate the next stage(s).
      updated = AGCRecurse(stage + 1, agc_in);
      // Afterwards we add its output to this stage input, whether it updated or
      // not.
      agc_in += agc_coeffs.agc_stage_gain *
        agc_state_[stage + 1].agc_memory;
    }
    // This performs a first-order recursive smoothing filter update, in time.
    agc_state.agc_memory += agc_coeffs.agc_epsilon *
      (agc_in - agc_state.agc_memory);
    AGCSpatialSmooth(agc_coeffs_[stage], &agc_state.agc_memory);
    updated = true;
  } else {
    updated = false;
  }
  return updated;
}

void Ear::AGCSpatialSmooth(const AGCCoeffs& agc_coeffs,
                           ArrayX* stage_state) const {
  int num_iterations = agc_coeffs.agc_spatial_iterations;
  bool use_fir;
  use_fir = (num_iterations < 4) ? true : false;
  if (use_fir) {
    FPType fir_coeffs_left = agc_coeffs.agc_spatial_fir_left;
    FPType fir_coeffs_mid = agc_coeffs.agc_spatial_fir_mid;
    FPType fir_coeffs_right = agc_coeffs.agc_spatial_fir_right;
    ArrayX ss_tap1(num_channels_);
    ArrayX ss_tap2(num_channels_);
    ArrayX ss_tap3(num_channels_);
    ArrayX ss_tap4(num_channels_);
    int n_taps = agc_coeffs.agc_spatial_n_taps;
    // This initializes the first two taps of stage state, which are used for
    // both possible cases.
    ss_tap1(0) = (*stage_state)(0);
    ss_tap1.block(1, 0, num_channels_ - 1, 1) =
      stage_state->block(0, 0, num_channels_ - 1, 1);
    ss_tap2(num_channels_ - 1) = (*stage_state)(num_channels_ - 1);
    ss_tap2.block(0, 0, num_channels_ - 1, 1) =
      stage_state->block(1, 0, num_channels_ - 1, 1);
    switch (n_taps) {
      case 3:
        *stage_state = (fir_coeffs_left * ss_tap1) +
          (fir_coeffs_mid * *stage_state) + (fir_coeffs_right * ss_tap2);
        break;
      case 5:
        // Now we initialize last two taps of stage state, which are only used
        // for the 5-tap case.
        ss_tap3(0) = (*stage_state)(0);
        ss_tap3(1) = (*stage_state)(1);
        ss_tap3.block(2, 0, num_channels_ - 2, 1) =
          stage_state->block(0, 0, num_channels_ - 2, 1);
        ss_tap4(num_channels_ - 2) = (*stage_state)(num_channels_ - 1);
        ss_tap4(num_channels_ - 1) = (*stage_state)(num_channels_ - 2);
        ss_tap4.block(0, 0, num_channels_ - 2, 1) =
        stage_state->block(2, 0, num_channels_ - 2, 1);
        *stage_state = (fir_coeffs_left * (ss_tap3 + ss_tap1)) +
          (fir_coeffs_mid * *stage_state) +
          (fir_coeffs_right * (ss_tap2 + ss_tap4));
        break;
      default:
        assert(true && "Bad n_taps in AGCSpatialSmooth; should be 3 or 5.");
        break;
    }
  } else {
    AGCSmoothDoubleExponential(agc_coeffs.agc_pole_z1, agc_coeffs.agc_pole_z2,
                               stage_state);
  }
}

void Ear::AGCSmoothDoubleExponential(const FPType pole_z1,
                                     const FPType pole_z2,
                                     ArrayX* stage_state) const {
  int32_t num_points = stage_state->size();
  FPType input;
  FPType state = 0.0;
  // TODO(alexbrandmeyer): I'm assuming one dimensional input for now, but this
  // should be verified with Dick for the final version
  for (int i = num_points - 11; i < num_points; ++i) {
    input = (*stage_state)(i);
    state = state + (1 - pole_z1) * (input - state);
  }
  for (int i = num_points - 1; i > -1; --i) {
    input = (*stage_state)(i);
    state = state + (1 - pole_z2) * (input - state);
  }
  for (int i = 0; i < num_points; ++i) {
    input = (*stage_state)(i);
    state = state + (1 - pole_z1) * (input - state);
    (*stage_state)(i) = state;
  }
}

ArrayX Ear::StageGValue(const ArrayX& undamping) const {
  ArrayX r = car_coeffs_.r1_coeffs + car_coeffs_.zr_coeffs * undamping;
  return (1 - 2 * r * car_coeffs_.a0_coeffs + (r * r)) /
    (1 - 2 * r * car_coeffs_.a0_coeffs + car_coeffs_.h_coeffs * r *
     car_coeffs_.c0_coeffs + (r * r));
}
