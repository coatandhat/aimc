#include "CARFAC.h"
#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

#include "CARFAC_common_typedefs.h"

CARFAC::CARFAC(int fs = DEFAULT_FS,
		CAR_parameters* _CAR_params = new CAR_parameters(),
		IHC_parameters* _IHC_params = new IHC_parameters(),
		AGC_parameters* _AGC_params = new AGC_parameters(),
		float ERB_break_freq = DEFAULT_ERB_break_freq,
		float ERB_Q = DEFAULT_ERB_Q){

	// for now, design is to take ownership. Preferences? Make copies, call by value, etc?
	CAR_params = _CAR_params;
	IHC_params = _IHC_params;
	AGC_params = _AGC_params;

	FloatArray pole_freqs; //TODO: do it
	CAR_coeffs = new CAR_coefficients(CAR_params, fs, pole_freqs);

	int n_ch = 10; // TODO: do the design loops
	AGC_coeffs = new AGC_coefficients(AGC_params, fs, n_ch);

	IHC_coeffs = new IHC_coefficients(IHC_params, fs, n_ch);
}

CARFAC::~CARFAC() {
	delete CAR_coeffs;
	//delete CAR_params;

	delete IHC_coeffs;
	//delete IHC_params;

	delete AGC_coeffs;
	//delete AGC_params;
}
