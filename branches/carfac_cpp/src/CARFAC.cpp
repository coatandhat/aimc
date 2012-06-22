#include <cmath>

#include "CARFAC.h"
#include "CAR.h"
#include "IHC.h"
#include "AGC.h"
#include "Ear.h"

#include "CARFAC_common.h"

CARFAC::CARFAC(int fs = kDefaultFs,
              CAR_parameters* car_params = new CAR_parameters(),
              IHC_parameters* ihc_params = new IHC_parameters(),
              AGC_parameters* agc_params = new AGC_parameters(),
              int n_ears = 1){

  fs_ = fs;

  float pole_hz = car_params->first_pole_theta_* fs / (2*kPi);
  while (pole_hz > car_params->min_pole_hz_){
      pole_freqs_.push_back(pole_hz); // TODO: STL specific
      pole_hz = pole_hz - car_params->erb_per_step_ *
                ERB_Hz(pole_hz, car_params->erb_break_freq_,
                    car_params->erb_q_);
  }
  n_ch_ = pole_freqs_.size();
  max_channels_per_octave_ = log(2) / log(pole_freqs_[0]/pole_freqs_[1]);

  n_ears_ = n_ears;
  Ear* ear = new Ear(car_params, ihc_params, agc_params, pole_freqs_, n_ch_, fs);
  ears_.assign(n_ears_, *ear);
}

float CARFAC::ERB_Hz(float cf_hz, float erb_break_freq, float erb_q){
  return (erb_break_freq + cf_hz) / erb_q;
}

CARFAC::~CARFAC() {
  //TODO: clean up
}