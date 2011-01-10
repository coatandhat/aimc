#!/usr/bin/env python
#
#  audio_example.py
#  
#
#  Created by Philippe Hamel (hamel.phil@gmail.com) on 11-01-05.
#
# methods to read and write aimc data files

from struct import *
import numpy as N

#int_size = calcsize('i')
#float_size =  calcsize('d')

def readbin(type,file) :
    """
    used to read binary data from a file
    """
    return unpack(type,file.read(calcsize(type)))

def read_aimc_data(filename):
    file = open(filename,'rb')
        
    nFrames = readbin('i',file)[0]
    period = readbin( 'f', file)[0] # Frame period in ms
    nChannels = readbin( 'i', file)[0] # vertical axis of an AI
    nSamples = readbin( 'i', file)[0] # horizontal axis of an AI
    sample_rate = readbin('f', file)[0] # sample rate of each channel in Hz
    
    print 'nFrames, period, nChannels, nSamples, sample_rate'
    print nFrames, period, nChannels, nSamples, sample_rate
    
    nData = nFrames * nChannels * nSamples
    print nData
    vec_data = readbin('%if'%nData,file)
    
    file.close()
    data = N.reshape(vec_data,(nFrames, nChannels, nSamples))
    return data, nFrames, period, nChannels, nSamples, sample_rate

def write_aimc_data(filename, data, sample_rate, period = 0.0):

    nFrames, nChannels, nSamples = data.shape
    
    file = open(filename,'wb')
    
    file.write(pack('i',nFrames))
    file.write(pack('f',period)) #Not correctly implemented yet
    file.write(pack('i',nChannels))
    file.write(pack('i',nSamples))
    file.write(pack('f',sample_rate))
    
    vec_data = data.flatten()
    for elem in vec_data:
        file.write(pack('f',elem))

    file.close()
    
