// Copyright 2006-2010, Thomas Walters
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
 * \file
 * \brief File output in the HTK format.
 *
 * \author Tom Walters <tom@acousticscale.org>
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2007/01/26
 * \version \$Id: $
 */

#ifdef _WINDOWS
#  include <direct.h>  // for _mkdir & _rmdir
#else
#  include <sys/types.h>
#  include <dirent.h>  // for opendir & friends
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#include "Modules/Output/FileOutputAIMC.h"

namespace aimc {
FileOutputAIMC::FileOutputAIMC(Parameters *params) : Module(params) {
  module_description_ = "File output in AIMC format";
  module_identifier_ = "htk_out";
  module_type_ = "output";
  module_version_ = "$Id: FileOutputAIMC.cc 51 2010-03-30 22:06:24Z tomwalters $";

  file_handle_ = NULL;
  header_written_ = false;
  frame_period_ms_ = 0.0f;
}

FileOutputAIMC::~FileOutputAIMC() {
  if (file_handle_ != NULL)
    CloseFile();
}

bool FileOutputAIMC::OpenFile(const char* filename, float frame_period_ms) {
  if (file_handle_ != NULL) {
    LOG_ERROR(_T("Couldn't open output file. A file is already open."));
    return false;
  }

  // Check that the output file exists and is writeable
  if ((file_handle_ = fopen(filename, "wb")) == NULL) {
    LOG_ERROR(_T("Couldn't open output file '%s' for writing."), filename);
    return false;
  }
  frame_count_ = 0;
  frame_period_ms_ = frame_period_ms;
  header_written_ = false;
  if (initialized_) {
    WriteHeader();
  }
  return true;
}

bool FileOutputAIMC::InitializeInternal(const SignalBank &input) {
  if (file_handle_ == NULL) {
    LOG_ERROR(_T("Couldn't initialize file output. "
                 "Please call FileOutputAIMC::OpenFile first"));
    return false;
  }
  if (header_written_) {
    LOG_ERROR(_T("A header has already been written on the output file. "
                 "Please call FileOutputAIMC::CloseFile to close that file, "
                 "and FileOutputAIMC::OpenFile to open an new one before "
                 "calling FileOutputAIMC::Initialize again."));
    return false;
  }
  channel_count_ = input.channel_count();
  buffer_length_ = input.buffer_length();	
  sample_rate_ = input.sample_rate();	
  WriteHeader();
  return true;
}

void FileOutputAIMC::ResetInternal() {
  if (file_handle_ != NULL && !header_written_) {
    WriteHeader();
  }
  if (file_handle_ != NULL)
    CloseFile();
}

void FileOutputAIMC::WriteHeader() {
  if (header_written_)
    return;

  /* File format:
   * Header: Number of frames (uint32),
   *         Frame period in milliseconds (float32),
   *         Number of channels per frame (uint32),
   *         Number of samples per channel of each frame (uint32)
   *         Sample rate (float32)
   *
   * Data: Series of floats, by time, then channel, then frame
   * f1c1t1,f1c1t2,f1c1t3...
   */

  uint32_t frame_count_out = frame_count_;
  float sample_period_out = frame_period_ms_;
  uint32_t channels_out = channel_count_;
  uint32_t samples_out = buffer_length_;
  float sample_rate = sample_rate_;

  fwrite(&frame_count_out, sizeof(frame_count_out), 1, file_handle_);
  fwrite(&sample_period_out, sizeof(sample_period_out), 1, file_handle_);
  fwrite(&channels_out, sizeof(channels_out), 1, file_handle_);
  fwrite(&samples_out, sizeof(samples_out), 1, file_handle_);
  fwrite(&sample_rate, sizeof(sample_rate), 1, file_handle_);
  fflush(file_handle_);

  header_written_ = true;
}

void FileOutputAIMC::Process(const SignalBank &input) {
  if (file_handle_ == NULL) {
    LOG_ERROR(_T("Couldn't process file output. No file is open."
                 "Please call FileOutputAIMC::OpenFile first"));
    return;
  }

  if (!header_written_) {
    LOG_ERROR(_T("No header has been written on the output file yet. Please "
                 "call FileOutputAIMC::Initialize() before calling "
                 "FileOutputAIMC::Process()"));
    return;
  }
  float s;

  for (int ch = 0; ch < input.channel_count(); ch++) {
    for (int i = 0; i < input.buffer_length(); i++) {
      s = input.sample(ch, i);
      fwrite(&s, sizeof(s), 1, file_handle_);
    }
  }
  frame_count_++;
}

bool FileOutputAIMC::CloseFile() {
  if (file_handle_ == NULL)
    return false;

  // Write the first 4 bytes of the file
  // with how many samples there are in the file
  fflush(file_handle_);
  rewind(file_handle_);
  fflush(file_handle_);
  uint32_t frame_count = frame_count_;
  fwrite(&frame_count, sizeof(frame_count), 1, file_handle_);

  // And close the file
  fclose(file_handle_);
  file_handle_ = NULL;
  header_written_ = false;
  return true;
}
}  // namespace aimc

