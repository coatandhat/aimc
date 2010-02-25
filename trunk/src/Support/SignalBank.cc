// Copyright 2010, Thomas Walters
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
