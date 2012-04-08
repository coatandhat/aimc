#ifndef AGC_H_
#define AGC_H_

#include "CARFAC_common.h"

class AGC_parameters {
public:
  AGC_parameters():
    n_stages_(4),
    time_constants_({0.002*1, 0.002*4, 0.002*16, 0.002*64}),
    agc_stage_gain_(2),
    decimation_({8, 2, 2, 2}),
    agc1_scales_({1.0, 1.4,  2.0, 2.8}),
    agc2_scales_({1.6, 2.25, 3.2, 4.5}),
    detect_scale_(0.25),
    agc_mix_coeff_(0.5){
    // do nothing more
  }

  virtual ~AGC_parameters(){
    // do nothing
  }

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
  virtual ~AGC_coefficients();

  int n_ch_;
  int n_agc_stages_;
  float agc_stage_gain_;
  FloatArray agc_epsilon_;
  FloatArray decimation_;
  FloatArray agc_polez1_;
  FloatArray agc_polez2_;
  FloatArray agc_spatial_iterations_;
  std::vector<FloatArray> agc_spatial_fir_;
  FloatArray agc_spatial_n_taps_;
  FloatArray agc_mix_coeffs_;
  float agc_gain_;
  float detect_scale_;
  
private:
  AGC_coefficients();
  FloatArray FIR_coeffs(int, float, float, int, bool*);
};

#endif /* AGC_H_ */
