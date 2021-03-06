
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
#ifndef EARS_H_
#define EARS_H_

#include <vector>

#include "Ear.H"

/**
Author: Matt Flax <flatmax>
Date : 2013.02.07

Ears is a class which manages a vector of Ear classes.
*/
class Ears : public vector<Ear> {
public:
    Ears();
    virtual ~Ears();
};

#endif // EARS_H_
