#ifndef AGC_H_
#define AGC_H_

#include "CARFAC_common_typedefs.h"

class AGC_parameters {
public:
	AGC_parameters();
	virtual ~AGC_parameters();

	float n_stages;
	FloatArray time_constants;
	float AGC_stage_gain;
	FloatArray decimation;
	FloatArray AGC1_scales;
	FloatArray AGC2_scales;
	float detect_scale;
	float AGC_mix_coeff;

};

class AGC_coefficients {
public:
	AGC_coefficients();
	virtual ~AGC_coefficients();
};

#endif /* AGC_H_ */
