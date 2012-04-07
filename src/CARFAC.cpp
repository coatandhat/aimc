#include <cmath>

#include "CARFAC.h"
#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

#include "CARFAC_common.h"

CARFAC::CARFAC(int fs = kDefaultFs,
              CAR_parameters* car_params = new CAR_parameters(),
              IHC_parameters* ihc_params = new IHC_parameters(),
              AGC_parameters* agc_params = new AGC_parameters(),
              float erb_break_freq = kDefaultErbBreakFreq,
              float erb_q = kDefaultErbQ) : n_ears_(0) {

  // Design is to take ownership. Preferences? Make copies, call by value, etc?
  car_params_ = car_params;
  ihc_params_ = ihc_params;
  agc_params_ = agc_params;

  float pole_hz = car_params->first_pole_theta_* fs / (2*kPi);
  while (pole_hz > car_params->min_pole_hz_){
      pole_freqs_.push_back(pole_hz); // TODO: STL specific
      pole_hz = pole_hz - car_params->erb_per_step_ *
                ERB_Hz(pole_hz, erb_break_freq, erb_q);
  }
  n_ch_ = pole_freqs_.size();

  max_channels_per_octave_ = log(2) / log(pole_freqs_[0]/pole_freqs_[1]);

  //TODO: pass const references?
  car_coeffs_ = new CAR_coefficients(car_params_, fs_, pole_freqs_);
  ihc_coeffs_ = new IHC_coefficients(ihc_params_, fs_, n_ch_);
  agc_coeffs_ = new AGC_coefficients(agc_params_, fs_, n_ch_);

  //TODO: move this into AGC_coefficients constructor instead? This style
  // makes me a bit wary.
  agc_coeffs_->detect_scale_ = agc_params_->detect_scale_ /
                               (ihc_coeffs_->saturation_output_ *
                               agc_coeffs_->agc_gain_);


}

//TODO: move this somewhere else?
float CARFAC::ERB_Hz(float cf_hz){
  return ERB_Hz(cf_hz, 1000/4.37, 1000/(24.7*4.37));
} // TODO: is it really intentional to use this default value thing in matlab code?
float CARFAC::ERB_Hz(float cf_hz, float erb_break_freq, float erb_q){
  return (erb_break_freq + cf_hz) / erb_q;
}

CARFAC::~CARFAC() {
  delete car_coeffs_;
  delete ihc_coeffs_;
  delete agc_coeffs_;

  //TODO: as the current design takes ownership OR creates news params,
  //deletion is a ambiguos. Revise this design!

  //delete car_params_;
  //delete ihc_params_;
  //delete agc_params_;
}
