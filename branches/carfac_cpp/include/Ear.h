/*
 * Ear.h
 *
 *  Created on: 16 maj 2012
 *      Author: ulha
 */

#ifndef EAR_H_
#define EAR_H_

#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

class Ear {
public:
  Ear(CAR_parameters*, IHC_parameters*, AGC_parameters*, FloatArray, int, float);

  CAR_parameters car_params_;
  IHC_parameters ihc_params_;
  AGC_parameters agc_params_;

  CAR_coefficients* car_coeffs_p_;
  IHC_coefficients* ihc_coeffs_p_;
  AGC_coefficients* agc_coeffs_p_;

  virtual
  ~Ear();

private:
  Ear();

};

#endif /* EAR_H_ */
