#!/usr/bin/env python
# encoding: utf-8
#
# AIM-C: A C++ implementation of the Auditory Image Model
# http://www.acousticscale.org/AIMC
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""
ModuleGaussians_test.py

Created by Thomas Walters on 2010-02-15.
Copyright 2010 Thomas Walters <tom@acousticscale.org>
Test for the Gaussians module. Runs a number of pre-computed SAI profiles
through the module, and tests them against the saved output from the
MATLAB rubber_GMM code.
"""

import aimc
from scipy import io

def main():
  data_file = "src/Modules/Features/testdata/aa153.0p108.1s100.0t+000itd.mat"
  data = io.loadmat(data_file)
  
  # The margin of error allowed between the returned values from AIM-C and
  # the stored MATLAB values.
  epsilon = 0.00001;
  
  given_profiles = data["Templates"]
  matlab_features = data["feature"]
  
  (profile_count, channel_count) = given_profiles.shape
  
  profile_sig = aimc.SignalBank()
  profile_sig.Initialize(channel_count, 1, 44100)
  parameters = aimc.Parameters()
  mod_gauss = aimc.ModuleGaussians(parameters)
  mod_gauss.Initialize(profile_sig)
  
  correct_count = 0;
  incorrect_count  = 0;
  for p in range(0, profile_count):
    profile = given_profiles[p]
    features = matlab_features[p]
    for i in range(0, channel_count):
      profile_sig.set_sample(i, 0, profile[i])
    mod_gauss.Process(profile_sig)
    out_sig = mod_gauss.GetOutputBank()
    error = False;
    for j in range(0, out_sig.channel_count()):
      if (abs(out_sig.sample(j, 0) - features[j]) > epsilon):
        error = True;
        incorrect_count += 1;
      else:
        correct_count += 1;
    if error:
      print("Mismatch at profile %d" % (p))
      print("AIM-C values: %f %f %f %f" % (out_sig.sample(0, 0), out_sig.sample(1, 0), out_sig.sample(2, 0), out_sig.sample(3, 0)))
      print("MATLAB values: %f %f %f %f" % (features[0], features[1], features[2], features[3]))
      print("")
    percent_correct = 100 * correct_count / (correct_count + incorrect_count)
  print("Total correct: %f percent" % (percent_correct))
  if percent_correct == 100:
    print("=== TEST PASSED ===")
  else:
    print("=== TEST FAILED! ===")

  pass


if __name__ == '__main__':
  main()
