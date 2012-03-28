#ifndef CAR_H_
#define CAR_H_

#include "CARFAC_common_typedefs.h"
#include <math.h>

class CAR_parameters {
public:
	CAR_parameters(){
		velocity_scale = 0.2;
		v_offset = 0.01;
		v2_corner = 0.2;
		v_damp_max = 0.01;
		min_zeta = 0.10;
		first_pole_theta = 0.085*PI;
		zero_ratio = sqrt(2);
		high_f_damping_compression = 0.5;
		ERB_per_step = 0.5;
		min_pole_Hz = 30;
	}
	virtual ~CAR_parameters(){}

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
	CAR_coefficients(){}
};

#endif /* CAR_H_ */
