Quick script to get the sources, install the dependencies and build AIM-C on Ubuntu. Tested on a clean install of Ubuntu 10.10.

```
#!/bin/bash
sudo apt-get -y install libsndfile1-dev libcairo-dev subversion scons build-essential libboost-dev
svn checkout http://aimc.googlecode.com/svn/trunk/ aimc-read-only
cd aimc-read-only
scons
```

Building Python bindings

```
#!/bin/bash
sudo apt-get -y install python-dev swig
cd ~/aimc-read-only
scripts/build_python_module.sh
```

Building documentation

```
#!/bin/bash
sudo apt-get -y install doxygen graphviz
cd ~/aimc-read-only
doxygen doc/Doxyfile
```