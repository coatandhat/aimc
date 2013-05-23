# Author Matt Flax <flatmax\@>
#
# This C++ file is part of an implementation of Lyon's cochlear model:
# "Cascade of Asymmetric Resonators with Fast-Acting Compression"
# to supplement Lyon's upcoming book "Human and Machine Hearing"
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Author : Matt Flax <flatmax\@>
# Date : 2012.02.08

SRC= AGC.C AGCCoeff.C AGCParam.C AGCState.C CAR.C CARCoeff.C CARParam.C CARState.C Coefficients.C Ear.C EarComponent.C Ears.C IHC.C IHCCoeff.C IHCParam.C IHCState.C Parameters.C State.C PsychoAcoustics.C
OBJ= $(SRC:.C=.o)

CFLAGS = `pkg-config --cflags eigen3`

all: lib ex

VERMAJ=1
VERMIN=0
VER="\"${VERMAJ}.${VERMIN}\""

LIBNAMEBASE=libCARFAC.so
LIBNAMEMAJ=${LIBNAMEBASE}.${VERMAJ}
LIBNAME=${LIBNAMEBASE}.${VERMAJ}.${VERMIN}

lib: ${OBJ}
	${CXX} -shared -Wl,-soname,${LIBNAMEMAJ} -o ${LIBNAME} ${OBJ}
	ln -sf ${LIBNAME} ${LIBNAMEMAJ}
	ln -sf ${LIBNAMEMAJ} ${LIBNAMEBASE}

ex:
	${CXX} ${CFLAGS} -I`pwd` -o test/EarTest test/EarTest.C -L`pwd` -lCARFAC

doc:
	doxygen Doxyfile

clean:
	rm -f *.o lib* *~

.C.o:
	${CXX} ${CFLAGS} -fPIC -c -o $@ $<
