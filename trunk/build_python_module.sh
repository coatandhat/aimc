#!/bin/bash
cd swig
python setup.py "build_ext"
python setup.py install
cd ..
