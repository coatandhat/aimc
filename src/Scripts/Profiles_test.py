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
Profiles_test.py

Created by Thomas Walters on 2010-02-22.
Copyright 2010 Thomas Walters <tom@acousticscale.org>
Test the AIM-C model from filterbank to SSI profiles
"""

import aimc
from scipy.io import wavfile
from scipy import io
import scipy
import pylab
from itertools import izip, chain, repeat

def grouper(n, iterable, padvalue=None):
    "grouper(3, 'abcdefg', 'x') --> ('a','b','c'), ('d','e','f'), ('g','x','x')"
    return izip(*[chain(iterable, repeat(padvalue, n-1))]*n)

def main():
  wave_path = "/Users/Tom/Documents/Work/PhD/HTK-AIM/Sounds/"
  features_path = "/Users/Tom/Documents/Work/PhD/HTK-AIM/work08-jess-original-rec_rubber/features/"
  
  file_name = "aa/aa161.1p119.4s100.0t+000itd"
  
  wave_suffix = ".wav"
  features_suffix = ".mat"
  
  frame_period_ms = 10;
    
  wave_filename = wave_path + file_name + wave_suffix
  features_filename = features_path + file_name + features_suffix
  
  (sample_rate, input_wave) = wavfile.read(wave_filename)
  wave_length = input_wave.size
  buffer_length = int(frame_period_ms * sample_rate / 1000)
 
  #pylab.plot(input_wave)
  #pylab.show()
  
  input_sig = aimc.SignalBank()
  input_sig.Initialize(1, buffer_length, sample_rate)
  parameters = aimc.Parameters()
  parameters.Load("src/Scripts/profile_features.cfg")
  mod_gt = aimc.ModuleGammatone(parameters)
  mod_hl = aimc.ModuleHCL(parameters)
  mod_profile = aimc.ModuleSlice(parameters)
  mod_scaler = aimc.ModuleScaler(parameters)
  mod_gt.AddTarget(mod_hl)
  mod_hl.AddTarget(mod_profile)
  mod_profile.AddTarget(mod_scaler)
  mod_gt.Initialize(input_sig)
  
  correct_count = 0;
  incorrect_count  = 0;
  
  scaled_wave = []
  for sample in input_wave:
    scaled_wave.append(float(sample / float(pow(2,15) - 1)))
  i = 0
  
  wave_chunks = grouper(buffer_length, scaled_wave, 0)

  out_frames = []
  for chunk in wave_chunks:
    i = 0
    for sample in chunk:
      input_sig.set_sample(0, i, float(sample))
      i += 1
    mod_gt.Process(input_sig)
    out_sig = mod_scaler.GetOutputBank()
    
    channel_count = out_sig.channel_count()
    out_buffer_length = out_sig.buffer_length()
    cfs = scipy.zeros((channel_count))
    out = scipy.zeros((channel_count, out_buffer_length))
  
    for ch in range(0, channel_count):
      for i in range(0, out_buffer_length):  
        out[ch, i] = out_sig.sample(ch, i)
    out_frames.append(out)
    
  outmat = dict(profile_out=out_frames)
  io.savemat("src/Scripts/profile_out.mat", outmat)

  pass


if __name__ == '__main__':
  main()
