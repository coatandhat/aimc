
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
#ifndef CARCOEFFS_H_
#define CARCOEFFS_H_

#include "Coefficients.H"

class CAR; // required for decleration as a friend

/**
    \author {Matt Flax <flatmax\@>}
    \date 2013.02.08

    Coefficients for the Cascaded Auditory Resonators
*/
class CARCoeff : public Coefficients {
    Matrix<FP_TYPE, Dynamic,1> r1_coeffs; ///< Dick, whats this ?
    Matrix<FP_TYPE, Dynamic,1> a0_coeffs; ///< Dick, whats this ?
    Matrix<FP_TYPE, Dynamic,1> c0_coeffs; ///< Dick, whats this ?
    Matrix<FP_TYPE, Dynamic,1> h_coeffs; ///< Dick, whats this ?
    Matrix<FP_TYPE, Dynamic,1> g0_coeffs; ///< Dick, whats this ?
    Matrix<FP_TYPE, Dynamic,1> zr_coeffs; ///< Dick, whats this ?

public:
    CARCoeff(); ///< Constructor
    virtual ~CARCoeff(); ///< Destructor

    friend class CAR;
};

#endif // CARCOEFFS_H_
