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

  inline const vector<float> &operator[](int channel) const {
    return signals_[channel];
  };

  float sample(int channel, int index) const;
  void set_sample(int channel, int index, float value);
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
