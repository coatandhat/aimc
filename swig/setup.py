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

"""
setup.py file for SWIG wrappers around aimc
"""

from distutils.core import setup, Extension

aimc_module = Extension('_aimc',
                        sources = ['aim_modules.i',
                                   '../src/Support/Common.cc',
                                   '../src/Support/Parameters.cc',
                                   '../src/Support/SignalBank.cc', 
                                   '../src/Support/Module.cc',
                                   '../src/Modules/BMM/ModuleGammatone.cc',
                                   '../src/Modules/Features/ModuleGaussians.cc',
                                   '../src/Modules/BMM/ModulePZFC.cc',
                                   '../src/Modules/NAP/ModuleHCL.cc',
                                   '../src/Modules/Strobes/ModuleParabola.cc',
                                   '../src/Modules/Strobes/ModuleLocalMax.cc',
                                   '../src/Modules/SAI/ModuleSAI.cc',
                                   '../src/Modules/SSI/ModuleSSI.cc',
                                   '../src/Modules/Profile/ModuleSlice.cc',
                                   '../src/Modules/Profile/ModuleScaler.cc'],
                        swig_opts = ['-c++','-I../src/'], 
                        include_dirs=['../src/', '/opt/local/include/']
                        )

setup (name = 'aimc',
       version = '0.1',
       author      = "Thomas Walters <tom@acousticscale.org>",
       description = """SWIG wrapper round the core of aimc""",
       # A bug with some versions of SWIG/distutils/python and some configurations requires
       # us to repeat the swig_opts here.
       options={'build_ext':{'swig_opts':'-c++ -I../src/'}}, 
       ext_modules = [aimc_module],
       py_modules = ["aimc"],
       )
