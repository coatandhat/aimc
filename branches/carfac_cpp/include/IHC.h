#ifndef IHC_H_
#define IHC_H_

// not sure how to best deal with the "three style" IHC - ulha
class IHC_parameters{
public:
  IHC_parameters();

  float tau_lpf_;
  float tau1_out_;
  float tau1_in_;
  float tau2_out_;
  float tau2_in_;
};

class IHC_coefficients{
public:
  IHC_coefficients(IHC_parameters* IHC_params_p, float fs, int n_ch);

  float saturation_output_;

private:

};

#endif /* IHC_H_ */
