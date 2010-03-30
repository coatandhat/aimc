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
    
def BankToArray(out_bank):
  channel_count = out_bank.channel_count()
  out_buffer_length = out_bank.buffer_length()
  out = scipy.zeros((channel_count, out_buffer_length))
  for ch in range(0, channel_count):
    for i in range(0, out_buffer_length):  
      out[ch, i] = out_bank.sample(ch, i)
  return out

def StrobesToList(bank):
  channel_count = bank.channel_count()
  strobes = []
  for ch in range(0, channel_count):
    s = []
    for i in range(0, bank.strobe_count(ch)):
      s.append(bank.strobe(ch, i))
    strobes.append(s)

def main():
  wave_path = "/Users/Tom/Documents/Work/PhD/HTK-AIM/Sounds/"
  #features_path = "/Users/Tom/Documents/Work/PhD/HTK-AIM/work08-jess-original-rec_rubber/features/"
  
  file_name = "ii/ii172.5p112.5s100.0t+000itd"
  
  wave_suffix = ".wav"
  features_suffix = ".mat"
  
  frame_period_ms = 10;
    
  wave_filename = wave_path + file_name + wave_suffix
  #features_filename = features_path + file_name + features_suffix
  
  (sample_rate, input_wave) = wavfile.read(wave_filename)
  wave_length = input_wave.size
  buffer_length = int(frame_period_ms * sample_rate / 1000)
 
  #pylab.plot(input_wave)
  #pylab.show()
  
  input_sig = aimc.SignalBank()
  input_sig.Initialize(1, buffer_length, sample_rate)
  parameters = aimc.Parameters()
  parameters.SetFloat("sai.frame_period_ms", 10.0)
  parameters.SetInt("input.buffersize", 480)

  mod_gt = aimc.ModuleGammatone(parameters)
  mod_hl = aimc.ModuleHCL(parameters)
  mod_strobes = aimc.ModuleLocalMax(parameters) 
  mod_sai = aimc.ModuleSAI(parameters)
  parameters.SetBool("ssi.pitch_cutoff", True)
  parameters.SetBool("ssi.weight_by_cutoff", False)
  parameters.SetBool("ssi.weight_by_scaling", True)
  parameters.SetBool("ssi.log_cycles_axis", True)
  mod_ssi = aimc.ModuleSSI(parameters) 
  
  parameters.SetFloat("nap.lowpass_cutoff", 100.0)
  mod_nap_smooth = aimc.ModuleHCL(parameters)
  mod_scaler = aimc.ModuleScaler(parameters)

  parameters.SetBool("slice.all", False)
  parameters.SetInt("slice.lower_index", 77)
  parameters.SetInt("slice.upper_index", 150)
  slice_1 = aimc.ModuleSlice(parameters)

  parameters.SetInt("slice.lower_index", 210)
  parameters.SetInt("slice.upper_index", 240)
  slice_2 = aimc.ModuleSlice(parameters)

  parameters.SetInt("slice.lower_index", 280)
  parameters.SetInt("slice.upper_index", 304)
  slice_3 = aimc.ModuleSlice(parameters)

  parameters.SetInt("slice.lower_index", 328)
  parameters.SetInt("slice.upper_index", 352)
  slice_4 = aimc.ModuleSlice(parameters)

  parameters.SetBool("slice.all", True)
  slice_5 = aimc.ModuleSlice(parameters)
  
  nap_profile = aimc.ModuleSlice(parameters)

  features_1 = aimc.ModuleGaussians(parameters)
  features_2 = aimc.ModuleGaussians(parameters)
  features_3 = aimc.ModuleGaussians(parameters)
  features_4 = aimc.ModuleGaussians(parameters)
  features_5 = aimc.ModuleGaussians(parameters)

  mod_gt.AddTarget(mod_hl)
  mod_gt.AddTarget(mod_nap_smooth)
  mod_nap_smooth.AddTarget(nap_profile)
  nap_profile.AddTarget(mod_scaler)
  mod_hl.AddTarget(mod_strobes)
  mod_strobes.AddTarget(mod_sai)
  mod_sai.AddTarget(mod_ssi)
  mod_ssi.AddTarget(slice_1)
  mod_ssi.AddTarget(slice_2)
  mod_ssi.AddTarget(slice_3)
  mod_ssi.AddTarget(slice_4)
  mod_ssi.AddTarget(slice_5)

  slice_1.AddTarget(features_1)
  slice_2.AddTarget(features_2)
  slice_3.AddTarget(features_3)
  slice_4.AddTarget(features_4)
  slice_5.AddTarget(features_5)

  mod_gt.Initialize(input_sig)
  
  correct_count = 0;
  incorrect_count  = 0;
  
  scaled_wave = []
  for sample in input_wave:
    scaled_wave.append(float(sample / float(pow(2,15) - 1)))
  i = 0
  
  wave_chunks = grouper(buffer_length, scaled_wave, 0)

  out_bmm = []
  out_nap = []
  out_smooth_nap_profile = []
  out_strobes = []
  out_sais = []
  out_ssis = []
  out_slice_1 = []
  out_slice_2 = []
  out_slice_3 = []
  out_slice_4 = []
  out_slice_5 = []
  out_feat_1 = []
  out_feat_2 = []
  out_feat_3 = []
  out_feat_4 = []
  out_feat_5 = []
  for chunk in wave_chunks:
    i = 0
    for sample in chunk:
      input_sig.set_sample(0, i, float(sample))
      i += 1
    mod_gt.Process(input_sig)
    
    #out_bmm.append(BankToArray(mod_gt.GetOutputBank()))
    #out_nap.append(BankToArray(mod_hl.GetOutputBank()))
    out_smooth_nap_profile.append(BankToArray(mod_scaler.GetOutputBank()))
    #out_strobes.append(BankToArray(mod_strobes.GetOutputBank()))
    #out_sais.append(BankToArray(mod_sai.GetOutputBank()))
    out_ssis.append(BankToArray(mod_ssi.GetOutputBank()))
    out_slice_1.append(BankToArray(slice_1.GetOutputBank()))
    out_slice_2.append(BankToArray(slice_2.GetOutputBank()))
    out_slice_3.append(BankToArray(slice_3.GetOutputBank()))
    out_slice_4.append(BankToArray(slice_4.GetOutputBank()))
    out_slice_5.append(BankToArray(slice_5.GetOutputBank()))
    out_feat_1.append(BankToArray(features_1.GetOutputBank()))
    out_feat_2.append(BankToArray(features_2.GetOutputBank()))
    out_feat_3.append(BankToArray(features_3.GetOutputBank()))
    out_feat_4.append(BankToArray(features_4.GetOutputBank()))
    out_feat_5.append(BankToArray(features_5.GetOutputBank()))
  
  out_bank = mod_gt.GetOutputBank()
  channel_count = out_bank.channel_count()
  cfs = scipy.zeros((channel_count))
  for ch in range(0, channel_count):
    cfs[ch] = out_bank.centre_frequency(ch)
  outmat = dict(bmm=out_bmm, nap=out_nap, sais=out_sais,
                ssis=out_ssis, slice1=out_slice_1, slice2=out_slice_2, 
                slice3=out_slice_3, slice4=out_slice_4, slice5=out_slice_5,
                feat1=out_feat_1, feat2=out_feat_2, feat3=out_feat_3,
                feat4=out_feat_4, feat5=out_feat_5,
                nap_smooth=out_smooth_nap_profile, centre_freqs=cfs)
  io.savemat("src/Scripts/profile_out.mat", outmat, oned_as='column')

  pass


if __name__ == '__main__':
  main()
