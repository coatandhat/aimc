#ifndef CARFAC_H_
#define CARFAC_H_

#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

const double kDefaultFs = 22050;

class CARFAC{
public:
  CARFAC(int, CAR_parameters*, IHC_parameters*, AGC_parameters*);
  virtual ~CARFAC();
  static float ERB_Hz(float, float, float);

  float fs_;
  float max_channels_per_octave_;

  CAR_coefficients* car_coeffs_;
  CAR_parameters* car_params_;

  IHC_coefficients* ihc_coeffs_;
  IHC_parameters* ihc_params_;

  int n_ch_;
  FloatArray pole_freqs_;

  AGC_coefficients* agc_coeffs_;
  AGC_parameters* agc_params_;

  int n_ears_;
};

#endif /* CARFAC_H_ */
