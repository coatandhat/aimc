#!/usr/bin/env python
# Copyright 2010, Thomas Walters
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

import aimc
module_params = aimc.Parameters()
global_params = aimc.Parameters()
mod_gauss = aimc.ModuleGaussians(module_params)
sig = aimc.SignalBank()
sig.Initialize(115, 1, 44100)
mod_gauss.Initialize(sig, global_params)
mod_gauss.Process(sig)

