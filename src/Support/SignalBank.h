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
 *  \brief Basic data structure for a bank of signals with a common sample
 *  rate (in Hz), a centre frequency (in Hz) for each channel, a start time
 *  for the signal (in samples) and optionally, a set of strobe points in each
 *  channel (a vector of the indices of certain points). This is a struct
 *  rather than a class, and data members are accessed directly. Therefore is
 *  is up to the users of this structure to use it properly. Never assume that
 *  a SignalBank will be set up correctly when you receive it. A basic
 *  constructor and initialisation routines are provided.
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \date 2010/01/23
 *  \version \$Id$
 */

#ifndef AIMC_SUPPORT_SIGNALBANK_H_
#define AIMC_SUPPORT_SIGNALBANK_H_

#include <deque>
#include <vector>

#include "Support/Common.h"

namespace aimc {
using std::deque;
using std::vector;
class SignalBank {
 public:
  SignalBank();
  ~SignalBank();
  bool Initialize(int channel_count, int signal_length, float sample_rate);

  /* \brief Initialize the signal bank with the same parameters, buffer size
   * and centre frequencies as the input signal bank
   */
  bool Initialize(const SignalBank &input);
  bool Validate() const;

  // Return a const reference to an individual signal. Allows for 
  // signal[channel][sample] referencing of SignalBanks.
  inline const vector<float> &operator[](int channel) const {
    return signals_[channel];
  };

  // Return a const reference to an individual signal.
  inline const vector<float> &get_signal(int channel) const {
    return signals_[channel];
  };
  
  inline const vector<int>& get_strobes(int channel) const {
    //DCHECK(channel > 0);
    //DCHECK(channel < strobes.size());
    return strobes_[channel];
  };

  // Return a reference to the signal vector. The reference is not
  // const, so the vector is directly modifiable. In order to maintain
  // consistency of the data within the filterbank, the size of this
  // vector should not be changed. Changes to the vector size can be picked
  // up with a call to Validate(), which will return false if the sizes of
  // channle vectors are not consistent.
  inline vector<float> &get_mutable_signal(int channel) {
    return signals_[channel];
  };

  inline void set_signal(int channel, vector<float> input) {
    signals_[channel] = input;
  }

  inline float sample(int channel, int index) const {
    return signals_[channel][index];
  }

  inline void set_sample(int channel, int index, float value) {
    signals_[channel][index] = value;
  }

  int strobe(int channel, int index) const;
  int strobe_count(int channel) const;
  void AddStrobe(int channel, int time);
  void ResetStrobes(int channel);
  float sample_rate() const;
  int buffer_length() const;
  int start_time() const;
  void set_start_time(int start_time);
  float centre_frequency(int i) const;
  void set_centre_frequency(int i, float cf);
  bool initialized() const;
  int channel_count() const;
  void Clear();
 private:
  int channel_count_;
  int buffer_length_;
  vector<vector<float> > signals_;
  vector<vector<int> > strobes_;
  vector<float> centre_frequencies_;
  float sample_rate_;
  int start_time_;
  bool initialized_;
  DISALLOW_COPY_AND_ASSIGN(SignalBank);
};
}

#endif  // AIMC_SUPPORT_SIGNALBANK_H_
