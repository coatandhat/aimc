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
import numpy
from itertools import izip, chain, repeat

def grouper(n, iterable, padvalue=None):
    "grouper(3, 'abcdefg', 'x') --> ('a','b','c'), ('d','e','f'), ('g','x','x')"
    return izip(*[chain(iterable, repeat(padvalue, n-1))]*n)
    
def BankToArray(out_bank):
  channel_count = out_bank.channel_count()
  out_buffer_length = out_bank.buffer_length()
  out = scipy.zeros((channel_count,out_buffer_length))
  for ch in range(0, channel_count):
    for i in range(0, out_buffer_length):  
      out[ch, i] = out_bank.sample(ch, i)
  return out

def StrobesToList(bank):
  channel_count = bank.channel_count()
  out = scipy.zeros((channel_count,), dtype=numpy.object)
  for ch in range(0, channel_count):
    s = []
    for i in range(0, bank.strobe_count(ch)):
      s.append(bank.strobe(ch, i))
    out[ch] = s
  return out

def main():
  wave_path = "/Users/Tom/Documents/Work/PhD/HTK-AIM/Sounds/"
  
  file_name = "ii/ii172.5p112.5s100.0t+000itd"
  
  wave_suffix = ".wav"
  
  frame_period_ms = 10;
    
  wave_filename = wave_path + file_name + wave_suffix
  
  (sample_rate, input_wave) = wavfile.read(wave_filename)
  wave_length = input_wave.size
  buffer_length = int(frame_period_ms * sample_rate / 1000)
 
  
  input_sig = aimc.SignalBank()
  input_sig.Initialize(1, buffer_length, sample_rate)
  parameters = aimc.Parameters()
  parameters.SetInt("input.buffersize", 480)

  mod_gt = aimc.ModuleGammatone(parameters)
  mod_hl = aimc.ModuleHCL(parameters)
  mod_strobes = aimc.ModuleLocalMax(parameters) 

  mod_gt.AddTarget(mod_hl)
  mod_hl.AddTarget(mod_strobes)

  mod_gt.Initialize(input_sig)
  
  correct_count = 0;
  incorrect_count  = 0;
  
  scaled_wave = []
  for sample in input_wave:
    scaled_wave.append(float(sample / float(pow(2,15) - 1)))
  i = 0
  
  wave_chunks = grouper(buffer_length, scaled_wave, 0)

  out_nap = []
  out_strobes = []

  for chunk in wave_chunks:
    i = 0
    for sample in chunk:
      input_sig.set_sample(0, i, float(sample))
      i += 1
    mod_gt.Process(input_sig) 
    out_nap.append(BankToArray(mod_hl.GetOutputBank()))
    out_strobes.append(StrobesToList(mod_strobes.GetOutputBank()))

  outmat = dict(nap=out_nap, strobes=out_strobes)
  io.savemat("src/Scripts/strobes_out.mat", outmat, oned_as='column')

  pass


if __name__ == '__main__':
  main()
