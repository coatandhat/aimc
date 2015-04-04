The module [FileOutputAIMC](http://code.google.com/p/aimc/source/browse/trunk/src/Modules/Output/FileOutputAIMC.cc) provides data in a simple format which can be read by other programs.

The data is stored as 32-bit floats with the native byte-ordering of the machine the file was written on. Before the data, there is a simple header.

## Header format ##
The first 20 bytes of the file contain the following fields:

  * 32-bit unsigned integer: frame count - total number of frames in the file (these can be SAIs, filterbank output frames or other things (eg. features), but all frames in the file are of the same type)
  * 32-bit float: frame period (the interval between adjacent frames in milliseconds) (TODO(tom): this is currently not set properly).
  * 32-bit unsigned integer: channel count (number of frequency channels in the output)
  * 32-bit unsigned integer frame length (length of each frame in samples)
  * 32-bit float: sample rate (audio sample rate in Hz)


## Reading the files using MATLAB ##
Use [the MATLAB AIMCread function](http://code.google.com/p/aimc/source/browse/trunk/matlab/AIMCread.m) to read the files.