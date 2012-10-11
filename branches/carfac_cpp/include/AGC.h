#ifndef AGC_H_
#define AGC_H_

#include "CARFAC_common.h"

class AGC_parameters {
public:
  AGC_parameters();

  int n_stages_;
  FloatArray time_constants_;
  float agc_stage_gain_;
  FloatArray decimation_;
  FloatArray agc1_scales_;
  FloatArray agc2_scales_;
  float detect_scale_;
  float agc_mix_coeff_;
};

class AGC_coefficients {
public:
  AGC_coefficients(AGC_parameters*, float, int);

  int n_ch_;
  int n_agc_stages_;
  float agc_stage_gain_;
  FloatArray agc_epsilon_;
  FloatArray decimation_;
  FloatArray agc_polez1_;
  FloatArray agc_polez2_;
  FloatArray agc_spatial_iterations_;
  FloatMatrix agc_spatial_fir_;
  FloatArray agc_spatial_n_taps_;
  FloatArray agc_mix_coeffs_;
  float agc_gain_;
  float detect_scale_;
  
private:
  FloatArray Build_FIR_coeffs(float, float, int*, int*);
};

#endif /* AGC_H_ */
