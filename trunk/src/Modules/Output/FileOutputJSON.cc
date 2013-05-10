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


#include <iostream>
#include <fstream>
#include <iomanip>

#include "Modules/Output/FileOutputJSON.h"

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
#include <string>

namespace aimc {
FileOutputJSON::FileOutputJSON(Parameters *params) : Module(params) {
  module_description_ = "File output in JSON format";
  module_identifier_ = "json_out";
  module_type_ = "output";
  module_version_ = "$Id: FileOutputJSON.cc 51 2010-03-30 22:06:24Z tomwalters $";
  file_suffix_ = parameters_->DefaultString("file_suffix", ".json");

  header_written_ = false;
  frame_period_ms_ = 0.0f;
}

FileOutputJSON::~FileOutputJSON() {
  if (file_handle_)
    CloseFile();
}

bool FileOutputJSON::OpenFile(string &filename) {
  file_handle_.open(filename.c_str());
  
  frame_count_ = 0;
  frame_period_ms_ = 0.0;
  header_written_ = false;

  if (initialized_) {
    WriteHeader();
  }
  
  return true;
}

bool FileOutputJSON::InitializeInternal(const SignalBank &input) {
  channel_count_ = input.channel_count();
  buffer_length_ = input.buffer_length();  
  sample_rate_ = input.sample_rate();
  ResetInternal();
  if (file_handle_ == NULL) {
    LOG_ERROR(_T("Couldn't initialize file output."));
    return false;
  }
  return true;
}

void FileOutputJSON::ResetInternal() {
  if (file_handle_ != NULL && !header_written_) {
    WriteHeader();
  }
  if (file_handle_ != NULL)
    CloseFile();
    
  string out_filename;
  out_filename = global_parameters_->GetString("output_filename_base") + file_suffix_;
  OpenFile(out_filename);
}

void FileOutputJSON::WriteHeader() {
  if (header_written_)
    return;

  uint32_t channels_out = channel_count_;
  uint32_t samples_out = buffer_length_;
  float sample_rate = sample_rate_;

  file_handle_ << std::setprecision(3);
  file_handle_ << "{" << std::endl;
  file_handle_ << "\"channels\" : " << channels_out << "," << std::endl;
  file_handle_ << "\"samples\" : " << sizeof(samples_out) << "," << std::endl;
  file_handle_ << "\"sample_rate\" : " << sizeof(sample_rate) << "," << std::endl;

  header_written_ = true;
}

void FileOutputJSON::Process(const SignalBank &input) {
  if (file_handle_ == NULL) {
    LOG_ERROR(_T("Couldn't process file output. No file is open."
                 "Please call FileOutputJSON::OpenFile first"));
    return;
  }

  if (!header_written_) {
    LOG_ERROR(_T("No header has been written on the output file yet. Please "
                 "call FileOutputJSON::Initialize() before calling "
                 "FileOutputJSON::Process()"));
    return;
  }
  float s;

  for (int ch = 0; ch < input.channel_count(); ch++) {
    file_handle_ << "\"channel\" : [";
    for (int i = 0; i < input.buffer_length(); i++) {
      s = input.sample(ch, i);
      file_handle_ << s;
      if (i < input.buffer_length() - 1) {
        file_handle_ << ",";
      }
    }
    file_handle_ << "]" << std::endl;
    if (ch < input.channel_count() - 1) {
      file_handle_ << ",";
    }
  }

  frame_count_++;

}

bool FileOutputJSON::CloseFile() {
  if (file_handle_ == NULL)
    return false;

  uint32_t frame_count = frame_count_;
  float sample_period_out = frame_period_ms_;

  file_handle_ << "\"frame_count\" : " << sizeof(frame_count) << std::endl;
  file_handle_ << "\"samples\" : " << sizeof(sample_period_out) << std::endl;
  file_handle_ << "}" << std::endl;


  file_handle_.close();
  header_written_ = false;
  return true;
}
}  // namespace aimc

