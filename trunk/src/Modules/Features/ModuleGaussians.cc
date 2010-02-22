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

/*! \file
 *  \brief Gaussian features - based on MATLAB code by Christian Feldbauer
 */

/*!
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2008/06/23
 * \version \$Id: ModuleGaussians.cc 2 2010-02-02 12:59:50Z tcw $
 */

#include <math.h>

#include "Modules/Features/ModuleGaussians.h"
#include "Support/Common.h"

namespace aimc {
ModuleGaussians::ModuleGaussians(Parameters *params) : Module(params) {
  // Set module metadata
  module_description_ = "Gaussian Fitting to SSI profile";
  module_identifier_ = "gaussians";
  module_type_ = "features";
  module_version_ = "$Id: ModuleGaussians.cc 2 2010-02-02 12:59:50Z tcw $";

  m_iParamNComp = parameters_->DefaultInt("features.gaussians.ncomp", 4);
  m_fParamVar = parameters_->DefaultFloat("features.gaussians.var", 115.0);
  m_fParamPosteriorExp =
    parameters_->DefaultFloat("features.gaussians.posterior_exp", 6.0);
  m_iParamMaxIt = parameters_->DefaultInt("features.gaussians.maxit", 250);

  // The parameters system doesn't support tiny numbers well, to define this
  // variable as a string, then convert it to a float afterwards
  parameters_->DefaultString("features.gaussians.priors_converged", "1e-7");
  m_fParamPriorsConverged =
    parameters_->GetFloat("features.gaussians.priors_converged");
}

ModuleGaussians::~ModuleGaussians() {
}

bool ModuleGaussians::InitializeInternal(const SignalBank &input) {
  m_pA.resize(m_iParamNComp, 0.0f);
  m_pMu.resize(m_iParamNComp, 0.0f);

  // Assuming the number of channels is greater than twice the number of
  // Gaussian components, this is ok
  if (input.channel_count() >= 2 * m_iParamNComp) {
    output_.Initialize(m_iParamNComp, 1, input.sample_rate());
  } else {
    LOG_ERROR(_T("Too few channels in filterbank to produce sensible "
                 "Gaussian features. Either increase the number of filterbank"
                 " channels, or decrease the number of Gaussian components"));
    return false;
  }

  m_iNumChannels = input.channel_count();
  m_pSpectralProfile.resize(m_iNumChannels, 0.0f);

  return true;
}

void ModuleGaussians::ResetInternal() {
  m_pSpectralProfile.clear();
  m_pSpectralProfile.resize(m_iNumChannels, 0.0f);
  m_pA.clear();
  m_pA.resize(m_iParamNComp, 0.0f);
  m_pMu.clear();
  m_pMu.resize(m_iParamNComp, 0.0f);
}

void ModuleGaussians::Process(const SignalBank &input) {
  if (!initialized_) {
    LOG_ERROR(_T("Module ModuleGaussians not initialized."));
    return;
  }
  // Calculate spectral profile
  for (int iChannel = 0;
       iChannel < input.channel_count();
       ++iChannel) {
    m_pSpectralProfile[iChannel] = 0.0f;
    for (int iSample = 0;
         iSample < input.buffer_length();
         ++iSample) {
      m_pSpectralProfile[iChannel] += input[iChannel][iSample];
    }
    m_pSpectralProfile[iChannel] /= static_cast<float>(input.buffer_length());
  }

  float spectral_profile_sum = 0.0f;
  for (int i = 0; i < input.channel_count(); ++i) {
    spectral_profile_sum += m_pSpectralProfile[i];
  }

  float logsum = log(spectral_profile_sum);
  if (!isinf(logsum)) {
    output_.set_sample(m_iParamNComp - 1, 0, logsum);
  } else {
    output_.set_sample(m_iParamNComp - 1, 0, -1000.0);
  }

  for (int iChannel = 0;
       iChannel < input.channel_count();
       ++iChannel) {
    m_pSpectralProfile[iChannel] = pow(m_pSpectralProfile[iChannel], 0.8);
  }

  RubberGMMCore(2, true);

  float fMean1 = m_pMu[0];
  float fMean2 = m_pMu[1];
  // LOG_INFO(_T("Orig. mean 0 = %f"), m_pMu[0]);
  // LOG_INFO(_T("Orig. mean 1 = %f"), m_pMu[1]);
  // LOG_INFO(_T("Orig. prob 0 = %f"), m_pA[0]);
  // LOG_INFO(_T("Orig. prob 1 = %f"), m_pA[1]);

  float fA1 = 0.05 * m_pA[0];
  float fA2 = 1.0 - 0.25 * m_pA[1];

  // LOG_INFO(_T("fA1 = %f"), fA1);
  // LOG_INFO(_T("fA2 = %f"), fA2);

  float fGradient = (fMean2 - fMean1) / (fA2 - fA1);
  float fIntercept = fMean2 - fGradient * fA2;

  // LOG_INFO(_T("fGradient = %f"), fGradient);
  // LOG_INFO(_T("fIntercept = %f"), fIntercept);

  for (int i = 0; i < m_iParamNComp; ++i) {
    m_pMu[i] = (static_cast<float>(i)
                / (static_cast<float>(m_iParamNComp) - 1.0f))
                * fGradient + fIntercept;
                // LOG_INFO(_T("mean %d = %f"), i, m_pMu[i]);
  }

  for (int i = 0; i < m_iParamNComp; ++i) {
    m_pA[i] = 1.0f / static_cast<float>(m_iParamNComp);
  }

  RubberGMMCore(m_iParamNComp, false);

  for (int i = 0; i < m_iParamNComp - 1; ++i) {
    if (!isnan(m_pA[i])) {
      output_.set_sample(i, 0, m_pA[i]);
    } else {
      output_.set_sample(i, 0, 0.0f);
    }
  }

  PushOutput();
}

bool ModuleGaussians::RubberGMMCore(int iNComponents, bool bDoInit) {
  int iSizeX = m_iNumChannels;

  // Normalise the spectral profile
  float fSpectralProfileTotal = 0.0f;
  for (int iCount = 0; iCount < iSizeX; iCount++) {
    fSpectralProfileTotal += m_pSpectralProfile[iCount];
  }
  for (int iCount = 0; iCount < iSizeX; iCount++) {
    m_pSpectralProfile[iCount] /= fSpectralProfileTotal;
  }

  if (bDoInit) {
    // Uniformly spaced components
    float dd = (iSizeX - 1.0f) / iNComponents;
    for (int i = 0; i < iNComponents; i++) {
      m_pMu[i] = dd / 2.0f + (i * dd);
      m_pA[i] = 1.0f / iNComponents;
    }
  }

  vector<float> pA_old;
  pA_old.resize(iNComponents);
  vector<float> pP_mod_X;
  pP_mod_X.resize(iSizeX);
  vector<float> pP_comp;
  pP_comp.resize(iSizeX * iNComponents);

  for (int iIteration = 0; iIteration < m_iParamMaxIt; iIteration++) {
    // (re)calculate posteriors (component probability given observation)
    // denominator: the model density at all observation points X
    for (int i = 0; i < iSizeX; ++i) {
      pP_mod_X[i] = 0.0f;
    }

    for (int i = 0; i < iNComponents; i++) {
      for (int iCount = 0; iCount < iSizeX; iCount++) {
        pP_mod_X[iCount] += 1.0f / sqrt(2.0f * M_PI * m_fParamVar)
                            * exp((-0.5f)
                            * pow(static_cast<float>(iCount+1) - m_pMu[i], 2)
                            / m_fParamVar) * m_pA[i];
      }
    }

    for (int i = 0; i < iSizeX * iNComponents; ++i) {
      pP_comp[i] = 0.0f;
    }

    for (int i = 0; i < iNComponents; i++) {
      for (int iCount = 0; iCount < iSizeX; iCount++) {
        pP_comp[iCount + i * iSizeX] =
          1.0f / sqrt(2.0f * M_PI * m_fParamVar)
          * exp((-0.5f) * pow((static_cast<float>(iCount + 1) - m_pMu[i]), 2)
          / m_fParamVar);
        pP_comp[iCount + i * iSizeX] =
          pP_comp[iCount + i * iSizeX] * m_pA[i] / pP_mod_X[iCount];
      }
    }

    for (int iCount = 0; iCount < iSizeX; ++iCount) {
      float fSum = 0.0f;
      for (int i = 0; i < iNComponents; ++i) {
        pP_comp[iCount+i*iSizeX] = pow(pP_comp[iCount + i * iSizeX],
                                       m_fParamPosteriorExp);  // expansion
        fSum += pP_comp[iCount+i*iSizeX];
      }
      for (int i = 0; i < iNComponents; ++i)
        pP_comp[iCount+i*iSizeX] = pP_comp[iCount + i * iSizeX] / fSum;
        // renormalisation
    }

    for (int i = 0; i < iNComponents; ++i) {
      pA_old[i] = m_pA[i];
      m_pA[i] = 0.0f;
      for (int iCount = 0; iCount < iSizeX; ++iCount) {
        m_pA[i] += pP_comp[iCount + i * iSizeX] * m_pSpectralProfile[iCount];
      }
    }

    // finish when already converged
    float fPrdist = 0.0f;
    for (int i = 0; i < iNComponents; ++i) {
      fPrdist += pow((m_pA[i] - pA_old[i]), 2);
    }
    fPrdist /= iNComponents;

    if (fPrdist < m_fParamPriorsConverged) {
      // LOG_INFO("Converged!");
      break;
    }
    // LOG_INFO("Didn't converge!");


    // update means (positions)
    for (int i = 0 ; i < iNComponents; ++i) {
      float mu_old = m_pMu[i];
      if (m_pA[i] > 0.0f) {
        m_pMu[i] = 0.0f;
        for (int iCount = 0; iCount < iSizeX; ++iCount) {
          m_pMu[i] += m_pSpectralProfile[iCount]
                      * pP_comp[iCount + i * iSizeX]
                      * static_cast<float>(iCount + 1);
        }
        m_pMu[i] /= m_pA[i];
        if (isnan(m_pMu[i])) {
          m_pMu[i] = mu_old;
        }
      }
    }
  }  // loop over iterations

  // Ensure they are sorted, using a really simple bubblesort
  bool bSorted = false;
  while (!bSorted) {
    bSorted = true;
    for (int i = 0; i < iNComponents - 1; ++i) {
      if (m_pMu[i] > m_pMu[i + 1]) {
        float fTemp = m_pMu[i];
        m_pMu[i] = m_pMu[i + 1];
        m_pMu[i + 1] = fTemp;
        fTemp = m_pA[i];
        m_pA[i] = m_pA[i + 1];
        m_pA[i + 1] = fTemp;
        bSorted = false;
      }
    }
  }
  return true;
}
}  // namespace aimc

