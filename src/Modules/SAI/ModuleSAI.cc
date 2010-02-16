// Copyright 2006-2010, Thomas Walters
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
 *  \brief SAI module
 */

/*
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2007/08/29
 * \version \$Id: ModuleSAI.cc 4 2010-02-03 18:44:58Z tcw $
 */
#include <math.h>

#include "Modules/SAI/ModuleSAI.h"

ModuleSAI::ModuleSAI(Parameters *parameters) : Module(parameters) {
  module_description_ = "Stabilised auditory image";
  module_name_ = "sai2007";
  module_type_ = "sai";
  module_id_ = "$Id: ModuleSAI.cc 4 2010-02-03 18:44:58Z tcw $";

  min_delay_ms_ = parameters_->DefaultFloat("sai.min_delay_ms", 0.0f);
  max_delay_ms_ = parameters_->DefaultFloat("sai.max_delay_ms", 35.0f);
  strobe_weight_alpha_ = parameters_->DefaultFloat("sai.strobe_weight_alpha",
                                                   0.5f);
  buffer_memory_decay_ = parameters_->DefaultFloat("sai.buffer_memory_decay",
                                                   0.03f);
  frame_period_ms_ = parameters_->DefaultFloat("sai.frame_period_ms", 20.0f);

  min_strobe_delay_index_ = 0;
  max_strobe_delay_index_ = 0;
  sai_decay_factor_ = 0.0f;
  max_concurrent_strobes_ = 0;
  output_frame_period_ms_ = 0.0f;
  last_output_frame_time_ms_ = 0.0f;
}

bool ModuleSAI::InitializeInternal(const SignalBank &input) {
  // The SAI output bank must be as long as the SAI's Maximum delay.
  // One sample is added to the SAI buffer length to account for the
  // zero-lag point
  int sai_buffer_length = 1 + Round(input.sample_rate() * max_delay_ms_);

  // Make an output SignalBank with the same number of channels and centre
  // frequencies as the input, but with a different buffer length
  if (!output_.Initialize(input.channel_count(),
                          sai_buffer_length,
                          input.sample_rate());) {
    LOG_ERROR("Failed to create output buffer in SAI module");
    return false;
  }
  for (int i = 0; i < input.channel_count(); ++i ) {
    output_.set_centre_frequency(i, input.get_centre_frequency(i));
  }

  // sai_temp_ will be initialized to zero
  if (!sai_temp_.Initialize(output_)) {
    LOG_ERROR("Failed to create temporary buffer in SAI module");
    return false;
  }

  last_output_time_ms_ = 0.0f;

  frame_period_samples_ = Round(input.sample_rate()
                                * frame_period_ms_ / 1000.0f);

  min_strobe_delay_idx_ = Round(input.sample_rate() * min_delay_ms_ / 1000.0f);
  max_strobe_delay_idx_ = Round(input.sample_rate() * max_delay_ms_ / 1000.0f);

  // Make sure we don't go past the output buffer's upper bound
  if (max_strobe_delay_idx_ > output_.buffer_length()))
    max_strobe_delay_idx_ = output_.buffer_length();

  // Define decay factor from time since last sample (see ti2003)
  sai_decay_factor_ = pow(0.5f, 1.0f / (buffer_memory_decay_
                                        * input.sample_rate()));

  // Maximum strobes that can be active at the same time within maxdelay.
  //! \todo Choose this value in a more principled way
  max_concurrent_strobes_ = Round(1000.0f * max_delay_ * 5);

  // Precompute strobe weights
  strobe_weights_.resize(max_concurrent_strobes_);
  for (int n = 0; n < max_concurrent_strobes_; ++n) {
    strobe_weights_[n] = pow(1.0f / (n + 1)), strobe_weight_alpha_);
  }

  // Active Strobes
  active_strobes_.Resize(input.channel_count());
  for (int i = 0; i < input.channel_count(); ++i) {
    active_strobes_[i].Create(max_concurrent_strobes_);
  }
  next_strobes_.resize(input.channel_count(), 0);

  return true;
}

void ModuleSAI::ResetInternal() {
}

