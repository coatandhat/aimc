#!/usr/bin/env python
# Copyright 2010, Thomas Walters
#
# AIM-C: A C++ implementation of the Auditory Image Model
# http://www.acousticscale.org/AIMC
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
                                   '../src/Modules/Features/ModuleGaussians.cc'],
                        swig_opts = ['-c++','-I../src/'], 
                        include_dirs=['../src/']
                        )

setup (name = 'aimc',
       version = '0.1',
       author      = "Thomas Walters <tom@acousticscale.org>",
       description = """SWIG wrapper round the core of aimc""",
       ext_modules = [aimc_module],
       py_modules = ["aimc"],
       )