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

#ifndef _AIMC_MODULES_INPUT_FILE_H_
#define _AIMC_MODULES_INPUT_FILE_H_

#include <sndfile.h>

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
class ModuleFileInput : public Module {
 public:
  ModuleFileInput(Parameters *pParam);
  virtual ~ModuleFileInput();

  /*! \brief Initializes this input device using an audio file
   *  \param sFilename Path of the file to load
   *  \return true on success, false on error
   */
  bool LoadFile(const char *sFilename);

  //! \brief Process the loaded file.
  void Process();

  //! \brief Dummy Initialize function. Call LoadFile instead.
  virtual bool Initialize(const SignalBank &input);

  //! \brief Dummy funciton to comply with the Module specification. Gives an
  //  error message when called.
  virtual void Process(const SignalBank &input);

 private:
  //! \brief Prepare the module
  //! \param input Input SignalBank
  //! \param output true on success false on failure
  virtual bool InitializeInternal(const SignalBank &input);

  //! \brief Rewind to the start of the file
  virtual void ResetInternal();

  //! \brief File descriptor
  SNDFILE *file_handle_;

  //! \brief Current position in time of the file
  int file_position_samples_;
  bool file_loaded_;
  int audio_channels_;
  int buffer_length_;
  float sample_rate_;
};
}  // namespace aimc

#endif // _AIMC_MODULES_INPUT_FILE_H_