void ModuleSAI::Process(const SignalBank &input) {
  int s;
  int c;
  int output_buffer_length = output_.buffer_length();

  // Reset the next strobe times
  next_strobes_.clear();
  next_strobes_.resize(output_.channel_count(), 0);

  // Offset the times on the strobes from the previous buffer
  for (c = 0; c < input.channel_count(), ++c) {
    active_strobes_[c].shiftStrobes(input.buffer_length());
  }

  // Make sure only start time is transferred to the output
  output_.set_start_time(input.start_time());

  // Loop over samples to make the SAI
  for (s = 0; s < input_buffer_length; ++s) {
    float decay_factor = pow(sai_decay_factor_, fire_counter_);
    // Loop over channels
    for (c = 0; c < input.channel_count(); ++c) {
      // Local convenience variables
      StrobeList &active_strobes = active_strobes_[c];
      float centre_frequency = input.get_centre_frequency(c);
      int next_strobe = next_strobes_[c];

      // 1. Update strobes
      // If we are up to or beyond the next strobe...
      if (next_strobe < input.strobe_count(c)) {
        if (s == pSigIn->getStrobe(iNextStrobe)) {
          //A new strobe has arrived
          // if there aren't already too many strobes active...
          if ((active_strobes.getStrobeCount() + 1) < max_concurrent_strobes_) {
            // ...add the active strobe to the list of current strobes
            // calculate the strobe weight
            float weight = 1.0f;
            if (active_strobes.getStrobeCount() > 0) {
              int last_strobe = active_strobes.getTime(
                active_strobes.getStrobeCount());

              // If the strobe occured within 10 impulse-response
              // cycles of the previous strobe, then lower its weight
              weight = (s - iLastStrobe) / input.sample_rate()
                       * centre_frequency / 10.0f;
              if (weight > 1.0f)
                weight = 1.0f;
            }
            pActiveStrobes->addStrobe(iCurrentSample, weight);
            iNextStrobe++;
          } else {
            // We have a problem
            aimASSERT(0);
          }

          // 2. Having updated the strobes, we now need to update the
          // strobe weights
          float total_strobe_weight = 0.0f;
          for (int si = 1; si <= pActiveStrobes->getStrobeCount(); ++si) {
            total_strobe_weight += (pActiveStrobes->getWeight(si) 
              * m_pStrobeWeights[pActiveStrobes->getStrobeCount() - si]);
          }
          for (int si = 1; si <= pActiveStrobes->getStrobeCount(); ++si) {
            active_strobes.setWorkingWeight(si,(active_strobes.getWeight(si)
              * strobe_weights_[active_strobes.getStrobeCount() - si])
                / total_strobe_weight);
          }
        }
      }

      // remove inactive strobes...
      while (pActiveStrobes->getStrobeCount() > 0) {
        // Get the time of the first strobe (ordering of strobes is
        // from one, not zero)
        int iStrobeTime = pActiveStrobes->getTime(1);
        int iDelay = iCurrentSample - iStrobeTime;
        // ... do we now need to remove this strobe?
        if (iDelay > m_maxStrobeDelayIdx)
          pActiveStrobes->deleteFirstStrobe();
        else
          break;
          // Since the strobes are ordered, we don't need to go
          // beyond the first still-active strobe
      }

      // 3. Loop over active strobes
      for (int si = 1; si <= pActiveStrobes->getStrobeCount(); si++) {
        // 3.1 Add effect of active strobe at correct place in the SAI buffer
        // Calculate the time from the strobe event to 'now': iDelay
        int iStrobeTime = pActiveStrobes->getTime(si);
        int iDelay = iCurrentSample - iStrobeTime;

        // If the delay is greater than the (user-set)
        // minimum strobe delay, the strobe can be used
        if (iDelay >= m_minStrobeDelayIdx && iDelay < m_maxStrobeDelayIdx) {
          // The value at be added to the SAI
          float sig = pSigIn->getSample(iCurrentSample, audCh);

          // Weight the sample correctly
          sig *= pActiveStrobes->getWorkingWeight(si);

          // Adjust the weight acording to the number of samples until the
          // next output frame
          sig *= fDecayFactor;

          // Update the temporary SAI buffer
          pSigOut->setSample(iDelay, audCh,
                             pSigOut->getSample(iDelay, audCh)+sig);
        }
      }

      m_pNextStrobes[bankCh]=iNextStrobe;

    } // End loop over channels


    //Check to see if we need to output an SAI frame this sample
    if (m_iFireCounter-- == 0) {
      // Decay the SAI by the correct amount and add the current output frame
      float decay = pow(sai_decay_factor_, fire_period_samples_);

      for (c = 0; c < input.channel_count(); ++c) {
        for (int i = 0; i < output_.buffer_length(); ++i) {
          output_.set_sample(c, i, sai_temp_[c][i] + output_[c][i] * decay);
        }
      }

      // Zero the temporary signal
      for (int ch = 0; ch < sai_temp_.channel_count(); ++ch) {
        for (int i = 0; i < sai_temp_.buffer_length(); ++i) {
          sai_temp_.set_sample(ch, i, 0.0f);
        }
      }

      m_iFireCounter=m_iFirePeriodSamples-1;

      // Make sure the start time is transferred to the output
      m_pOutputData->setStartTime(m_pInputData->getSignal(0)->getStartTime()+(SIGNAL_SAMPLE)((float)iCurrentSample*1000.0f/(float)m_pInputData->getSamplerate()));
      PushOutput();
    }
  } // End loop over samples
}

ModuleSAI::~ModuleSAI() {
}
