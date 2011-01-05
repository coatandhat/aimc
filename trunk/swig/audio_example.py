#!/usr/bin/env python
#
#  audio_example.py
#  
#
#  Created by Philippe Hamel on 10-12-22.
#
# Script to extract SAIs from an audio file using python

# Example wav file
wavfile = '../test_data/short_example.wav'


def process_module(module, input_signal, global_params):
	module.Initialize(input_signal,global_params)
	module.Process(input_signal)
	return module.GetOutputBank()

import aimc
import numpy
#pygmy is available at hg clone https://bitbucket.org/douglaseck/pygmy/
#TODO : find a more general way to open audio file
from pygmy.io.audio import AudioFile

af=AudioFile(wavfile)
x,sr,z=af.read(mono=True)
#x,sr,z=af.read(mono=True,tlen_sec=0.1)
#x=x[:1024]

print len(x)
print sr

sig = aimc.SignalBank()
sig.Initialize(1,len(x),sr)
sig.set_signal(0,x)

pzfc_params = aimc.Parameters()
#pzfc_params.SetString('name','PZFCFilterBank')
#pzfc_params.SetString('child1','NAP')

hcl_params = aimc.Parameters()
#hcl_params.SetString('name','NAP')
#hcl_params.SetString('child1','NAP')

global_params = aimc.Parameters()

pzfc = aimc.ModulePZFC(pzfc_params)
pzfc.Initialize(sig,global_params)
pzfc.Process(sig)
pzfc_output = pzfc.GetOutputBank()

hcl = aimc.ModuleHCL(hcl_params)
hcl_output = process_module(hcl, pzfc_output, global_params)

local_max = aimc.ModuleLocalMax( aimc.Parameters())
local_max_output = process_module(local_max, hcl_output, global_params)

sai = aimc.ModuleSAI( aimc.Parameters())
sai_output = process_module(sai, local_max_output, global_params)


if True:
	output_bank=sai_output
	n_channel = output_bank.channel_count()
	sig_length = output_bank.buffer_length()
	output_matrix = numpy.zeros((n_channel,sig_length))
	for i in range(n_channel):
		output_matrix[i] = numpy.array(output_bank.get_signal(i))
	print output_matrix
	print output_matrix.shape
	print output_matrix.sum()

#output_signal = numpy.array(output_bank.get_signal(0))

