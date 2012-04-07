#ifndef CAR_H_
#define CAR_H_

#include "CARFAC_common.h"
#include <cmath>

class CAR_parameters{
public:
  CAR_parameters():
    velocity_scale_(0.2),
    v_offset_(0.01),
    v2_corner_(0.2),
    v_damp_max_(0.01),
    min_zeta_(0.10),
    first_pole_theta_(0.085*kPi),
    zero_ratio_(sqrt(2)),
    high_f_damping_compression_(0.5),
    erb_per_step_(0.5),
    min_pole_hz_(30){
    // do nothing more
  }

  virtual ~CAR_parameters(){
    // do nothing
  }

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
};

class CAR_coefficients{
public:
  CAR_coefficients(CAR_parameters*, float, FloatArray);
  virtual ~CAR_coefficients();

  FloatArray r1_coeffs_;
  FloatArray a0_coeffs_;
  FloatArray c0_coeffs_;
  FloatArray h_coeffs_;
  FloatArray g0_coeffs_;
  FloatArray zr_coeffs_;

private:
  CAR_coefficients();
};

#endif /* CAR_H_ */
