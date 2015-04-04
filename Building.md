# Introduction #

AIM-C is built using the [SCons](http://www.scons.org/) build system. SCons is multiplatform, and is based on Python.

# Build Environment #
One of the major requirements of AIM-C is that it is written, as far as possible, in standard C++ (including some heavy dependence on the standard template library). This means that (in theory at least) it will compile under any reasonably sane C++ compiler. In practice, so far development has taken place mainly using GCC on Mac OS 10.6.

# External Dependencies #
Current dependencies are [libsndfile](http://www.mega-nerd.com/libsndfile/) and [cairo](http://cairographics.org) for audio input and graphics output respectively. [PortAudio](http://www.portaudio.com/) is a requirement for live audio input. This will be ported over soon. The boost c++ extensions are also required for the module that adds noise to the input.

On Ubuntu, the necessary build tools and dependencies can be installed using the following command.

```
sudo apt-get install libsndfile-dev cairo-dev 
```

The project also uses [simpleini](http://code.jellycan.com/simpleini/) version 4.12 for storing, parsing and writing out parameters files, but this file (and the UTF support code that it requires) is included in the source repository.

# Build Tools #
## Building ##

Assuming SCons is installed,  to build the source tree simply navigate to the top level of the source tree (where the SConstruct file resides) and issue the command:

`scons`

## Getting SCons ##
On GNU/Linux systems, either use your favourite package manager to install scons, for example on Debian / Ubuntu:

`sudo apt-get install scons`

On Mac OS X, I'd recommend using MacPorts. It's a handy way of installing all sorts of free software. First install MacPorts, then run the command:

`sudo port install scons`

Alternatively, in either case, you can just download and install a tarball from the [SCons site](http://www.scons.org/).

On Windows
See BuildingOnWindows