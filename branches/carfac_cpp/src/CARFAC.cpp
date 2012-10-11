#include <cmath>

#include "CARFAC.h"
#include "CAR.h"
#include "IHC.h"
#include "AGC.h"
#include "Ear.h"
#include "unit_conversion.h"
#include "CARFAC_common.h"

CARFAC::CARFAC(int fs,
              CAR_parameters* car_params,
              IHC_parameters* ihc_params,
              AGC_parameters* agc_params,
              int n_ears){

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
  Ear ear = Ear(car_params, ihc_params, agc_params, pole_freqs_, n_ch_, fs);
  ears_.assign(n_ears_, ear);
}
