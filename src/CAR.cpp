#include "CAR.h"
#include "CARFAC_common.h"
#include "CARFAC.h"
#include <cmath>

CAR_coefficients::CAR_coefficients(CAR_parameters* car_params, float fs,
                                   FloatArray pole_freqs){

  float f = pow(car_params->zero_ratio_, 2) + 1;

  // dirty FloatArray initialisation. Redo this later
  r1_coeffs_= pole_freqs;
  a0_coeffs_= pole_freqs;
  c0_coeffs_= pole_freqs;
  h_coeffs_= pole_freqs;
  g0_coeffs_= pole_freqs;
  zr_coeffs_= pole_freqs;

  FloatArray theta = pole_freqs;

  float min_zeta_mod;
  float x;
  float ff = car_params->high_f_damping_compression_;

  // temp loop until we get eigen in
  for(float i=0; i<theta.size(); i++){
      theta[i] *= (2*kPi*fs); // scalar mult.
      c0_coeffs_[i] = sin(theta[i]);
      a0_coeffs_[i] = cos(theta[i]);

      x = theta[i]/kPi;
      zr_coeffs_[i] = kPi * (x - ff * x*x*x);

      min_zeta_mod = (car_params->min_zeta_ + 0.25 *
          (CARFAC::ERB_Hz(pole_freqs[i])/pole_freqs[i]-car_params->min_zeta_));

      r1_coeffs_[i] = 1-zr_coeffs_[i]*min_zeta_mod;

      h_coeffs_[i] = c0_coeffs_[i] * f;
  }

  // g0_coeffs_[i] = ... TODO: restructure this!

}

CAR_coefficients::~CAR_coefficients(){
  // TODO Auto-generated destructor stub
}
