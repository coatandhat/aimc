// Copyright 2010, Thomas Walters
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

/*!
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2010/02/19
 * \version \$Id$
 */

#include <cmath>

#include "Modules/SSI/ModuleSSI.h"

namespace aimc {
#ifdef _MSC_VER
// MSVC doesn't define log2()
float log2(float n) {
  return log(n) / log(2.0);  
}
#endif

ModuleSSI::ModuleSSI(Parameters *params) : Module(params) {
  module_description_ = "Size-shape image (aka the 'sscAI')";
  module_identifier_ = "ssi";
  module_type_ = "ssi";
  module_version_ = "$Id$";

  // Cut off the SSI at the end of the first cycle
  do_pitch_cutoff_ = parameters_->DefaultBool("ssi.pitch_cutoff", false);

  // Weight the values in each channel more strongly if the channel was
  // truncated due to the pitch cutoff. This ensures that the same amount of
  // energy remains in the SSI spectral profile
  weight_by_cutoff_ = parameters_->DefaultBool("ssi.weight_by_cutoff", false);

  // Weight the values in each channel more strongly if the channel was
  // scaled such that the end goes off the edge of the computed SSI.
  // Again, this ensures that the overall energy of the spectral profile
  // remains the same.
  weight_by_scaling_ = parameters_->DefaultBool("ssi.weight_by_scaling",
                                                false);

  // Time from the zero-lag line of the SAI from which to start searching
  // for a maximum in the input SAI's temporal profile.
  pitch_search_start_ms_ = parameters_->DefaultFloat(
      "ssi.pitch_search_start_ms", 2.0f);

  // Total width in cycles of the whole SSI
  ssi_width_cycles_ = parameters_->DefaultFloat("ssi.width_cycles", 10.0f);

  // Set to true to make the cycles axis logarithmic (ie indexing by gamma
  // rather than by cycles) 
  log_cycles_axis_ = parameters_->DefaultBool("ssi.log_cycles_axis", true);

  // The centre frequency of the channel which will just fill the complete
  // width of the SSI buffer
  pivot_cf_ = parameters_->DefaultFloat("ssi.pivot_cf", 1000.0f);
  
  // Whether or not to do smooth offset when the pitch cutoff is active.
  do_smooth_offset_ = parameters_->DefaultBool("ssi.do_smooth_offset", false);
  
  // The number of cycles, centered on the pitch line, over which the SSI is taken
  // to zero when doing the pitch cutoff.
  smooth_offset_cycles_ = parameters_->DefaultFloat("ssi.smooth_offset_cycles", 3.0f);
}

ModuleSSI::~ModuleSSI() {
}

bool ModuleSSI::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();

  ssi_width_samples_ = sample_rate_ * ssi_width_cycles_ / pivot_cf_;
  if (ssi_width_samples_ > buffer_length_) {
    ssi_width_samples_ = buffer_length_;
    float cycles = ssi_width_samples_ * pivot_cf_ / sample_rate_;
    LOG_INFO(_T("Requested SSI width of %f cycles is too long for the "
                "input buffer length of %d samples. The SSI will be "
                "truncated at %d samples wide. This corresponds to a width "
                "of %f cycles."), ssi_width_cycles_, buffer_length_,
                ssi_width_samples_, cycles);
    ssi_width_cycles_ = cycles;
  }
  for (int i = 0; i < input.channel_count(); ++i) {
    output_.set_centre_frequency(i, input.centre_frequency(i));
  }
  
  h_.resize(ssi_width_samples_, 0.0);
  float gamma_min = -1.0f;
  float gamma_max = log2(ssi_width_cycles_);
  for (int i = 0; i < ssi_width_samples_; ++i) {
    if (log_cycles_axis_) {
      float gamma = gamma_min + (gamma_max - gamma_min)
                                 * static_cast<float>(i)
                                 / static_cast<float>(ssi_width_samples_);
      h_[i]= pow(2.0f, gamma);
    } else {
      h_[i] = static_cast<float>(i) * ssi_width_cycles_
              / static_cast<float>(ssi_width_samples_);
    }
  }
  
