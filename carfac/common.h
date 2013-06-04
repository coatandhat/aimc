//
//  common.h
//  CARFAC Open Source C++ Library
//
//  Created by Alex Brandmeyer on 5/10/13.
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

#ifndef CARFAC_COMMON_H
#define CARFAC_COMMON_H

// This macro disallows the copy constructor and operator= functions.
// This should be used in the private: declarations for a class.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// The Eigen library is used extensively for floating point arrays.
// For more information, see: http://eigen.tuxfamily.org
#include <Eigen/Dense>

// The 'FPType' typedef is used to enable easy switching in precision level.
// It's currently set to double for during the unit testing phase of the
// project.
typedef float FPType;
// A typedef is used to define a one-dimensional Eigen array with the same
// precision level as FPType.
typedef Eigen::Array<FPType, Eigen::Dynamic, 1> ArrayX;
typedef Eigen::Array<FPType, Eigen::Dynamic, Eigen::Dynamic> ArrayXX;

// A fixed value of PI is defined throughout the project.
static const FPType kPi = 3.141592653589793238;

#endif  // CARFAC_COMMON_H