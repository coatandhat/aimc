# Introduction #
The [auditory image model](http://www.pdn.cam.ac.uk/groups/cnbh/research/aim.php) (AIM) is a computational model of the early stages of the processing that occurs in the human auditory system.

AIM-C is a C++ version of AIM developed at the [CNBH lab](http://www.pdn.cam.ac.uk/groups/cnbh/) at the University of Cambridge. The original AIM-C software was written by [Tom Walters](http://www.tomwalters.co.uk/academic/) and [Willem van Engen](http://willem.engen.nl/uni/intern-cnbh/).

The main goal of AIM-C was to provide near real-time processing of audio data through the auditory model, allowing for the use of AIM in content-based audio analysis tasks. The goal of this project is to make available a well-tested version of AIM-C to the academic community, for experimental use and further development.

This project aims to take the [original AIM-C sources](http://www.acousticscale.org/AIM-C-trac/), and produce a well-maintained, easily accessible version of AIM-C.

This site will be the home of the continued development of AIM-C. The code is undergoing a complete overhaul, both in coding style and design as it is ported to the new project. We're taking the opportunity to start afresh, with unit tests, a new coding style guide and a rewritten API. We're also developing bindings for python (and potentially other languages too) using SWIG. The code on this site can't currently be used to build a working AIM-C executable, but we're working on it!

# Design Phase #
Currently this project is basically in the [Design](Design.md) phase, although there are a few ported files in place.
  * Details on [Building](Building.md) the software.