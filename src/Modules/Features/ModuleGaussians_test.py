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
through the module, and tests them against the equivalent output from the
MATLAB rubber_GMM code.
"""

import aimc
import matplotlib
import pylab
import scipy

def main():
  data_file = "src/Modules/Features/testdata/aa153.0p108.1s100.0t+000itd.mat"
  data = scipy.io.loadmat(data_file)
  
  given_profiles = data["Templates"]
  matlab_features = data["feature"]
  
  
  
  pass


if __name__ == '__main__':
  main()