  output_.Initialize(channel_count_, ssi_width_samples_, sample_rate_);
  return true;
}

void ModuleSSI::ResetInternal() {
}

int ModuleSSI::ExtractPitchIndex(const SignalBank &input) const {
  // Generate temporal profile of the SAI
  vector<float> sai_temporal_profile(buffer_length_, 0.0f);
  for (int i = 0; i < buffer_length_; ++i) {
    float val = 0.0f;
    for (int ch = 0; ch < channel_count_; ++ch) {
      val += input.sample(ch, i);
    }
    sai_temporal_profile[i] = val;
  }

  // Find pitch value
  int start_sample = floor(pitch_search_start_ms_ * sample_rate_ / 1000.0f);
  int max_idx = 0;
  float max_val = 0.0f;
  for (int i = start_sample; i < buffer_length_; ++i) {
    if (sai_temporal_profile[i] > max_val) {
      max_idx = i;
      max_val = sai_temporal_profile[i];
    }
  }
  return max_idx;
}

void ModuleSSI::Process(const SignalBank &input) {
  // Check to see if the module has been initialized. If not, processing
  // should not continue.
  if (!initialized_) {
    LOG_ERROR(_T("Module %s not initialized."), module_identifier_.c_str());
    return;
  }

  // Check that ths input this time is the same as the input passed to
  // Initialize()
  if (buffer_length_ != input.buffer_length()
      || channel_count_ != input.channel_count()) {
    LOG_ERROR(_T("Mismatch between input to Initialize() and input to "
                 "Process() in module %s."), module_identifier_.c_str());
    return;
  }

  output_.set_start_time(input.start_time());

  int pitch_index = buffer_length_ - 1;
  if (do_pitch_cutoff_) {
    pitch_index = ExtractPitchIndex(input);
  }

  for (int ch = 0; ch < channel_count_; ++ch) {
    float centre_frequency = input.centre_frequency(ch);
    float cycle_samples = sample_rate_ / centre_frequency;
    
    float channel_weight = 1.0f;
    int cutoff_index = buffer_length_ - 1;
    if (do_pitch_cutoff_) {
      if (pitch_index < cutoff_index) {
        if (weight_by_cutoff_) {
          channel_weight = static_cast<float>(buffer_length_)
                           / static_cast<float>(pitch_index);
        }
        cutoff_index = pitch_index;
      }
    }
    
    // tanh(3) is about 0.995. Seems reasonable. 
    float smooth_pitch_constant = 3.0f / smooth_offset_cycles_;
    float pitch_h = 0.0f;
    if (do_smooth_offset_) {
      pitch_h = static_cast<float>(pitch_index) / cycle_samples;
    }
    
    // Copy the buffer from input to output, addressing by h-value.
    for (int i = 0; i < ssi_width_samples_; ++i) {
      
      // The index into the input array is a floating-point number, which is
      // split into a whole part and a fractional part. The whole part and
      // fractional part are found, and are used to linearly interpolate
      // between input samples to yield an output sample.
      double whole_part;
      float frac_part = modf(h_[i] * cycle_samples, &whole_part);
      int sample = floor(whole_part);

      float weight = channel_weight;
      
      if (do_smooth_offset_ && do_pitch_cutoff_) {
        // Smoothing around the pitch cutoff line.
        float pitch_weight = (1.0f + tanh((pitch_h - h_[i])
                                          * smooth_pitch_constant)) / 2.0f;
        weight *= pitch_weight;
        //LOG_INFO("Channel %d, Sample %d. Pitch weight: %f", ch, i, pitch_weight);
      }

      if (weight_by_scaling_) {
        if (centre_frequency > pivot_cf_) {
          weight *= (centre_frequency / pivot_cf_);
        }
      }

      float val;
      if (sample < cutoff_index || do_smooth_offset_) {
        float curr_sample = input.sample(ch, sample);
        float next_sample = input.sample(ch, sample + 1);
        val = weight * (curr_sample
                        + frac_part * (next_sample - curr_sample));
      } else {
        val = 0.0f;
      }
      output_.set_sample(ch, i, val);
    }
  }
  PushOutput();
}
}  // namespace aimc

