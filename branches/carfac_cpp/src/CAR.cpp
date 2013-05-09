#include "CAR.h"
#include "CARFAC_common.h"
#include "CARFAC.h"
#include <cmath>
#include "unit_conversion.h"

CAR_parameters::CAR_parameters():
	  velocity_scale_(0.2),
	  v_offset_(0.01),
	  v2_corner_(0.2),
	  v_damp_max_(0.01),
	  min_zeta_(0.10),
	  first_pole_theta_(0.085*kPi),
	  zero_ratio_(sqrt(2)),
	  high_f_damping_compression_(0.5),
	  erb_per_step_(0.5),
	  min_pole_hz_(30),
	  erb_break_freq_(kDefaultErbBreakFreq),
	  erb_q_(kDefaultErbQ)
{
    // do nothing more!
}

CAR_coefficients::CAR_coefficients(CAR_parameters* car_params_p, float fs,
                                   FloatArray pole_freqs)
{

  float f = pow(car_params_p->zero_ratio_, 2) + 1;

  // TODO: dirty FloatArray initialization.
  r1_coeffs_= pole_freqs;
  a0_coeffs_= pole_freqs;
  c0_coeffs_= pole_freqs;
  h_coeffs_= pole_freqs;
  g0_coeffs_= pole_freqs;
  zr_coeffs_= pole_freqs;

  FloatArray theta = pole_freqs;

  float min_zeta_mod;
  float x;
  float ff = car_params_p->high_f_damping_compression_;

  // TODO: temp loop until we get eigen in  (or we just leave it like this)
  for(float i=0; i<pole_freqs.size(); i++){
      theta[i] *= (2*kPi*fs); // scalar mult.
      c0_coeffs_[i] = sin(theta[i]);
      a0_coeffs_[i] = cos(theta[i]);

      x = theta[i]/kPi;
      zr_coeffs_[i] = kPi * (x - ff * x*x*x);

      min_zeta_mod = (car_params_p->min_zeta_ + 0.25 * (ERB_Hz(
                      pole_freqs[i], car_params_p->erb_break_freq_,
                      car_params_p->erb_q_) / pole_freqs[i]
                     - car_params_p->min_zeta_));

      r1_coeffs_[i] = 1-zr_coeffs_[i]*min_zeta_mod;

      h_coeffs_[i] = c0_coeffs_[i] * f;

      //TODO: g0_coeffs_ calculated here for now. Let's talk about this, need
      // to see the whole picture of what makes sense.
      // Related: I believe CAR, IHC and AGC classes would be beneficial!
      float tmp = 1 - 2*r1_coeffs_[i]*a0_coeffs_[i] + r1_coeffs_[i]*r1_coeffs_[i];
      g0_coeffs_[i] = tmp / ( tmp + h_coeffs_[i]*r1_coeffs_[i]*c0_coeffs_[i]);
  }

}