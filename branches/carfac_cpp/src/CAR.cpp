#include "CAR.h"
#include "CARFAC_common.h"
#include <cmath>

CAR_coefficients::CAR_coefficients(CAR_parameters* car_params, float fs,
                                   FloatArray pole_freqs){

  // initialisation of the FloatArray coefficients, if needed.

  float f = pow(car_params->zero_ratio_, 2) + 1;

  FloatArray theta = pole_freqs;
  FloatArray c0 = pole_freqs;
  FloatArray a0 = pole_freqs;

  // temp until we get eigen in
  for(float i=0; i<theta.size(); i++){
      theta[i] *= (2*kPi*fs); // scalar mult.
      c0[i] = sin(theta[i]);
      a0[i] = cos(theta[i]);
  }

  // etc

}

CAR_coefficients::~CAR_coefficients(){
  // TODO Auto-generated destructor stub
}
