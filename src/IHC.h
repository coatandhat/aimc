#ifndef IHC_H_
#define IHC_H_

// not sure how to best deal with the "three style" IHC - ulha
class IHC_parameters {
public:
	IHC_parameters()
	{
		tau_lpf = 0.000080;
		tau1_out = 0.020;
		tau1_in = 0.020;
		tau2_out = 0.005;
		tau2_in = 0.005;
	}
	virtual ~IHC_parameters(){}

	float tau_lpf;
	float tau1_out;
	float tau1_in;
	float tau2_out;
	float tau2_in;
};

class IHC_coefficients {
public:
	IHC_coefficients();
	virtual ~IHC_coefficients();
};

#endif /* IHC_H_ */
