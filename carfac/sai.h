// Copyright 2013, Google, Inc.
// Author: Ron Weiss <ronw@google.com>
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

#ifndef CARFAC_SAI_H_
#define CARFAC_SAI_H_

#include <vector>

// TODO(ronw): Rename this file to common.h
#include "carfac_common.h"

// Design parameters for a single SAI.
struct SAIParams {
  // Number of channels (height) of the SAI.
  int n_ch;

  // TODO(ronw): Consider parameterizing this as past_lags and
  // future_lags, with width == past_lags + 1 + future_lags.
  //
  // Total width (i.e. number of lag samples) of the SAI.
  int width;
  // Number of lag samples that should come from the future.
  int future_lags;
  // Number of windows (triggers) to consider during each SAI frame.
  int n_window_pos;

  // TODO(ronw): more carefully define terms "window" and "frame"

  // Size of the window to compute.
  int window_width;

  FPType channel_smoothing_scale;
};

class SAI {
 public:
  explicit SAI(const SAIParams& params);

  // Fill output_frame with a params_.n_ch by params_.width SAI frame
  // computed from the given input frames.
  //
  // The input should have dimensionality of params_.window_width by
  // params_.n_ch.  Inputs containing too few frames are zero-padded.
  // FIXME: Float2DArray input type would be less awkward.
  void RunSegment(const std::vector<FloatArray>& input,
                  Float2dArray* output_output_frame);

 private:
  // Process successive windows within input_buffer, choose trigger
  // points, and blend each window into output_buffer.
  void StabilizeSegment(const Float2dArray& input_buffer,
                        Float2dArray* output_buffer) const;

  SAIParams params_;
  // Window function to apply before selecting a trigger point.
  // Size: params_.window_width.
  FloatArray window_;
  // Buffer to store a large enough window of input frames to compute
  // a full SAI frame.  Size: params_.n_ch by params_.buffer_width.
  Float2dArray input_buffer_;
  // Output frame buffer.  Size: params_.n_ch by params_.width.
  Float2dArray output_buffer_;
};

#endif  // CARFAC_SAI_H_
