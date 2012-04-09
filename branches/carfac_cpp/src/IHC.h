#ifndef IHC_H_
#define IHC_H_

// not sure how to best deal with the "three style" IHC - ulha
class IHC_parameters{
public:
  IHC_parameters() :
    tau_lpf_(0.000080),
    tau1_out_(0.010),
    tau1_in_(0.20),
    tau2_out_(0.0025),
    tau2_in_(0.005){
    // do nothing more
  }

  virtual ~IHC_parameters(){
    // do nothing
  }

  float tau_lpf_;
  float tau1_out_;
  float tau1_in_;
  float tau2_out_;
  float tau2_in_;
};

class IHC_coefficients{
public:
  IHC_coefficients(IHC_parameters*, float, int);
  virtual ~IHC_coefficients();

  float saturation_output_;

private:
  IHC_coefficients();
};

#endif /* IHC_H_ */
