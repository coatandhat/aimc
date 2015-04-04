# Introduction #
AIMCopy is the AIM-C command-line tool for both making movies and extracting features in raw form from sound files.

The default behavior of the SCons script that builds AIM-C is to make an AIMCopy binary at a path that looks something like:
```
build/{win32,posix,darwin}-release/AIMCopy{,.exe}
```
Where the first part of the directory name depends on the platform on which you built the binary, and the .exe suffix is added on Windows.

AIMCopy has the following command-line flags (which closely mirror the flags for [HTK](http://htk.eng.cam.ac.uk/)'s HCopy command).
```
 AIM-C AIMCopy
  (c) 2006-2010, Thomas Walters and Willem van Engen
  http://www.acoustiscale.org/AIMC/

AIMCopy is intended as a drop-in replacement for HTK's HCopy
command. It is used for making features from audio files for
use with HTK.
Usage: 
  <flag>  <meaning>                                 <default>
  -A      Print command line arguments              off
  -C cf   Set config file to cf                     none
  -S f    Set script file to f                      none
  -V      Print version information                 off
  -D d    Write complete parameter set to file d    none
  -G g    Write graph to file g                     none
```

To run AIMCopy, at minimum the -C and -S flags must be specified.

-C specifies a [configuration file](ConfigurationFileFormat.md) to be used with AIMCopy, this dictates the set of AIM-C modules to use, and how they are joined together into a tree. Example configuration files for making movies, generating SAIs &c. can be found in `src/Configurations/`. The configuration file determines the output format(s). There can be multiple output modules specified in the configuration file, and each one will write a file based on the output path names specified in the script file (see below),  each module adding a filename suffix to the output file. The filename suffix for each output module can be set in the parameters for that module. Further details of the configuration file format are at ConfigurationFileFormat.

-S specifies a 'script' file to use. This file should contain a list of paths to the audio files to process, and the base name of the output file in each case. There should be one pair of filenames per line, separated by whitespace. The paths may be absolute, or relative to the working directory. Filenames with spaces are probably not dealt with properly yet (TODO:tom). **WARNING: Currently, AIMCopy expects all audio files listed in the script file to have the same sample rate.** Unexpected behavior may occur if you mix-and match sample rates in the same file. (TODO:tom Fix this).

-A writes the complete command-line used to call AIMCopy to stdout. This is useful for record-keeping when piping the output of AIMCopy to a file.

- V prints the AIMCopy version information (this is also done by default if no other flags are specified).

-D writes the complete module tree description, and all parameters for all the modules used in the tree (including the default options) to the file specified. This keeps a complete record of exactly what parameters were used for every module, as well as version information for all the modules used.

-G writes a directed graph of the structure of the module tree to the file specified, in [dot format](http://www.graphviz.org/). This can be used to draw a graph of the module structure using the dot command-line tool. For example
```
dot -Tps -ograph.ps graph.dot
```
will write the graph file graph.dot to the file graph.ps in postscript format.