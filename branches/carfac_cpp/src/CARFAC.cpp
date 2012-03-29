#include "CARFAC.h"
#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

#include "CARFAC_common_typedefs.h"

CARFAC::CARFAC(int fs = kDefaultFs,
              CAR_parameters* car_params = new CAR_parameters(),
              IHC_parameters* ihc_params = new IHC_parameters(),
              AGC_parameters* agc_params = new AGC_parameters(),
              float erb_break_freq = kDefaultErbBreakFreq,
              float erb_q = kDefaultErbQ){

  // for now, design is to take ownership. Preferences? Make copies, call by value, etc?
  car_params_ = car_params;
  ihc_params_ = ihc_params;
  agc_params_ = agc_params;

  FloatArray pole_freqs; //TODO: do it
  car_coeffs_ = new CAR_coefficients(car_params_, fs, pole_freqs);

  int n_ch = 10; // TODO: do the design loops
  agc_coeffs_ = new AGC_coefficients(agc_params_, fs, n_ch);

  ihc_coeffs_ = new IHC_coefficients(ihc_params_, fs, n_ch);
}

CARFAC::~CARFAC() {
  delete car_coeffs_;
  delete ihc_coeffs_;
  delete agc_coeffs_;

  //delete car_params_;
  //delete ihc_params_;
  //delete agc_params_;
}
