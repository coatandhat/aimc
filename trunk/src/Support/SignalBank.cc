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

/*! \file
 *  \brief
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \date 2010/01/23
 *  \version \$Id$
 */

#include "Support/SignalBank.h"

namespace aimc {
using std::deque;
using std::vector;

SignalBank::SignalBank() {
  sample_rate_ = 0.0f;
  start_time_ = 0;
  channel_count_ = 0;
  buffer_length_ = 0;
  initialized_ = false;
}

SignalBank::~SignalBank() {
}

bool SignalBank::Initialize(int channel_count,
                            int signal_length,
                            float sample_rate) {
  if (channel_count < 1)
    return false;
  if (signal_length < 1)
    return false;
  if (sample_rate < 0.0f)
    return false;

  start_time_ = 0;
  sample_rate_ = sample_rate;
  buffer_length_ = signal_length;
  channel_count_ = channel_count;
  signals_.resize(channel_count_);
  strobes_.resize(channel_count_);
  centre_frequencies_.resize(channel_count_, 0.0f);
  for (int i = 0; i < channel_count_; ++i) {
    signals_[i].resize(buffer_length_, 0.0f);
  }
  initialized_ = true;
  return true;
}

bool SignalBank::Initialize(const SignalBank &input) {
  if (input.channel_count() < 1)
    return false;
  if (input.buffer_length() < 1)
    return false;
  if (input.sample_rate() < 0.0f)
    return false;

  start_time_ = input.start_time();
  sample_rate_ = input.sample_rate();
  buffer_length_ = input.buffer_length();
  channel_count_ = input.channel_count();

  signals_.resize(channel_count_);
  strobes_.resize(channel_count_);

  centre_frequencies_.resize(channel_count_, 0.0f);
  for (int i = 0; i < channel_count_; ++i) {
    centre_frequencies_[i] = input.centre_frequency(i);
  }

  for (int i = 0; i < channel_count_; ++i) {
    signals_[i].resize(buffer_length_, 0.0f);
    strobes_[i].resize(0);
  }
  initialized_ = true;
  return true;
}

void SignalBank::Clear() {
  for (int i = 0; i < channel_count_; ++i) {
    signals_[i].assign(buffer_length_, 0.0f);
    strobes_[i].resize(0);
  }
}

bool SignalBank::Validate() const {
  if (sample_rate_ <= 0.0f)
    return false;

  if (static_cast<int>(signals_.size()) < 1)
    return false;

  if (static_cast<int>(signals_.size()) != channel_count_)
    return false;

  if (static_cast<int>(strobes_.size()) != channel_count_)
    return false;

  for (int i = 0; i < channel_count_; ++i) {
    if (static_cast<int>(signals_[i].size()) != buffer_length_)
      return false;
  }
  return true;
}

int SignalBank::strobe(int channel, int index) const {
  return strobes_[channel][index];
}

int SignalBank::strobe_count(int channel) const {
  return strobes_[channel].size();
}

void SignalBank::AddStrobe(int channel, int time) {
  strobes_[channel].push_back(time);
}

void SignalBank::ResetStrobes(int channel) {
  strobes_[channel].resize(0);
}

float SignalBank::sample_rate() const {
  return sample_rate_;
}

int SignalBank::buffer_length() const {
  return buffer_length_;
}

int SignalBank::start_time() const {
  return start_time_;
}

void SignalBank::set_start_time(int start_time) {
  start_time_ = start_time;
}

float SignalBank::centre_frequency(int i) const {
  if (i < channel_count_)
    return centre_frequencies_[i];
  else
    return 0.0f;
}

void SignalBank::set_centre_frequency(int i, float cf) {
  if (i < channel_count_)
    centre_frequencies_[i] = cf;
}

bool SignalBank::initialized() const {
  return initialized_;
}

int SignalBank::channel_count() const {
  return channel_count_;
}
}
