// Copyright 2006-2010, Thomas Walters
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

/*!
 * \file
 * \brief File output in the HTK format.
 *
 * \author Tom Walters <tom@acousticscale.org>
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/10/30
 * \version \$Id$
 */

#ifdef _WINDOWS
#  include <direct.h>  // for _mkdir&_rmdir
#else
#  include <sys/types.h>
#  include <dirent.h>  // for opendir&friends
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#include "Modules/Output/FileOutputHTK.h"

namespace aimc {
FileOutputHTK::FileOutputHTK(Parameters *params) : Module(params) {
  module_description_ = "File output in HTK format";
  module_identifier_ = "htk_out";
  module_type_ = "output";
  module_version_ = "$Id$";

  file_handle_ = NULL;
  header_written_ = false;
  frame_period_ms_ = 0.0f;
}

FileOutputHTK::~FileOutputHTK() {
  if (file_handle_ != NULL)
    CloseFile();
}

bool FileOutputHTK::OpenFile(const char* filename, float frame_period_ms) {
  if (file_handle_ != NULL) {
    LOG_ERROR(_T("Couldn't open output file. A file is already open."));
    return false;
  }

  // Check that the output file exists and is writeable
  if ((file_handle_ = fopen(filename, "wb")) == NULL) {
    LOG_ERROR(_T("Couldn't open output file '%s' for writing."), filename);
    return false;
  }
  sample_count_ = 0;
  frame_period_ms_ = frame_period_ms;
  header_written_ = false;
  if (initialized_) {
    WriteHeader(channel_count_ * buffer_length_, frame_period_ms_);
  }
  return true;
}

bool FileOutputHTK::InitializeInternal(const SignalBank &input) {
  if (file_handle_ == NULL) {
    LOG_ERROR(_T("Couldn't initialize file output. "
                 "Please call FileOutputHTK::OpenFile first"));
    return false;
  }
  if (header_written_) {
    LOG_ERROR(_T("A header has already been written on the output file. "
                 "Please call FileOutputHTK::CloseFile to close that file, "
                 "and FileOutputHTK::OpenFile to open an new one before "
                 "calling FileOutputHTK::Initialize again."));
    return false;
  }
  channel_count_ = input.channel_count();
  buffer_length_ = input.buffer_length();
  WriteHeader(channel_count_ * buffer_length_, frame_period_ms_);
  return true;
}

void FileOutputHTK::ResetInternal() {
  if (file_handle_ != NULL && !header_written_) {
    WriteHeader(channel_count_ * buffer_length_, frame_period_ms_);
  }
  if (file_handle_ != NULL)
    CloseFile();
}

void FileOutputHTK::WriteHeader(int num_elements, float period_ms) {
  if (header_written_)
    return;

  /* HTK format file: (taken from the HTK book - section 5.10.1)
   * Header: 12 bytes in total, contains:
   * sample_count - number of samples in file (4-byte integer)(long)
   * sample_period - sample period in 100ns units (4-byte integer)(long)
   * sample_size - number of bytes per sample (2-byte integer)(short)
   * parameter_kind - a code indicating the sample kind (2-byte integer)(short)
   */

  // To be filled in when the file is done
  int32_t sample_count = 0;

  int32_t sample_period = floor(1e4 * period_ms);
  int16_t sample_size = num_elements * sizeof(float);  // NOLINT

  // User-defined coefficients with energy term
  int16_t parameter_kind = H_USER + H_E;

  // Fix endianness
  sample_count = ByteSwap32(sample_count);
  sample_period = ByteSwap32(sample_period);
  sample_size = ByteSwap16(sample_size);
  parameter_kind = ByteSwap16(parameter_kind);

  // Enter header values. sample_count is a dummy value which is filled in on
  // file close
  fwrite(&sample_count, sizeof(sample_count), 1, file_handle_);
  fwrite(&sample_period, sizeof(sample_period), 1, file_handle_);
  fwrite(&sample_size, sizeof(sample_size), 1, file_handle_);
  fwrite(&parameter_kind, sizeof(parameter_kind), 1, file_handle_);
  fflush(file_handle_);

  header_written_ = true;
}


void FileOutputHTK::Process(const SignalBank &input) {
  if (file_handle_ == NULL) {
    LOG_ERROR(_T("Couldn't process file output. No file is open."
                 "Please call FileOutputHTK::OpenFile first"));
    return;
  }

  if (!header_written_) {
    LOG_ERROR(_T("No header has been written on the output file yet. Please "
                 "call FileOutputHTK::Initialize() before calling "
                 "FileOutputHTK::Process()"));
    return;
  }
  float s;

  for (int ch = 0; ch < input.channel_count(); ch++) {
    for (int i = 0; i < input.buffer_length(); i++) {
      s = input.sample(ch, i);
      s = ByteSwapFloat(s);
      fwrite(&s, sizeof(s), 1, file_handle_);
    }
  }
  sample_count_++;
}

bool FileOutputHTK::CloseFile() {
  if (file_handle_ == NULL)
    return false;

  // Write the first 4 bytes of the file
  // with how many samples there are in the file
  fflush(file_handle_);
  rewind(file_handle_);
  fflush(file_handle_);
  int32_t samples = sample_count_;
  samples = ByteSwap32(samples);
  fwrite(&samples, sizeof(samples), 1, file_handle_);

  // And close the file
  fclose(file_handle_);
  file_handle_ = NULL;
  header_written_ = false;
  return true;
}

float FileOutputHTK::ByteSwapFloat(float d) {
  // Endianness fix
  float a;
  unsigned char *dst = (unsigned char *)&a;
  unsigned char *src = (unsigned char *)&d;

  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];

  return a;
}
}  // namespace aimc

