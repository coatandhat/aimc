#include "CARFAC.h"
#include "CARFAC_common.h"
#include "gtest/gtest.h"

/*
 * CARFACDesignTest - Test suite for constructor related tests.
 */
TEST(CARFACDesignTest, SampleRate) {
  CAR_parameters * car_params = new CAR_parameters();;
  IHC_parameters * ihc_params = new IHC_parameters();
  AGC_parameters * agc_params = new AGC_parameters();

  int arbitrary_number = 1347;

  CARFAC* fooCarfac = new CARFAC(arbitrary_number, car_params, ihc_params, agc_params, 1);
  EXPECT_EQ(fooCarfac->fs_, arbitrary_number);
}

TEST(CARFACDesignTest, NumberOfEars) {
  CAR_parameters * car_params = new CAR_parameters();;
  IHC_parameters * ihc_params = new IHC_parameters();
  AGC_parameters * agc_params = new AGC_parameters();

  int arbitrary_number = 1347;

  CARFAC* fooCarfac = new CARFAC(arbitrary_number, car_params, ihc_params, agc_params, 2);
  EXPECT_EQ(fooCarfac->n_ears_, fooCarfac->ears_.size());
}
