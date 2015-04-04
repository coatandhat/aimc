# Unit Tests #
The goal is for every component of AIM-C to have a corresponding unit test.
C++ unit testing is done using google test, the testing user interface.

Unit tests can also be written in python. Currently these are done on an ad-hoc basis, but they will be replaced eventually with a proper testing framework (hopefully).

# Valgrind #
Valgrind is an excellent tool for detecting memory leaks. When developing modules, it is highly advisable to run valgrind on the aimc executable while it runs a sound file through a complete set of modules.