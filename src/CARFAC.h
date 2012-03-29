#ifndef CARFAC_H_
#define CARFAC_H_

#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

class CARFAC{
public:
  CARFAC(int, CAR_parameters*, IHC_parameters*, AGC_parameters*, float, float);
  virtual ~CARFAC();

  CAR_coefficients* car_coeffs_;
  CAR_parameters* car_params_;

  IHC_coefficients* ihc_coeffs_;
  IHC_parameters* ihc_params_;

  AGC_coefficients* agc_coeffs_;
  AGC_parameters* agc_params_;
};

#endif /* CARFAC_H_ */
