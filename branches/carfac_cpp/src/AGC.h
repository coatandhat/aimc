#ifndef AGC_H_
#define AGC_H_

#include "CARFAC_common_typedefs.h"

class AGC_parameters {
public:
	AGC_parameters(){
		n_stages = 4; // redundant?
		time_constants = {0.002*1, 0.002*4, 0.002*16, 0.002*64};
		AGC_stage_gain = 2;
		decimation = {8, 2, 2, 2};
		AGC1_scales = {1.0, 1.4,  2.0, 2.8};
		AGC2_scales = {1.6, 2.25, 3.2, 4.5};
		detect_scale = 0.25;
		AGC_mix_coeff = 0.5;
	}

	virtual ~AGC_parameters(){}

	int n_stages;
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
	AGC_coefficients(AGC_parameters*, float, int);
	virtual ~AGC_coefficients();
private:
	AGC_coefficients();
};

#endif /* AGC_H_ */
