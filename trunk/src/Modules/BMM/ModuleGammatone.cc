// Copyright 2009-2010, Thomas Walters
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
 *  \brief Slaney's gammatone filterbank
 *
 *  \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2009/11/13
 *  \version \$Id$
 */

#include <complex>

#include "Modules/BMM/ModuleGammatone.h"

namespace aimc {
using std::complex;
ModuleGammatone::ModuleGammatone(Parameters *params) : Module(params) {
  module_identifier_ = "gtfb";
  module_type_ = "bmm";
  module_description_ = "Gammatone filterbank (Slaney's IIR gammatone)";
  module_version_ = "$Id$";

  num_channels_ = parameters_->DefaultInt("gtfb.channel_count", 200);
  min_frequency_ = parameters_->DefaultFloat("gtfb.min_frequency", 86.0f);
  max_frequency_ = parameters_->DefaultFloat("gtfb.max_frequency", 16000.0f);
}

bool ModuleGammatone::InitializeInternal(const SignalBank& input) {
  // Calculate number of channels, and centre frequencies

  forward_.resize(num_channels_);
  feedback_.resize(num_channels_);

  for (int ch = 0; ch < num_channels_; ++ch) {
    float erb = Freq2ERBw(cf)

    // Sample interval
    float dt = 1.0f / fs;

    // Bandwidth parameter
    float b = 1.019f * 2.0f * M_PI * erb;

    // All of the following expressions are derived in Apple TR #35, "An 
    // Efficient Implementation of the Patterson-Holdsworth Cochlear 
    // Filter Bank".

    // Calculate the gain:
    complex<float> exponent(0.0f, 2.0f * cf * M_PI * T);
    complex<float> ec = exp(2.0f * complex_exponent);
    complex<float> two_cf_pi_t(2.0f * cf * M_PI * T, 0.0f);
    complex<float> two_pow(pow(2.0f, (3.0f/2.0f)), 0.0f);

    complex<float> p = -2.0f * ec * T + 2.0f * exp(-(B * T) + exponent) * dt;

    float gain = abs(
      (part1 * (cos(two_cf_pi_t) - sqrt(3.0f - twopow) * sin(two_cf_pi_t)))
      * (part1 * (cos(two_cf_pi_t) + sqrt(3.0f - twopow) * sin(two_cf_pi_t)))
      * (part1 * (cos(two_cf_pi_t) - sqrt(3.0f + twopow) * sin(two_cf_pi_t)))
      * (part1 * (cos(two_cf_pi_t) + sqrt(3.0f + twopow) * sin(two_cf_pi_t)))
      / pow(-2.0f / exp(2.0f * b * dt) - 2.0f * ec + 2.0f * (1.0f + ec)
            / exp(b * dt), 4.0f));

    // The filter coefficients themselves:
    forward[ch].resize(5, 0.0f);
    feedback[ch].resize(9, 0.0f);

    forward[ch][0] = pow(T, 4.0f) / gain;
    forward[ch][1] = (-4.0f * pow(T, 4.0f) * cos(2.0f * cf * M_PI * dt)
                      / exp(b * dt) / gain);
    forward[ch][2] = (6.0f * pow(T, 4.0f) * cos(4.0f * cf * M_PI * dt)
                      / exp(2.0f * b * dt) / gain);
    forward[ch][3] = (-4.0f * pow(T, 4.0f) * cos(6.0f * cf * M_PI * dt)
                      / exp(3.0f * b * dt) / gain);
    forward[ch][4] = (pow(T,4.0f) * cos(8.0f * cf * M_PI * dt)
                      / exp(4.0f * b * dt) / gain);

    feedback[ch][0] = 1.0f;
    feedback[ch][1] = -8.0f * cos(2.0f * cf * M_PI * T) / exp(b * dt);
    feedback[ch][2] = (4.0f * (4.0f + 3.0f * cos(4.0f * cf * M_PI * dt))
                       / exp(2.0f * b * dt));
    feedback[ch][3] = (-8.0f * (6.0f * cos(2.0f * cf * M_PI * dt)
                                + cos(6.0f * cf * M_PI * dt))
                       / exp(3.0f * b * dt));
    feedback[ch][4] = (2.0f * (18.0f + 16.0f * cos(4.0f * cf * M_PI * dt)
                               + cos(8.0f * cf * M_PI * dt))
                       / exp(4.0f * b * dt));
    feedback[ch][5] = (-8.0f * (6.0f * cos(2.0f * cf * M_PI * T)
                                + cos(6.0f * cf * M_PI * T))
                       / exp(5.0f * B * T));
    feedback[ch][6] = (4.0f * (4.0f + 3.0f * cos(4.0f * cf * M_PI * T))
                       / exp(6.0f * B * T));
    feedback[ch][7] = -8.0f * cos(2.0f * cf * M_PI * dt) / exp(7.0f * b * dt);
    feedback[ch][8] = exp(-8.0f * b * dt);
  }
}
} // namespace aimc