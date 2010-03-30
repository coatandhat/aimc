#!/usr/bin/env python
# encoding: utf-8
#
# AIM-C: A C++ implementation of the Auditory Image Model
# http://www.acousticscale.org/AIMC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""
ModuleGammatone_test.py

Created by Thomas Walters on 2010-02-15.
Copyright 2010 Thomas Walters <tom@acousticscale.org>
Test for the Slaney IIR gammatone.
"""

import aimc
from scipy import io
import wave
import scipy

def main():
  data_file = "src/Modules/BMM/testdata/gammatone.mat"
  data = io.loadmat(data_file)
  
  # The margin of error allowed between the returned values from AIM-C and
  # the stored MATLAB values.
  epsilon = 0.000001;
  
  input_wave = data["input_wave"]
  sample_rate = data["sample_rate"]
  centre_frequencies = data["centre_frequencies"]
  expected_output = data["expected_output"]
  
  (channel_count, frame_count) = expected_output.shape
  buffer_length = 20000;
  
  input_sig = aimc.SignalBank()
  input_sig.Initialize(1, buffer_length, 48000)
  parameters = aimc.Parameters()
  parameters.Load("src/Modules/BMM/testdata/gammatone.cfg")
  mod_gt = aimc.ModuleGammatone(parameters)
  mod_gt.Initialize(input_sig)
  
  correct_count = 0;
  incorrect_count  = 0;
  
  out = scipy.zeros((channel_count, buffer_length))
  
  cfs = scipy.zeros((channel_count))

  for i in range(0, buffer_length):
    input_sig.set_sample(0, i, input_wave[i][0])
  mod_gt.Process(input_sig)
  out_sig = mod_gt.GetOutputBank()
  for ch in range(0, out_sig.channel_count()):
    cfs[ch] = out_sig.centre_frequency(ch);
    for i in range(0, buffer_length):  
      out[ch, i] = out_sig.sample(ch, i)
  
  outmat = dict(filterbank_out=out, centre_frequencies_out=cfs)
  io.savemat("src/Modules/BMM/testdata/out_v2.mat", outmat)

  pass


if __name__ == '__main__':
  main()
