// Copyright 2006-2010, Willem van Engen, Thomas Walters
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
 *  \brief Audio file input
 *
 *  \author Willem van Engen <cnbh@willem.engen.nl>
 *  \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2006/09/21
 *  \version \$Id$
 */

#include <vector>

#include "Modules/Input/ModuleFileInput.h"

namespace aimc {
ModuleFileInput::ModuleFileInput(Parameters *params) : Module(params) {
  module_description_ = "File input using libsndfile";
  module_identifier_ = "file_input";
  module_type_ = "input";
  module_version_ = "$Id$";

  file_handle_ = NULL;
  buffer_length_ = parameters_->DefaultInt("input.buffersize", 1024);

  file_position_samples_ = 0;
  file_loaded_ = false;
  audio_channels_ = 0;
  sample_rate_ = 0.0f;
}

ModuleFileInput::~ModuleFileInput() {
  if (file_handle_ != NULL) {
    sf_close(file_handle_);
    file_handle_ = NULL;
  }
}

void ModuleFileInput::ResetInternal() {
  // If there's a file open. Close it.
  if (file_handle_ != NULL) {
    sf_close(file_handle_);
    file_handle_ = NULL;
  }
  // Open a file
  SF_INFO sfinfo;
  memset(reinterpret_cast<void*>(&sfinfo), 0, sizeof(SF_INFO));

  file_loaded_ = false;
  file_handle_ = sf_open(global_parameters_->GetString("input_filename"),
                         SFM_READ,
                         &sfinfo);

  if (file_handle_ == NULL) {
    /*! \todo Also display error reason
     */
    LOG_ERROR(_T("Couldn't read audio file '%s'"),
              global_parameters_->GetString("input_filename"));
    return;
  }
  
  file_loaded_ = true;
  done_ = false;
  audio_channels_ = sfinfo.channels;
  sample_rate_ = sfinfo.samplerate;
  file_position_samples_ = 0;

  if (audio_channels_ < 1 || buffer_length_ < 1 || sample_rate_ < 0.0f) {
    LOG_ERROR(_T("Problem with file: audio_channels = %d, buffer_length_ = %d, sample_rate = %f"), audio_channels_, buffer_length_, sample_rate_);
    return;
  }

  output_.Initialize(audio_channels_, buffer_length_, sample_rate_);
  output_.set_start_time(0);
}

bool ModuleFileInput::InitializeInternal(const SignalBank& input) {
  ResetInternal();
  return true;
}

void ModuleFileInput::Process(const SignalBank& input) {
  if (!file_loaded_)
    return;
  sf_count_t read;
  vector<float> buffer;
  buffer.resize(buffer_length_ * audio_channels_);

  // Read buffersize bytes into buffer
  read = sf_readf_float(file_handle_, &buffer[0], buffer_length_);
	
  // Place the contents of the buffer into the signal bank
  int counter = 0;
  for (int i = 0; i < read; ++i) {
	for (int c = 0; c < audio_channels_; ++c) {
      output_.set_sample(c, i, buffer[counter]);
      ++counter;
    }
  }

  // If the number of samples read is less than the buffer length, the end
  // of the file has been reached.
  if (read < buffer_length_) {
    // Zero samples at end
	for (int i = read; i < buffer_length_; ++i) {
	  for (int c = 0; c < audio_channels_; ++c) {
        output_.set_sample(c, i, 0.0f);
      }
    }
    // When we're past the end of the buffer, set the
    // module state to 'done' and exit.
    if (read == 0)
      done_ = true;
      return;
  }

  // Update time.
  output_.set_start_time(file_position_samples_);
  file_position_samples_ += read;
  PushOutput();
}
}  // namespace aimc
