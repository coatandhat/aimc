/*
 * CARFAC.cpp
 *
 *  Created on: 24 mar 2012
 *      Author: ulha
 */

#include "CARFAC.h"
#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

#include "CARFAC_common_typedefs.h"

CARFAC::CARFAC(int fs = DEFAULT_FS,
		CAR_parameters _CAR_params = CAR_parameters(),
		IHC_parameters _IHC_params = IHC_parameters(),
		AGC_parameters _AGC_params = AGC_parameters(),
		float ERB_break_freq = DEFAULT_ERB_break_freq,
		float ERB_Q = DEFAULT_ERB_Q){

	CAR_params = _CAR_params;
	IHC_params = _IHC_params;
	AGC_params = _AGC_params;

	FloatArray pole_freqs; //TODO: do it, or push it down to CAR_coefficients level instead
	CAR_coeffs = CAR_coefficients(CAR_params, fs, pole_freqs);

	// etc ...
}

CARFAC::~CARFAC() {
	// TODO Auto-generated destructor stub
}
