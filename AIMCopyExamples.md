All examples assume a working directory of the root of the AIM-C source tree.

## SAI movie ##

Making a movie of the stabilized auditory image for every file in script.scp using the configuration provided in
[src/Configurations/SAI\_movie.aimcconfig](http://code.google.com/p/aimc/source/browse/trunk/src/Configurations/SAI_movie.aimcconfig).

Command-line:
```
build/darwin-release/AIMCopy -C src/Configurations/SAI_movie.aimcconfig -S script.scp
```

The contents of script.scp are simple. For example the following script will take sa1.wav sa2.wav and sa3.wav in the working directory and convert them to sa1.mov sa2.mov and sa3.mov respectively. (The .mov suffix is added by the graphics\_time module).
```
sa1.wav sa1.
sa2.wav sa2.
sa3.wav sa3.
```

## Raw SAIs ##
Calculate stabilized auditory images for files in script.scp using the configuration from [src/Configurations/SAI\_dump.aimcconfig](http://code.google.com/p/aimc/source/browse/trunk/src/Configurations/SAI_dump.aimcconfig)

Command-line:
```
build/darwin-release/AIMCopy -C src/Configurations/SAI_dump.aimcconfig -S script.scp
```

In this case, the files are in the [AIM-C data format](AIMCDataFormat.md) and will be **very large** (ca. 100Mb for a 5-second sound).