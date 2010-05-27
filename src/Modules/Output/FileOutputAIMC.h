// Copyright 2006-2010, Thomas Walters, Willem van Engen
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
 * \brief File output to AIMC format
 *
 * \author Thomas Walters <tom@acousticscale.org>
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/10/30
 * \version \$Header$
 */

#ifndef AIMC_MODULES_OUTPUT_AIMC_H_
#define AIMC_MODULES_OUTPUT_AIMC_H_

#include "Support/Module.h"
#include "Support/SignalBank.h"

namespace aimc {
class FileOutputAIMC : public Module {
 public:
  /*! \brief Create a new file output for an AIMC format file.
   */
  explicit FileOutputAIMC(Parameters *pParam);
  ~FileOutputAIMC();

  /*! \brief Initialize the output to AIMC.
   *  \param *filename Filename of the ouptut file to be created.
   *  If the file exists it will be overwritten
   *  \return Returns true on success of initialization.
   */
  bool OpenFile(const char *filename, float frame_period_ms);
  bool CloseFile();
  virtual void Process(const SignalBank &input);
 private:
  virtual bool InitializeInternal(const SignalBank &input);
  virtual void ResetInternal();

  void WriteHeader();

  /*! \brief Whether initialization is done or not
   */
  bool header_written_;

  /*! \brief Internal pointer to the output file
   */
  FILE *file_handle_;

  /*! \brief Count of the number of samples in the file, written on close
   */
  int sample_count_;

  int channel_count_;
  int buffer_length_;
  float sample_rate_;	
  float frame_period_ms_;
};
}  // namespace aimc

#endif  // AIMC_MODULES_OUTPUT_AIMC_H_

