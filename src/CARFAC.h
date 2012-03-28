/*
 * CARFAC.h
 *
 *  Created on: 24 mar 2012
 *      Author: ulha
 */

#ifndef CARFAC_H_
#define CARFAC_H_

#include "CAR.h"
#include "IHC.h"
#include "AGC.h"

class CARFAC {
public:
	CARFAC(int, CAR_parameters, IHC_parameters, AGC_parameters, float, float);
	virtual ~CARFAC();

	CAR_coefficients CAR_coeffs;
	CAR_parameters CAR_params;

	IHC_coefficients IHC_coeffs;
	IHC_parameters IHC_params;

	AGC_coefficients AGC_coeffs;
	AGC_parameters AGC_params;
};

#endif /* CARFAC_H_ */
