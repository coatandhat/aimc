# Windows build #
Windows build instructions tested on Windows XP. If you know about building on Windows 7 or other platforms, please add the details here.

Install [Python 2.7](http://www.python.org/download/) (default options)

Install [SCons 2.0.0.final.0](http://www.scons.org/download.php)

Add C:\Python27\Scripts to the path
Control Panel->System->Advanced->Environment Variables->System Variables>PATH->Edit->Add ';C:\Python27\Scripts' (sans quotes) to the end of the line

Install subversion

There's a  [list of various subversion packages for Windows here](http://subversion.apache.org/packages.html#windows), I used [SilkSVN](http://www.sliksvn.com/en/download).

Install [libsndfile](http://www.mega-nerd.com/libsndfile/#Download)

Download and install [MS Visual C++ Express 2010](http://www.microsoft.com/express/Downloads/#2010-Visual-CPP)

The defaults should be fine for installing just VC++ 2010

To build the video output for AIM-C, you'll need to install the Cairo development libraries and a few other support files. According to the page at http://cairographics.org/download/ you need the cairo dev library and the runtime libraries for libpng, freetype, fontconfig, expat and Zlib from [the GTK Windows downloads page](http://www.gtk.org/download-windows.html). Unzip all these  into a directory somewhere (I used C:\Program Files\cairo\) and set the windows\_cairo\_libraries variable at the top of the SConstruct file to the correct path.

To use the noise generator module in AIM-C, you need to install the boost random library.
To do this download the boost package for your visual studio version from [boostpro.com](http://www.boostpro.com/download/) and install at least the 'random' package.
(If you don't need this module, you can remove ModuleNoise.cc from the list of modules in the SConstruct file and then you don't need boost at all).


Then, to build:
Open (Visual Studio) Command Prompt

cd to the directory of your choice

> svn checkout http://aimc.googlecode.com/svn/trunk/ aimc-read-only

Run the 'Visual Studio Command Prompt' and execute the command:
> scons

## Building the Python Module ##
Download [SWIG](http://www.swig.org/download.html)

If using MSVC 2010
Edit
C:\Python27\Lib\distutils\msvc9compiler.py line 243 to be
> toolskey = "VS100COMNTOOLS"

For all versions of MSVC, you may also need to add the line
> ld\_args.append('/MANIFEST')
just after the line
> > ld\_args.append('/MANIFESTFILE:' + temp\_manifest)

then run

> pythonsetup.py "build\_ext"