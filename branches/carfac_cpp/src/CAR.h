/*
 * CAR.h
 *
 *  Created on: 24 mar 2012
 *      Author: ulha
 */

#ifndef CAR_H_
#define CAR_H_

#include "CARFAC_common_typedefs.h"

class CAR_parameters {
public:
	CAR_parameters();
	virtual ~CAR_parameters();
};

class CAR_coefficients{
public:
	CAR_coefficients(){}
	CAR_coefficients(CAR_parameters, float, FloatArray);
	virtual ~CAR_coefficients();
};

#endif /* CAR_H_ */
