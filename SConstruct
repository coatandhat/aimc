# Copyright 2006-2010, Willem van Engen, Thomas Walters
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

## @author Thomas Walters <tom@acousticscale.org>
#  @author Willem van Engen <cnbh@willem.engen.nl>
#  @date created 2010/02/02
#  @version \$Id$

"""@package SConstruct
SConstruct file for the aimc project

"""

import os
import shutil

# Location of libraries / headers on Windows
windows_libsndfile_location = "C:\\Program Files\\Mega-Nerd\\libsndfile\\"
windows_cairo_location = "C:\\Program Files\\cairo\\"

# Sources common to every version
common_sources = ['Support/Common.cc',
                  'Support/FileList.cc',
                  'Support/SignalBank.cc',
                  'Support/Parameters.cc',
                  'Support/Module.cc',
                  'Support/ModuleFactory.cc',
                  'Support/ModuleTree.cc',
                  'Modules/Input/ModuleFileInput.cc',
                  'Modules/BMM/ModuleGammatone.cc',
                  'Modules/BMM/ModulePZFC.cc',
                  'Modules/NAP/ModuleHCL.cc',
                  'Modules/Strobes/ModuleParabola.cc',
                  'Modules/Strobes/ModuleLocalMax.cc',
                  'Modules/SAI/ModuleSAI.cc',
                  'Modules/SSI/ModuleSSI.cc',
                  'Modules/Profile/ModuleSlice.cc',
                  'Modules/Profile/ModuleScaler.cc',
                  'Modules/Output/FileOutputHTK.cc',
                  'Modules/Output/FileOutputAIMC.cc',
                  'Modules/Output/FileOutputJSON.cc',
                  #'Modules/Output/OSCOutput.cc',
                  'Modules/Features/ModuleGaussians.cc',
                  'Modules/Features/ModuleBoxes.cc',]
                  #'Modules/Features/ModuleDCT.cc' ]

graphics_sources = [ 'Modules/Output/Graphics/GraphAxisSpec.cc',
                     'Modules/Output/Graphics/GraphicsView.cc',
                     'Modules/Output/Graphics/GraphicsViewTime.cc',
                     'Modules/Output/Graphics/Scale/Scale.cc',
                     'Modules/Output/Graphics/Devices/GraphicsOutputDevice.cc',
                     'Modules/Output/Graphics/Devices/GraphicsOutputDeviceCairo.cc',
                     'Modules/Output/Graphics/Devices/GraphicsOutputDeviceMovie.cc',]
                     #'Modules/Output/Graphics/Devices/GraphicsOutputDeviceMovieDirect.cc' ]
graphics_libraries = [ 'cairo',
                        ]

# List of currently incative source files which we may want to add back in
sources_disabled = ['Modules/SNR/ModuleNoise.cc',
                    ]

# File which contains main()
#sources = common_sources + graphics_sources + ['Main/AIMCopy_SSI_Features_v3.cc']
#sources = common_sources + ['Main/AIMCopy_SSI_Features_v4_PZFC.cc']
sources = common_sources + graphics_sources + ['Main/AIMCopy.cc']
#sources = common_sources + ['Main/aimc.cc']

# Test sources
test_sources = ['Modules/Profile/ModuleSlice_unittest.cc']
test_sources += common_sources

# Define the command-line options for running scons
options = Variables()
options.Add(BoolVariable('mingw',
                         'Compile on Windows using mingw rather than msvc',
                         False))
