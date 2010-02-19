# Copyright 2006-2010, Willem van Engen, Thomas Walters
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

## @author Thomas Walters <tom@acousticscale.org>
#  @author Willem van Engen <cnbh@willem.engen.nl>
#  @date created 2010/02/02
#  @version \$Id$

"""@package SConstruct
SConstruct file for the aimc project

"""

import os
import shutil

# Define the command-line options for running scons
options = Variables()
options.Add(BoolVariable('mingw', 
                         'Compile on Windows using mingw rather than msvc',
                         False))

# Environment variables
env = Environment(options = options, ENV = os.environ)
if env['mingw']:
  # SCons Defaults to MSVC if installed on Windows.
  env = Environment(options = opts, ENV = os.environ, tools = ['mingw'])

# Platform
build_platform = env['PLATFORM']
target_platform = build_platform

# Build products location and executable name
build_dir = os.path.join('build', target_platform + '-release')
target_executable = 'aimc'

# Create build products directory if necessary
if not os.path.exists(build_dir):
  os.makedirs(build_dir)
env.SConsignFile(os.path.join(build_dir,'.sconsign'))

# Set any platform-specific environment variables and options
if target_platform == 'win32':
  env.AppendUnique(CPPDEFINES = ['_WINDOWS', 'WIN32', 
                                 'WINVER=0x0400', '_CONSOLE'])
elif target_platform == 'darwin':
  env.AppendUnique(CPPDEFINES = ['_MACOSX'])

# Compiler selection based on platform
# compiler can be one of: gcc msvc
compiler = 'gcc'
if (build_platform == 'win32' 
    and target_platform == 'win32' 
    and not env['mingw']):
  compiler = 'msvc'

# Compiler-specific options:
# Microsoft visual studio
if compiler == 'msvc':
  env.AppendUnique(CPPFLAGS = ['/arch:SSE2', '/nologo', '/W3', '/EHsc'])
  env.AppendUnique(CPPDEFINES = ['_CRT_SECURE_NO_DEPRECATE', 
                                 '_RINT_REQUIRED'])
  env.AppendUnique(CPPFLAGS = ['/Ox'])
  env.AppendUnique(CPPDEFINES = ['NDEBUG', '_ATL_MIN_CRT'])

# GNU compiler collection
elif compiler == 'gcc':
  env['STRIP'] = 'strip'
  env.AppendUnique(CPPFLAGS = ['-Wall'])
  env.AppendUnique(CPPFLAGS = ['-O3', '-fomit-frame-pointer'])
  if env['mingw']:
    if not env['PLATFORM'] == 'win32':
      print('Cross-compilation for Windows is not supported')
      Exit(1)
else:
  print('Unsupported compiler: ' + compiler)
  Exit(1)

# Sources common to every version
common_sources = ['Support/Common.cc',
                  'Support/SignalBank.cc',
                  'Support/Parameters.cc',
                  'Support/Module.cc',
                  'Modules/Input/ModuleFileInput.cc',
                  'Modules/BMM/ModuleGammatone.cc',
                  'Modules/BMM/ModulePZFC.cc',
                  'Modules/NAP/ModuleHCL.cc',
                  'Modules/Strobes/ModuleParabola.cc',
                  'Modules/SAI/ModuleSAI.cc',
                  'Modules/Features/ModuleGaussians.cc',
                  'Modules/Output/FileOutputHTK.cc']
    
if not target_platform == 'win32':
  # On windows, utf support is builtin for SimpleIni
  # but not on other platforms
  common_sources += ['Support/ConvertUTF.c']

# Choose file which contains main()
sources = common_sources + ['Main/aimc.cc']

# Place the build products in the corect place
env.BuildDir('#' + build_dir, '#', duplicate = 0)

# Look for the sources in the correct place
env.Append(CPPPATH = '#src')

# Dependencies
deplibs = ['sndfile']

for depname in deplibs:
  env.ParseConfig('pkg-config --cflags --libs ' + depname)

env.AppendUnique(LIBS = deplibs)

# Set up the builder to build the program
program = env.Program(target = os.path.join(build_dir, target_executable), 
                      source = map(lambda x: '#' + build_dir + '/src/' + x,
                                   sources))
env.Default(program)
