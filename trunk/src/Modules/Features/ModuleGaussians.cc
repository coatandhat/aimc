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

/*! \file
 *  \brief Gaussian features - based on MATLAB code by Christian Feldbauer
 */

/*!
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2008/06/23
 * \version \$Id$
 */

#include <math.h>

#ifdef _WINDOWS
#include <float.h>
#endif

#include "Modules/Features/ModuleGaussians.h"
#include "Support/Common.h"

namespace aimc {
ModuleGaussians::ModuleGaussians(Parameters *params) : Module(params) {
  // Set module metadata
  module_description_ = "Gaussian Fitting to SSI profile";
  module_identifier_ = "gaussians";
  module_type_ = "features";
  module_version_ = "$Id$";

  m_iParamNComp = parameters_->DefaultInt("gaussians.ncomp", 4);
  m_fParamVar = parameters_->DefaultFloat("gaussians.var", 115.0);
  m_fParamPosteriorExp = parameters_->DefaultFloat("gaussians.posterior_exp",
                                                   6.0);
  m_iParamMaxIt = parameters_->DefaultInt("gaussians.maxit", 250);

  // The parameters system doesn't support tiny numbers well, to define this
  // variable as a string, then convert it to a float afterwards
  parameters_->DefaultString("gaussians.priors_converged", "1e-7");
  priors_converged_ = parameters_->GetFloat("gaussians.priors_converged");
  output_positions_ = parameters_->DefaultBool("gaussians.positions", false);
}

ModuleGaussians::~ModuleGaussians() {
}

bool ModuleGaussians::InitializeInternal(const SignalBank &input) {
  m_pA.resize(m_iParamNComp, 0.0f);
  m_pMu.resize(m_iParamNComp, 0.0f);

  // Assuming the number of channels is greater than twice the number of
  // Gaussian components, this is ok
  output_component_count_ = 1; // Energy component
  if (input.channel_count() >= 2 * m_iParamNComp) {
    output_component_count_ += (m_iParamNComp - 1);
  } else {
    LOG_ERROR(_T("Too few channels in filterbank to produce sensible "
                 "Gaussian features. Either increase the number of filterbank"
                 " channels, or decrease the number of Gaussian components"));
    return false;
  }

  if (output_positions_) {
    output_component_count_ += m_iParamNComp;
  }

  output_.Initialize(output_component_count_, 1, input.sample_rate());

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
  output_.set_start_time(input.start_time());
  // Calculate spectral profile
  for (int ch = 0; ch < input.channel_count(); ++ch) {
    m_pSpectralProfile[ch] = 0.0f;
    for (int i = 0; i < input.buffer_length(); ++i) {
      m_pSpectralProfile[ch] += input[ch][i];
    }
    m_pSpectralProfile[ch] /= static_cast<float>(input.buffer_length());
  }

  float spectral_profile_sum = 0.0f;
  for (int i = 0; i < input.channel_count(); ++i) {
    spectral_profile_sum += m_pSpectralProfile[i];
  }

  // Set the last component of the feature vector to be the log energy
  float logsum = log(spectral_profile_sum);
  if (!isinf(logsum)) {
    output_.set_sample(output_component_count_ - 1, 0, logsum);
  } else {
    output_.set_sample(output_component_count_ - 1, 0, -1000.0);
  }

  for (int ch = 0; ch < input.channel_count(); ++ch) {
    m_pSpectralProfile[ch] = pow(m_pSpectralProfile[ch], 0.8f);
  }

  RubberGMMCore(2, true);

  float mean1 = m_pMu[0];
  float mean2 = m_pMu[1];
  // LOG_INFO(_T("Orig. mean 0 = %f"), m_pMu[0]);
  // LOG_INFO(_T("Orig. mean 1 = %f"), m_pMu[1]);
  // LOG_INFO(_T("Orig. prob 0 = %f"), m_pA[0]);
  // LOG_INFO(_T("Orig. prob 1 = %f"), m_pA[1]);

  float a1 = 0.05 * m_pA[0];
  float a2 = 1.0 - 0.25 * m_pA[1];

  // LOG_INFO(_T("fA1 = %f"), fA1);
  // LOG_INFO(_T("fA2 = %f"), fA2);

  float gradient = (mean2 - mean1) / (a2 - a1);
  float intercept = mean2 - gradient * a2;

  // LOG_INFO(_T("fGradient = %f"), fGradient);
  // LOG_INFO(_T("fIntercept = %f"), fIntercept);

  for (int i = 0; i < m_iParamNComp; ++i) {
    m_pMu[i] = (static_cast<float>(i)
                / (static_cast<float>(m_iParamNComp) - 1.0f))
                * gradient + intercept;
                // LOG_INFO(_T("mean %d = %f"), i, m_pMu[i]);
  }

  for (int i = 0; i < m_iParamNComp; ++i) {
    m_pA[i] = 1.0f / static_cast<float>(m_iParamNComp);
  }

  RubberGMMCore(m_iParamNComp, false);

  // Amplitudes first
  for (int i = 0; i < m_iParamNComp - 1; ++i) {
    if (!isnan(m_pA[i])) {
      output_.set_sample(i, 0, m_pA[i]);
    } else {
      output_.set_sample(i, 0, 0.0f);
    }
  }

  // Then means if required
  if (output_positions_) {
    int idx = 0;
    for (int i = m_iParamNComp - 1; i < 2 * m_iParamNComp - 1; ++i) {
      if (!isnan(m_pMu[i])) {
        output_.set_sample(i, 0, m_pMu[idx]);
      } else {
        output_.set_sample(i, 0, 0.0f);
      }
      ++idx;
    }
  }

  PushOutput();
}

bool ModuleGaussians::RubberGMMCore(int iNComponents, bool bDoInit) {
  int iSizeX = m_iNumChannels;

  // Normalise the spectral profile
  float SpectralProfileTotal = 0.0f;
  for (int iCount = 0; iCount < iSizeX; iCount++) {
    SpectralProfileTotal += m_pSpectralProfile[iCount];
  }
  for (int iCount = 0; iCount < iSizeX; iCount++) {
    m_pSpectralProfile[iCount] /= SpectralProfileTotal;
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

    for (int c = 0; c < iNComponents; c++) {
      for (int iCount = 0; iCount < iSizeX; iCount++) {
        pP_mod_X[iCount] += 1.0f / sqrt(2.0f * M_PI * m_fParamVar)
                            * exp((-0.5f)
                            * pow(static_cast<float>(iCount+1) - m_pMu[c], 2)
                            / m_fParamVar) * m_pA[c];
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

    if (fPrdist < priors_converged_) {
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

