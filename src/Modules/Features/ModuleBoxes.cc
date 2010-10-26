// Copyright 2010, Google
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
 * \date created 2010-06-14
 * \version \$Id$
 */

#include "Modules/Features/ModuleBoxes.h"

namespace aimc {
ModuleBoxes::ModuleBoxes(Parameters *params) : Module(params) {
  module_description_ = "'Box-cutting' routine to generate dense features";
  module_identifier_ = "boxes";
  module_type_ = "features";
  module_version_ = "$Id$";

  box_size_spectral_ = parameters_->DefaultInt("boxes.spectral_size", 16);
  box_size_temporal_ = parameters_->DefaultInt("boxes.temporal_size", 32);
}

ModuleBoxes::~ModuleBoxes() {
}

bool ModuleBoxes::InitializeInternal(const SignalBank &input) {
  // Copy the parameters of the input signal bank into internal variables, so
  // that they can be checked later.
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();

  int channels_height = box_size_spectral_;
  while (channels_height < channel_count_ / 2) {
    int top = channel_count_ - 1;
    while (top - channels_height >= 0) {
      box_limits_channels_.push_back(std::make_pair(top,
                                                    top - channels_height));
      LOG_INFO("ch: t %d, b %d", top, top - channels_height);
      top -= channels_height / 2;
    }
    channels_height *= 2;
  }

  int temporal_width = box_size_temporal_;
  while (temporal_width < buffer_length_) {
    box_limits_time_.push_back(temporal_width);
    LOG_INFO("sp: %d", temporal_width);
    temporal_width *= 2;
  }

  box_count_ = box_limits_time_.size() * box_limits_channels_.size();
  feature_size_ = box_size_spectral_ + box_size_temporal_;
  LOG_INFO("Total box count is %d", box_count_);
  LOG_INFO("Total feature size is %d", feature_size_);

  output_.Initialize(box_count_, feature_size_, 1.0f);
  return true;
}

void ModuleBoxes::ResetInternal() {
}

void ModuleBoxes::Process(const SignalBank &input) {
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

  int box_index = 0;
  for (int c = 0; c < static_cast<int>(box_limits_channels_.size()); ++c) {
    for (int s = 0; s < static_cast<int>(box_limits_time_.size()); ++s) {
      int pixel_size_channels = (box_limits_channels_[c].first
                                 - box_limits_channels_[c].second)
                                   / box_size_spectral_;
      int pixel_size_samples = box_limits_time_[s] / box_size_temporal_;
      vector<vector<float> > box;
      vector<float> line;
      line.resize(box_size_temporal_, 0.0f);
      box.resize(box_size_spectral_, line);
      for (int i = 0; i < box_size_spectral_; ++i) {
        for (int j = 0; j < box_size_temporal_; ++j) {
          float pixel_value = 0.0f;
          for (int k = i * pixel_size_channels;
               k < (i + 1) * pixel_size_channels; ++k) {
            for (int l = j * pixel_size_samples;
                 l < (j + 1) * pixel_size_samples; ++l) {
              pixel_value += input.sample(k
                                          + box_limits_channels_[c].second, l);
            }
          }
          pixel_value /= (pixel_size_channels * pixel_size_samples);
          box[i][j] = pixel_value;
        }
      }
      int feature_index = 0;
      for (int i = 0; i < box_size_spectral_; ++i) {
        float feature_value = 0.0f;
        for (int j = 0; j < box_size_temporal_; ++j) {
          feature_value += box[i][j];
        }
        feature_value /= box_size_temporal_;
        output_.set_sample(box_index, feature_index, feature_value);
        ++feature_index;
      }
      for (int j = 0; j < box_size_temporal_; ++j) {
        float feature_value = 0.0f;
        for (int i = 0; i < box_size_spectral_; ++i) {
          feature_value += box[i][j];
        }
        feature_value /= box_size_spectral_;
        output_.set_sample(box_index, feature_index, feature_value);
        ++feature_index;
      }
      ++box_index;
    }
  }

  PushOutput();
}
}  // namespace aimc
