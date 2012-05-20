#ifndef CARFAC_H_
#define CARFAC_H_

#include "CAR.h"
#include "IHC.h"
#include "AGC.h"
#include "Ear.h"
#include <vector>

const double kDefaultFs = 22050;

class CARFAC{
public:
  CARFAC(int, CAR_parameters*, IHC_parameters*, AGC_parameters*, int);
  virtual ~CARFAC();
  static float ERB_Hz(float, float, float);

  float fs_;

  int n_ch_; // these three, push down to ear level?
  float max_channels_per_octave_;
  FloatArray pole_freqs_;

  int n_ears_;
  std::vector<Ear> ears_;

private:
  CARFAC(){};

};

#endif /* CARFAC_H_ */
