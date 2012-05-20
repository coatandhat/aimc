#include "AGC.h"
#include <cmath>
#include <stdlib.h>
#include <stdio.h>  

AGC_coefficients::AGC_coefficients(AGC_parameters* AGC_params_p,
                                   float fs, int n_ch){
  float decim = 1.0;
  float total_DC_gain = 0.0;
  float tau, ntimes, delay, spread_sq, u, p, dp;
  int n_taps = 0, n_iterations = 1;

  n_ch_ = n_ch;
  n_agc_stages_ = AGC_params_p->n_stages_;
  agc_stage_gain_ = AGC_params_p->agc_stage_gain_;
  
  // FloatArray initialization using assign method - dont know if this is good enough
  agc_epsilon_.assign(n_agc_stages_, 0.0); //the 1/(tau*fs) roughly
  agc_polez1_ = agc_epsilon_;
  agc_polez2_ = agc_epsilon_;
  agc_spatial_iterations_ = agc_epsilon_;
  agc_spatial_n_taps_ = agc_epsilon_;
  agc_mix_coeffs_ = agc_epsilon_;
  FloatArray agc1_scales = AGC_params_p->agc1_scales_;
  FloatArray agc2_scales = AGC_params_p->agc2_scales_;
  FloatArray agc_spatial_FIR;
  decimation_ = AGC_params_p->decimation_;
  
  for(int stage=0; stage < n_agc_stages_; stage++){
    tau = AGC_params_p->time_constants_[stage];
    decim *= AGC_params_p->decimation_[stage];
    agc_epsilon_[stage] = 1.0 - exp(-decim/(tau*fs));
    ntimes = tau * (fs/decim);
    delay = (agc2_scales[stage]-agc1_scales[stage])/ntimes;
    spread_sq = (agc1_scales[stage]*agc1_scales[stage] + agc2_scales[stage]*agc2_scales[stage])/ntimes;
    
    u = 1.0 + 1.0/spread_sq;
    p = u - sqrt(u*u-1);
    dp = delay*(1 - 2*p + p*p)*0.5;
    agc_polez1_[stage] = p - dp;
    agc_polez2_[stage] = p + dp;

    agc_spatial_FIR = Build_FIR_coeffs(spread_sq, delay, &n_iterations, &n_taps);

    agc_spatial_iterations_[stage] = (float) n_iterations;
    agc_spatial_n_taps_[stage] = (float) n_taps;
    agc_spatial_fir_.push_back(FloatArray());
    
    for(int i =0; i < 3; i++)
      agc_spatial_fir_[stage].push_back(agc_spatial_FIR[i]);
      
    total_DC_gain += pow(AGC_params_p->agc_stage_gain_,stage);
    
    if(stage == 0)
      agc_mix_coeffs_[stage] = 0.0;
    else
      agc_mix_coeffs_[stage] = AGC_params_p->agc_mix_coeff_/(tau * (fs/decim));
  }
  agc_gain_ = total_DC_gain;
  detect_scale_ = AGC_params_p->detect_scale_/total_DC_gain;
}
AGC_coefficients::~AGC_coefficients(){
  // TODO Auto-generated destructor stub
}

FloatArray AGC_coefficients::Build_FIR_coeffs(float var, float mn, int* ptr_iters, int* ptr_taps){
  float a, b;
  FloatArray FIR(3);

  // Try with 3 FIR taps
  a = (var + mn*mn - mn)/2;
  b = (var + mn*mn + mn)/2;
  FIR[0] = a;
  FIR[1] = 1.0 - a - b;
  FIR[2] = b;

  if(FIR[1] >= 0.2){
    *ptr_taps = 3;
    return FIR;
  }
  else //Try with 5 FIR taps
    {
      for(int n_iter = 1; n_iter<16; n_iter++){
        mn /= n_iter;
        var /= n_iter;

        a = ((var + mn*mn)*2/5 - mn*2/3)/2;
        b = ((var + mn*mn)*2/5 + mn*2/3)/2;
        FIR[0] = a/2;
        FIR[1] = 1.0 - a - b;
        FIR[2] = b;

        *ptr_iters = n_iter;

        if(FIR[1] >= 0.1){
            *ptr_taps = 5;
            return FIR;
        }
      }
      //TODO: discuss how we handle errors
      printf("Too many iterations in FIR_coeffs\n");
      FIR = {0, 0, 0};
      return FIR;
   }
}
