#include "IHC.h"

IHC_parameters::IHC_parameters():
tau_lpf_(0.000080),
tau1_out_(0.010),
tau1_in_(0.20),
tau2_out_(0.0025),
tau2_in_(0.005)
{
// do nothing more
}

IHC_coefficients::IHC_coefficients(IHC_parameters* IHC_params_p, float fs, int n_ch):
saturation_output_(0.0)
{
  // TODO stuff!
}
