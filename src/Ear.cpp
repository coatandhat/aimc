/*
 * Ear.cpp
 *
 *  Created on: 16 maj 2012
 *      Author: ulha
 */

#include "Ear.h"
#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

Ear::Ear(CAR_parameters* car_params, IHC_parameters* ihc_params, AGC_parameters* agc_params, FloatArray pole_freqs, int n_ch, float fs){
  car_params_ = *car_params;
  ihc_params_ = *ihc_params;
  agc_params_ = *agc_params;

  car_coeffs_p_ = new CAR_coefficients(&car_params_, fs, pole_freqs);
  ihc_coeffs_p_ = new IHC_coefficients(&ihc_params_, fs, n_ch);
  agc_coeffs_p_ = new AGC_coefficients(&agc_params_, fs, n_ch);
}
