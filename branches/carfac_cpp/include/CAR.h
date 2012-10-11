#ifndef CAR_H_
#define CAR_H_

#include "CARFAC_common.h"
#include <cmath>

const double kDefaultErbBreakFreq = 165.3;
const double kDefaultErbQ = 1000/(24.7*4.37);

class CAR_parameters{
public:
  CAR_parameters();

  float velocity_scale_;
  float v_offset_;
  float v2_corner_;
  float v_damp_max_;
  float min_zeta_;
  float first_pole_theta_;
  float zero_ratio_;
  float high_f_damping_compression_;
  float erb_per_step_;
  float min_pole_hz_;
  float erb_break_freq_;
  float erb_q_;
};

class CAR_coefficients{
public:
  CAR_coefficients(CAR_parameters* car_params_p, float fs,
      FloatArray pole_freqs);

  FloatArray r1_coeffs_;
  FloatArray a0_coeffs_;
  FloatArray c0_coeffs_;
  FloatArray h_coeffs_;
  FloatArray g0_coeffs_;
  FloatArray zr_coeffs_;

};

#endif /* CAR_H_ */
