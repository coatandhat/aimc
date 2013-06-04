//
//  carfac_output.h
//  CARFAC Open Source C++ Library
//
//  Created by Alex Brandmeyer on 5/10/13.
//
// This C++ file is part of an implementation of Lyon's cochlear model:
// "Cascade of Asymmetric Resonators with Fast-Acting Compression"
// to supplement Lyon's upcoming book "Human and Machine Hearing"
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

#ifndef CARFAC_CARFAC_OUTPUT_H
#define CARFAC_CARFAC_OUTPUT_H

#include <deque>
#include <vector>

#include "common.h"
#include "ear.h"

// A CARFACOutput object can store up to 5 different types of output from a
// CARFAC model, and is provided as an argument to the CARFAC::RunSegment
// method.
class CARFACOutput {
 public:
  // The constructor takes five boolean values as arguments which indicate
  // the portions of the CARFAC model's output to be stored.
  CARFACOutput(const bool store_nap, const bool  store_nap_decim,
            const bool store_bm, const bool store_ohc, const bool store_agc);
  
  // The AppendOutput method is called on a sample by sample basis by the
  // CARFAC::RunSegemtn method, appending a single frame of n_ears x n_channels
  // data to the end of the individual data members selected for storage.
  void AppendOutput(const std::vector<Ear>& ears);
  const std::deque<std::vector<ArrayX>>& nap() const { return nap_; }
  const std::deque<std::vector<ArrayX>>& bm() const { return bm_; }
  const std::deque<std::vector<ArrayX>>& nap_decim() const {
    return nap_decim_; }
  const std::deque<std::vector<ArrayX>>& ohc() const { return ohc_; }
  const std::deque<std::vector<ArrayX>>& agc() const { return agc_; }

 private:
  // TODO (alexbrandmeyer): figure out why this breaks object initialization.
  //DISALLOW_COPY_AND_ASSIGN(CARFACOutput);
  bool store_nap_;
  bool store_nap_decim_;
  bool store_bm_;
  bool store_ohc_;
  bool store_agc_;
  
  // CARFAC outputs are stored in nested containers with dimensions:
  // n_frames x n_ears x n_channels.
  std::deque<std::vector<ArrayX>> nap_;
  std::deque<std::vector<ArrayX>> nap_decim_;
  std::deque<std::vector<ArrayX>> bm_;
  std::deque<std::vector<ArrayX>> ohc_;
  std::deque<std::vector<ArrayX>> agc_;
};

#endif  // CARFAC_CARFAC_OUTPUT_H