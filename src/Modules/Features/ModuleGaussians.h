// Copyright 2008-2010, Thomas Walters
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
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

/*! \file ModuleGaussians.h
 *  \brief Gaussian features
 */

/*! \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2008/06/23
 *  \version \$Id$
 */

#ifndef AIMC_MODULES_FEATURES_GAUSSIANS_H_
#define AIMC_MODULES_FEATURES_GAUSSIANS_H_

#include <vector>

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
using std::vector;
class ModuleGaussians : public Module {
 public:
  explicit ModuleGaussians(Parameters *pParam);
  virtual ~ModuleGaussians();

  /*! \brief Process a buffer
   */
  virtual void Process(const SignalBank &input);

 private:
  /*! \brief Reset the internal state of the module
   */
  virtual void ResetInternal();

  /*! \brief Prepare the module
   *  \param input Input signal
   *  \param output true on success false on failure
   */
  virtual bool InitializeInternal(const SignalBank &input);

  bool RubberGMMCore(int iNumComponents, bool bDoInit);

  /*! \brief Number of Gaussian Components
   */
  int m_iParamNComp;

  /*! \brief Constant variance of Gaussians
   */
  float m_fParamVar;

  /*! \brief posterior probability expansion exponent
   */
  float m_fParamPosteriorExp;

  /*! \brief Maximum Number of iterations
   */
  int m_iParamMaxIt;

  /*! \brief convergence criterion
   */
  float priors_converged_;

  /*! \brief Output component positions as well as amplitudes
   */
  bool output_positions_;

  /*! \brief Total number of values in the output
   */
  int output_component_count_;

  /*! \brief The amplitudes of the components (priors)
   */
  vector<float> m_pA;

  /*! \brief The means of the components (priors)
   */
  vector<float> m_pMu;

  /*! \brief The spectral profile of the incoming buffer
   */
  vector<float> m_pSpectralProfile;

  int m_iNumChannels;
};
}  // namespace aimc

#endif  // AIMC_MODULES_FEATURES_GAUSSIANS_H_
