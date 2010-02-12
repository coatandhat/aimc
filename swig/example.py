#!/usr/bin/env python
# Copyright 2010, Thomas Walters
#
# AIM-C: A C++ implementation of the Auditory Image Model
# http:#www.acousticscale.org/AIMC
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
# along with this program.  If not, see <http:#www.gnu.org/licenses/>.

import aimc
import matplotlib

parameters = aimc.Parameters()
parameters.LoadOnly('defaults.cfg')

buffer_length = 1000
sample_rate = 44100

input_signal = aimc.SignalBank()
input_signal.Initialize(1, buffer_length, sample_rate)

filterbank = aimc.ModuleBMMPZFC(parameters)
filterbank.Initialize(input_signal)

filterbank_output = filterbank.getOutputBank()

#for s in range(0, buffer_length):
