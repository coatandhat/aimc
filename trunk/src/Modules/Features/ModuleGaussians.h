// Copyright 2008-2010, Thomas Walters
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*! \file ModuleGaussians.h
 *  \brief Gaussian features
 */

/*! \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2008/06/23
 *  \version \$Id: ModuleGaussians.h 2 2010-02-02 12:59:50Z tcw $
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
  float m_fParamPriorsConverged;

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
