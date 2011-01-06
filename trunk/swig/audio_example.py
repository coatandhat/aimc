#!/usr/bin/env python
#
#  audio_example.py
#  
#
#  Created by Philippe Hamel (hamel.phil@gmail.com) on 10-12-22.
#
# Script to extract SAIs from an audio file using python

import aimc
import numpy

do_plot=True
# Example wav file
wavfile = '../test_data/short_example.wav'

#Loading audio file
try:
	#pygmy is available at hg clone https://bitbucket.org/douglaseck/pygmy/
	from pygmy.io.audio import AudioFile
	af=AudioFile(wavfile)
	x,sr,z=af.read(mono=True)
except:
	from scipy.io import wavfile as WF
	(sr,x) = WF.read(wavfile)
	x=x.astype('float')/float(pow(2,15) - 1)

if x.ndim <= 1:
	x=reshape(x,(x.shape[0],1))
	
nChannels = x.shape[1]

nSamples = x.shape[0]
buffer_length = 1024
zero_pad = numpy.zeros((buffer_length-(nSamples%buffer_length),nChannels)).astype('float')
print x.shape, zero_pad.shape
x = numpy.vstack((x,zero_pad))
print x.shape
assert(x.shape[0]%buffer_length == 0)

nFrames = x.shape[0]/buffer_length
print nFrames

sig = aimc.SignalBank()
sig.Initialize(nChannels,buffer_length,sr)

pzfc = aimc.ModulePZFC(aimc.Parameters())
hcl = aimc.ModuleHCL(aimc.Parameters())
local_max = aimc.ModuleLocalMax(aimc.Parameters())
sai = aimc.ModuleSAI(aimc.Parameters())

pzfc.AddTarget(hcl)
hcl.AddTarget(local_max)
local_max.AddTarget(sai)

global_params = aimc.Parameters()
pzfc.Initialize(sig,global_params)


##One chunk only
#chunk=x[:buffer_length]

output_list = []

for f in range(nFrames):
	for i in range(nChannels):
		sig.set_signal(i,x[buffer_length*f:buffer_length*(f+1),i])

	pzfc.Process(sig)
	output_bank = sai.GetOutputBank()
	n_channel = output_bank.channel_count()
	sig_length = output_bank.buffer_length()
	output_matrix = numpy.zeros((n_channel,sig_length))
	for i in range(n_channel):
		output_matrix[i] = numpy.array(output_bank.get_signal(i))
	output_list.append(output_matrix)

if do_plot:
	import pylab as P
	P.figure()
	P.imshow(output_list[0], aspect='auto', origin='lower')


################ DEPRECATED CODE (kept here for reference) #########################

#if True:
#	output_bank=output_list[0]
#	n_channel = output_bank.channel_count()
#	sig_length = output_bank.buffer_length()
#	output_matrix = numpy.zeros((n_channel,sig_length))
#	for i in range(n_channel):
#		output_matrix[i] = numpy.array(output_bank.get_signal(i))
#	print output_matrix
#	print output_matrix.shape
#	print output_matrix.sum()

#output_signal = numpy.array(output_bank.get_signal(0))

#def process_module(module, input_signal, global_params):
#	module.Initialize(input_signal,global_params)
#	module.Process(input_signal)
#	return module.GetOutputBank()

#pzfc_params = aimc.Parameters()
##pzfc_params.SetString('name','PZFCFilterBank')
##pzfc_params.SetString('child1','NAP')
#
#hcl_params = aimc.Parameters()
##hcl_params.SetString('name','NAP')
##hcl_params.SetString('child1','NAP')
#
#global_params = aimc.Parameters()
#
#pzfc = aimc.ModulePZFC(pzfc_params)
#pzfc.Initialize(sig,global_params)
#pzfc.Process(sig)
#pzfc_output = pzfc.GetOutputBank()
#
#hcl = aimc.ModuleHCL(hcl_params)
#hcl_output = process_module(hcl, pzfc_output, global_params)
#
#local_max = aimc.ModuleLocalMax( aimc.Parameters())
#local_max_output = process_module(local_max, hcl_output, global_params)
#
#sai = aimc.ModuleSAI( aimc.Parameters())
#sai_output = process_module(sai, local_max_output, global_params)