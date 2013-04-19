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

/*!
 * \file
 * \brief Output device for output to a movie
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/10/16
 * \version \$Id: $
 */

#ifndef __GRAPHICS_OUTPUT_DEVICE_MOVIE_H__
#define __GRAPHICS_OUTPUT_DEVICE_MOVIE_H__

#include <string>
#include <unistd.h>
#include "Modules/Output/Graphics/Devices/GraphicsOutputDeviceCairo.h"

namespace aimc {

/*!
 * \class GraphicsOutputDeviceMovie "Output/GraphicsOutputDeviceMovie.h"
 * \brief Output class for output to a movie
 */
 // GraphicsOutputDevicePlotutils is also possible here
class GraphicsOutputDeviceMovie : public GraphicsOutputDeviceCairo {
 public:
  GraphicsOutputDeviceMovie(Parameters *parameters);
  virtual ~GraphicsOutputDeviceMovie() { };

  /*! \brief Initializes this output device, prepares plotting tools.
   *  \param sSoundFile Sound file for the movie
   *  \param sMovieFile Movie filename to produce
   *  \return true on success, false on failure.
   *
   *  As usual, make sure to call this function before any other. If this
   *  Initialize() failed, you shouldn't try the other functions either.
   */
  bool Initialize(Parameters *global_parameters);

  void Start();
  //! \brief This function now also generates the output movie.
  void Stop();
  
  void Reset(Parameters* global_parameters);
  

 protected:
  /*! \brief Plots a summary of relevant parameters on the output
   *
   *  This is intended for use in a movie as the first frame, which has no
   *  interesting data anyway, since at least one buffer of data is needed
   *  to be able to show someting.
   *
   *  The caller must do the gGrab() and gRelease().
   */
  void PlotParameterScreen();

  //! \brief Name of the sound file to be merged with the video
  string sound_filename_;
  //! \brief Name of the movie file to produce
  string movie_filename_;
};
}  // namespace aimc
#endif /* __GRAPHICS_OUTPUT_DEVICE_MOVIE_H__ */
