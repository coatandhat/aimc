// Copyright 2006-2010, Willem van Engen, Thomas Walters
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
  output_.Initialize(audio_channels_, buffer_length_, sample_rate_);
  output_.set_start_time(0);
}

bool ModuleFileInput::LoadFile(const char* filename) {
  // If there's a file open. Close it.
  if (file_handle_ != NULL) {
    sf_close(file_handle_);
    file_handle_ = NULL;
  }
  // Open the file
  SF_INFO sfinfo;
  memset(reinterpret_cast<void*>(&sfinfo), 0, sizeof(SF_INFO));

  file_handle_ = sf_open(filename, SFM_READ, &sfinfo);

  if (file_handle_ == NULL) {
    /*! \todo Also display error reason
     */
    LOG_ERROR(_T("Couldn't read audio file '%s'"), filename);
    return false;
  }

  file_loaded_ = true;
  audio_channels_ = sfinfo.channels;
  sample_rate_ = sfinfo.samplerate;
  file_position_samples_ = 0;

  // A dummy signal bank to be passed to the Initialize() function.
  SignalBank s;
  s.Initialize(1, 1, 1);

  // Self-initialize by calling Module::Initialize() explicitly.
  // The Initialize() call in this subclass is overloaded to prevent it from
  // being called drectly.
  return Module::Initialize(s);
}


/* Do not call Initialize() on ModuleFileInput directly
 * instead call LoadFile() with a filename to load.
 * This will automatically initialize the module.
 */
bool ModuleFileInput::Initialize(const SignalBank& input) {
  LOG_ERROR(_T("Do not call Initialize() on ModuleFileInput directly "
               "instead call LoadFile() with a filename to load. "
               "This will automatically initialize the module."));
  return false;
}

void ModuleFileInput::Process(const SignalBank& input) {
  LOG_ERROR(_T("Call Process() on ModuleFileInput instead of passing in "
               "a SignalBank"));
}

bool ModuleFileInput::InitializeInternal(const SignalBank& input) {
  if (!file_loaded_) {
    LOG_ERROR(_T("No file loaded in FileOutputHTK"));
    return false;
  }
  if (audio_channels_ < 1 || buffer_length_ < 1 || sample_rate_ < 0.0f) {
    LOG_ERROR(_T("audio_channels, buffer_length_ or sample_rate too small"));
    return false;
  }
  ResetInternal();
  return true;
}

void ModuleFileInput::Process() {
  if (!file_loaded_)
    return;
  sf_count_t read;
  vector<float> buffer;
  buffer.resize(buffer_length_ * audio_channels_);

  while (true) {
    // Read buffersize bytes into buffer
    read = sf_readf_float(file_handle_, &buffer[0], buffer_length_);

    // Place the contents of the buffer into the signal bank
    int counter = 0;
    for (int c = 0; c < audio_channels_; ++c) {
      for (int i = 0; i < read; ++i) {
        output_.set_sample(c, i, buffer[counter]);
        ++counter;
      }
    }

    // If the number of saples read is less than the buffer length, the end
    // of the file has been reached.
    if (read < buffer_length_) {
      // Zero samples at end
      for (int c = 0; c < audio_channels_; ++c) {
        for (int i = read; i < buffer_length_; ++i) {
          output_.set_sample(c, i, 0.0f);
        }
      }
      // When we're past the end of the buffer, stop looping.
      if (read == 0)
        break;
    }

    // Update time
    output_.set_start_time(file_position_samples_);
    file_position_samples_ += read;
    PushOutput();
  }
}
}  // namespace aimc