options.Add(BoolVariable('symbols',
                         'Add debuging symbols when compiling on gcc',
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
#target_executable = 'aimc'
target_executable = 'AIMCopy'
test_executable = 'aimc_tests'

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
  env.AppendUnique(CPPFLAGS = ['-O1',])# '-fomit-frame-pointer'])
  if env['symbols']:
    env.AppendUnique(CPPFLAGS = ['-g'])
  if env['mingw']:
    if not env['PLATFORM'] == 'win32':
      print('Cross-compilation for Windows is not supported')
      Exit(1)
else:
  print('Unsupported compiler: ' + compiler)
  Exit(1)

# To make a statically-linked version for os 10.4 and up...
#if build_platform == 'darwin':
#  env.AppendUnique(CPPFLAGS = ['-arch', 'i386'])
#  env.AppendUnique(LINKFLAGS = ['-arch', 'i386'])
#  env.AppendUnique(LINKFLAGS = ['-Wl'])
#  env.AppendUnique(LINKFLAGS = ['-search_paths_first'])
#  env.AppendUnique(MACOSX_DEPLOYMENT_TARGET = ['10.4'])
#  env.AppendUnique(GCC_VERSION = ['4.0'])
#  env.Replace(CC = ['gcc-4.0'])
#  env.Replace(CXX = ['gcc-4.0'])
#  env.AppendUnique(CPPFLAGS = ['-fno-stack-protector','-isysroot', '/Developer/SDKs/MacOSX10.5.sdk', '-mmacosx-version-min=10.4'])
#  deplibs = ['sndfile', 'flac', 'vorbis', 'vorbisenc', 'ogg']
if not target_platform == 'win32':
  # On windows, utf support is builtin for SimpleIni
  # but not on other platforms
  sources += ['Support/ConvertUTF.c']

# Place the build products in the corect place
env.VariantDir('#' + build_dir, '#', duplicate = 0)

# Look for the sources in the correct place
env.Append(CPPPATH = ['#src'])

# Dependencies
deplibs = ['sndfile']
deplibs += graphics_libraries

#env.Append(CPPPATH = ['#external/oscpack/'])
#env.AppendUnique(LIBPATH = ['#external/oscpack/'])

if target_platform != 'win32':
  for depname in deplibs:
    env.ParseConfig('pkg-config --cflags --libs ' + depname)
else:
  #env.AppendUnique(LIBS = ['wsock32', 'winmm'])
  if 'sndfile' in deplibs:
    ###### libsndfile ########################################
    # This one is only valid for win32 and already precompiled
    # Only need to create .lib file from .def
    shutil.copyfile(windows_libsndfile_location + '/libsndfile-1.dll',
                    build_dir+'/libsndfile-1.dll')
    if compiler=='msvc':
      shutil.copyfile(windows_libsndfile_location + '/libsndfile-1.def',
                      build_dir+'/libsndfile-1.def')
      env.Command(build_dir + '/libsndfile-1.lib', build_dir + '/libsndfile-1.def',
                  env['AR'] + ' /nologo /machine:i386 /def:$SOURCE /out:$TARGET')
      env.Append(CPPPATH = [windows_libsndfile_location + '/include/'])
      env.AppendUnique(LIBPATH = [build_dir])
      # Replace 'sndfile' with 'sndfile-1'
      deplibs.remove('sndfile')
      deplibs.append('libsndfile-1')
  if 'cairo' in deplibs:
    shutil.copyfile(windows_cairo_location + '/bin/libcairo-2.dll',
                    build_dir+'/libcairo-2.dll')
    env.Append(CPPPATH = [windows_cairo_location + '/include/cairo/'])
    env.AppendUnique(LIBPATH = [windows_cairo_location + '/lib/'])

#deplibs.append('liboscpack')
env.AppendUnique(LIBS = deplibs)



# Builder for the main program
program = env.Program(target = os.path.join(build_dir, target_executable), 
                      source = map(lambda x: '#' + build_dir + '/src/' + x,
                                   sources))
env.Alias(target_executable, os.path.join(build_dir, target_executable))
env.Default(program)

#test_env = env.Clone()
#test_libs = ['gtest', 'gtest_main']
##for depname in test_libs:
##  test_env.ParseConfig('pkg-config --cflags --libs ' + depname)
#test_env.AppendUnique(LIBPATH = ['/usr/local/lib'],
#                      CPPPATH = ['/usr/local/lib'],
#                      LIBS = test_libs)
#
#test = test_env.Program(target = os.path.join(build_dir, test_executable),
#                        source = map(lambda x: '#' + build_dir + '/src/' + x,
#                                     test_sources))
#env.Alias('test', os.path.join(build_dir, test_executable))
