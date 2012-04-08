#include "AGC.h"
#include <cmath>
#include <stdlib.h>
#include <stdio.h>  

AGC_coefficients::AGC_coefficients(AGC_parameters* AGC_params,
                                   float fs, int n_ch){
  float decim = 1.0;
  float total_DC_gain = 0.0;
  float tau, ntimes, delay, spread_sq, u, p, dp;
  int n_taps = 0, n_iterations = 1;
  bool FIR_OK = false;
  
  n_ch_ = n_ch;
  n_agc_stages_ = AGC_params->n_stages_;
  agc_stage_gain_ = AGC_params->agc_stage_gain_;
  
  // FloatArray initialization using assign method - dont know if this is good enough
  agc_epsilon_.assign(n_agc_stages_, 0.0); //the 1/(tau*fs) roughly
  agc_polez1_ = agc_epsilon_;
  agc_polez2_ = agc_epsilon_;
  agc_spatial_iterations_ = agc_epsilon_;
  agc_spatial_n_taps_ = agc_epsilon_;
  agc_mix_coeffs_ = agc_epsilon_;
  FloatArray agc1_scales = AGC_params->agc1_scales_;
  FloatArray agc2_scales = AGC_params->agc2_scales_;
  FloatArray agc_spatial_FIR;
  decimation_ = AGC_params->decimation_;
  
  for(int stage=0; stage < n_agc_stages_; stage++){
    tau = AGC_params->time_constants_[stage];
    decim *= AGC_params->decimation_[stage];
    agc_epsilon_[stage] = 1.0 - exp(-decim/(tau*fs));
    ntimes = tau * (fs/decim);
    delay = (agc2_scales[stage]-agc1_scales[stage])/ntimes;
    spread_sq = (agc1_scales[stage]*agc1_scales[stage] + agc2_scales[stage]*agc2_scales[stage])/ntimes;
    
    u = 1.0 + 1.0/spread_sq;
    p = u - sqrt(u*u-1);
    dp = delay*(1 - 2*p + p*p)*0.5;
    agc_polez1_[stage] = p - dp;
    agc_polez2_[stage] = p + dp;
    
    while(!FIR_OK){
      switch(n_taps){
        case 0:
          n_taps = 3;
          break;
        case 3:
          n_taps = 5;
          break;
        case 5:
          n_iterations++;
          if(n_iterations > 16){
            printf("Too many n_iterations in CARFAC_DesignAGC\n");
            exit(1);
          }
          break;
        default:
          printf("Bad n_taps in CARFAC_DesignAGC\n");
          exit(1);
      }
      agc_spatial_FIR = FIR_coeffs(n_taps, spread_sq, delay, n_iterations, &FIR_OK);
    }
    agc_spatial_iterations_[stage] = (float) n_iterations;
    agc_spatial_n_taps_[stage] = (float) n_taps;
    agc_spatial_fir_.push_back(FloatArray());
    
    for(int i =0; i < 3; i++)
      agc_spatial_fir_[stage].push_back(agc_spatial_FIR[i]);
      
    total_DC_gain += pow(AGC_params->agc_stage_gain_,stage);
    
    if(stage == 0)
      agc_mix_coeffs_[stage] = 0.0;
    else
      agc_mix_coeffs_[stage] = AGC_params->agc_mix_coeff_/(tau * (fs/decim));
  }
  agc_gain_ = total_DC_gain;
  
  // TODO Computation of the detect_scale_ member
}
AGC_coefficients::~AGC_coefficients(){
  // TODO Auto-generated destructor stub
}

FloatArray  AGC_coefficients::FIR_coeffs(int n_taps, float var, float mn, int n_iter, bool* ptr_FIR_OK)
{
  float a, b;
  FloatArray FIR(3);
  mn /= n_iter;
  var /= n_iter;
  
  switch(n_taps){
    case 3:
      a = (var + mn*mn - mn)/2;
      b = (var + mn*mn + mn)/2;
      FIR[0] = a;
      FIR[1] = 1.0 - a - b;
      FIR[2] = b;
      if(FIR[1] >= 0.2)
        *ptr_FIR_OK = true;
      break;
    case 5:
      a = ((var + mn*mn)*2/5 - mn*2/3)/2;
      b = ((var + mn*mn)*2/5 + mn*2/3)/2;
      FIR[0] = a/2;
      FIR[1] = 1.0 - a - b;
      FIR[2] = b;
      if(FIR[1] >= 0.1)
        *ptr_FIR_OK = true;
      break;
    default:
      printf("Bad n_taps in AGC_spatial_FIR\n");
      exit(1);
  }
  
  return FIR;
}