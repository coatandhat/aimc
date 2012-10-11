#include "CARFAC.h"
#include "CARFAC_common.h"
#include "gtest/gtest.h"

/*
 * CARFACDesignTest - Test suite for constructor related tests.
 */
TEST(CARFACDesignTest, SampleRate) {
  CAR_parameters car_params = CAR_parameters();
  IHC_parameters ihc_params = IHC_parameters();
  AGC_parameters agc_params = AGC_parameters();

  int arbitrary_number = 1347;

  CARFAC fooCarfac = CARFAC(arbitrary_number, &car_params, &ihc_params, &agc_params, 1);
  EXPECT_EQ(fooCarfac.fs_, arbitrary_number);
}

TEST(CARFACDesignTest, NumberOfEars) {
  CAR_parameters car_params = CAR_parameters();
  IHC_parameters ihc_params = IHC_parameters();
  AGC_parameters agc_params = AGC_parameters();

  CARFAC fooCarfac = CARFAC(kDefaultFs, &car_params, &ihc_params, &agc_params, 2);
  EXPECT_EQ(fooCarfac.n_ears_, fooCarfac.ears_.size());
}

TEST(CARFACDesignTest, MaxChannelsPerOctave) {
  CAR_parameters car_params = CAR_parameters();
  IHC_parameters ihc_params = IHC_parameters();
  AGC_parameters agc_params = AGC_parameters();

  car_params.erb_per_step_ = 2*0.5; //not sure if this is the best way to design this test... I'm just doing _something_ here.
  float expected_max_channels_per_octave = 10.56/2;
  float tolerance = 0.25;

  CARFAC fooCarfac = CARFAC(kDefaultFs, &car_params, &ihc_params, &agc_params, 1);
  ASSERT_NEAR(fooCarfac.max_channels_per_octave_, expected_max_channels_per_octave, tolerance);
}
