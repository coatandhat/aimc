
// Author Matt Flax <flatmax@>
//
// This C++ file is part of an implementation of Lyon's cochlear model:
// "Cascade of Asymmetric Resonators with Fast-Acting Compression"
// to supplement Lyon's upcoming book "Human and Machine Hearing"
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef CARFACCOMMON_H_INCLUDED
#define CARFACCOMMON_H_INCLUDED

typedef float FP_TYPE; ///< The floating point type
#define AGC_STAGE_COUNT 4 ///< The number of cascades in the AGC
#define DEFAULT_SAMPLERATE 22050. ///< The default sample rate in Hz

#include <iostream>
using namespace std;
#include <Eigen/Dense>
using namespace Eigen;

#include "PsychoAcoustics.H"

#define AGC_DESIGN_ITERATION_ERROR -100 ///< Error when designing the AGC filter taps
#define AGC_DESIGN_TAPS_OOB_ERROR -101 ///< The number of taps requested have not been accounted for in the code yet.
#define AGC_FIR_TAP_COUNT_ERROR -102 ///< The number of taps passed to the AGC::Design_FIR_coeffs method are not handled

/**
    \mainpage CARFAC C++

    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

    \section intro_sec Introduction

    This C++ code implements Dick Lyon's CARFAC model for the peripheral hearing circuit.

    \section code_philo Philosophy of the implementation

    \subsection dd Matching the design document

    As requested by the CARFAC design description, this codebase uses Eigen to compute
    matrix/vector operations.

    \subsection oo Object oriented acritecture

    Where possible common paradigms inherit from common Objects. This aims to minimise
    the amount of coding required to implement and modify CARFAC. For example, the EarComponent
    encapsulates the CAR, AGC and IHC where all of the have the common features of Coefficients,
    Parameters and State.

    \subsection cc Common code

    Where possible typedefs, definitions, includes and namespace inclusions which are common
    to the code, or usefull outside of class definitions are put in the CARFACCommon.H file.

    \subsection fileNames File naming convention and header guards

    C++ files in this codebase are named using the .C and .H suffixes (C code uses .c and .h).

    Header guards are labeled using the files name with '_' characters, for example.H would become
    EXAMPLE_H_.

    \subsection globalVars Global variables

    In general the use of global variables is discouraged. Where possible the code must be instantiated
    many times on the same computer system and the use of global variables complicates having multiple
    instances of shared library classes.

    \subsection cvns Class and variable naming convention

    In general, classes begin with capitol letters and a variable name begis with a lower case character.
    The consider a class for example :

    \code
    class ForExample { class def here };

    ForExample forExample;

    class OMG { class def here };

    OMG omg; // here it is clear what is the type and what is the variable.

    \endcode

    The class 'ForExample' is defined, and the variable name 'forExample' may be used in the code,
    which clearly indicates the type of the variable.

    The concept of labeling variables using 'p' for pointer, and type name references is not necessary,
    and in some cases discouraged. Consider for example, \code float *fs \endcode defining the pointer to the sample rate.
    If we were to use \code float * pFFs // don't do this - difficult to see that pFFs references fs - the sample rate \endcode , it becomes rather difficult to understand that pFFs actualy
    points to the sample rate.

    A deeper argument for using simple variable names (in C++) is as follows. Good engineers program
    classes and methods which are short and concise. As monitors (LCDs) get larger, most of your methods
    and in some cases classes are visible in one or two pages of your monitor. Consequently if the exact
    type of a variable named 'fs' needs to be found, it is as simple as looking at the top of your monitor
    or scrolling up a little to find a method's input variable name/type. In the case of class member
    variables, a class 'SoundCard' is expected to define a sound card. Consequently certain member variables
    are expected to exist, for example, fs, inputChannels, outputChannels and so on. If the actual types
    of these variables have been forgotten, then the header file is referenced, and this is normally as
    simple as a few key strokes to change from the SoundCard.C file to the SoundCard.H file to inspect
    the names and types of available member variables.

    \copyright {\code
    Author Matt Flax <flatmax@>


    This C++ file is part of an implementation of Lyon's cochlear model:

    "Cascade of Asymmetric Resonators with Fast-Acting Compression"

    to supplement Lyon's upcoming book "Human and Machine Hearing"


    Licensed under the Apache License, Version 2.0 (the "License");

    you may not use this file except in compliance with the License.

    You may obtain a copy of the License at


        http://www.apache.org/licenses/LICENSE-2.0


    Unless required by applicable law or agreed to in writing, software

    distributed under the License is distributed on an "AS IS" BASIS,

    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

    See the License for the specific language governing permissions and

    limitations under the License. \endcode}
*/

#endif // CARFACCOMMON_H_INCLUDED
