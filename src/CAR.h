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

	float velocity_scale;
	float v_offset;
	float v2_corner;
	float v_damp_max;
	float min_zeta;
	float first_pole_theta;
	float zero_ratio;
	float high_f_damping_compression;
	float ERB_per_step;
	float min_pole_Hz;

};

class CAR_coefficients{
public:
	CAR_coefficients(CAR_parameters*, float, FloatArray);
	virtual ~CAR_coefficients();
private:
	CAR_coefficients(){};
};

#endif /* CAR_H_ */
