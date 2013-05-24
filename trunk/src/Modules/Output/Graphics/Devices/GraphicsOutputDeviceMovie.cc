// Copyright 2006, Willem van Engen
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
 * \version \$Id: GraphicsOutputDeviceMovie.cpp 633 2008-09-11 04:20:16Z tom $
 */

/*! \todo
 *  A recent experiment showed that the video was about an audioframe
 *  (something like 30 ms) behind the audio in rdct.wav. It seems odd
 *  to me, since I already output a frame at the beginning to compensate
 *  for the missed buffer.
 *  A solution that would solve this and be a broader improvement, is to
 *  include the source's time when Fire()ing. The outputdevice can then
 *  do audio/video synchronization. In the case of the movie, the very
 *  first gGrab() looks at the time and emits as much empty output frames
 *  as needed until the correct signal time is reached.
 */
#include "Support/Common.h"

#ifdef _WINDOWS
#  include <direct.h> // for _mkdir&_rmdir
#else
#  include <sys/types.h>
#  include <dirent.h> // for opendir&friends
#endif
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "Modules/Output/Graphics/Devices/GraphicsOutputDeviceMovie.h"

namespace aimc {

GraphicsOutputDeviceMovie::GraphicsOutputDeviceMovie(Parameters *parameters)
  : GraphicsOutputDeviceCairo(parameters) {
  sound_filename_.clear();
  movie_filename_.clear();
}

bool GraphicsOutputDeviceMovie::Initialize(Parameters *global_parameters) {
  global_parameters_ = global_parameters;
  sound_filename_ = global_parameters->GetString("input_filename");
  string file_suffix = parameters_->DefaultString("filename_suffix", ".mov");
  movie_filename_ = global_parameters->GetString("output_filename_base")
                    + file_suffix;
  
  FILE *f;

  // Check sound file exists
  if ((f = fopen(sound_filename_.c_str(), "r")) == NULL) {
    LOG_ERROR(_T("Couldn't open sound file '%s' for movie creation."),
             sound_filename_.c_str());
    sound_filename_.clear();
    return false;
  }
  fclose(f);

  // Check movie output file can be made
  if ((f = fopen(movie_filename_.c_str(), "w")) == NULL) {
    LOG_ERROR(_T("Couldn't open movie file '%s' to write to."),
             movie_filename_.c_str());
    movie_filename_.clear();
    return false;
  }
  fclose(f);

  // Get a temporary image output directory
  //! \warning Not really safe ... but windows has no mkdtemp()
  //! \todo Make build system check for mkdtemp() to use it when available. See TODO.txt.
#ifdef _WINDOWS
  char *temp_dir = NULL;
  if ((temp_dir = _tempnam(NULL, AIM_NAME))
      && _mkdir(temp_dir) >= 0) {
    directory_ = temp_dir;
    directory_ += "\\"; // Make sure to end with trailing slash
  } else  {
    LOG_ERROR(_T("Couldn't create a temporary directory for movie output."));
    if (temp_dir) {
      free(temp_dir);
    }
    return false;
  }
  if (temp_dir) {
    free(temp_dir);
  }
#else
  char temp_dir[PATH_MAX];
  strcpy(temp_dir, "/tmp/"AIM_NAME"-movie.XXXXXX");
  if (mkdtemp(temp_dir)) {
    directory_ = temp_dir;
    directory_ += "/"; // Make sure to end with trailing slash    
  } else {
    LOG_ERROR(_T("Couldn't create a temporary directory for movie output."));
    return false;
  }
#endif
  
  // We want png for movie conversion
  parameters_->SetString("output.img.format", "png");
  if ( !GraphicsOutputDeviceCairo::Initialize(directory_) ) {
    return false;
  }
  return true;
}

void GraphicsOutputDeviceMovie::Start() {
  GraphicsOutputDeviceCairo::Start();
  // Output a couple of frames to get audio/video in sync, put params in there.
  gGrab();
  PlotParameterScreen();
  gRelease();
  gGrab();
  PlotParameterScreen();
  gRelease();
}

void GraphicsOutputDeviceMovie::Reset(Parameters* global_parameters) {
  Stop();
  Initialize(global_parameters);
}

void GraphicsOutputDeviceMovie::Stop() {
  GraphicsOutputDeviceCairo::Stop();
  CloseFile();
  m_iFileNumber = 0;

#ifdef __WX__
  // GUI only: popup dialog
#else
  printf("Generating movie ... \n");
#endif
  AIM_ASSERT(parameters_);
  // Convert images and sound file to a movie.
  //! \warning Movie files are overwritten without warning.
  char sffmpegPath[1024];
  if (!parameters_->IsSet("output.ffmpeg_path")) {
  strcpy(sffmpegPath,"ffmpeg");
  } else {
    strcpy(sffmpegPath, parameters_->GetString("output.ffmpeg_path"));
  }
  char sCodecOptions[1024];
  if (!parameters_->IsSet("output.ffmpeg_codec_options")) {
    strcpy(sCodecOptions,"");
  } else {
    strcpy(sCodecOptions, parameters_->GetString("output.ffmpeg_codec_options"));
  }
  float frame_rate = global_parameters_->DefaultFloat("frame_rate", -1.0);
  char sCmdLine[1024]; //!\todo check that snprintf does not want a larger buffer
  snprintf(sCmdLine, sizeof(sCmdLine)/sizeof(sCmdLine[0]),
    "%s -y -i \"%s\" -r %.2f -i \"%s%%06d.png\" "
    "-qscale 1 -r %.2f -ar 44100 -acodec pcm_s16le %s \"%s\"",
           sffmpegPath, sound_filename_.c_str(), frame_rate, directory_.c_str(),
           frame_rate, sCodecOptions, movie_filename_.c_str());
  printf("%s", sCmdLine);
  printf("\n");
  if (system(sCmdLine)) {
    LOG_ERROR(_T("Couldn't create movie output."));
  }

#ifdef __WX__
  // GUI only: close dialog again
#endif
  // Remove files in temporary directory and the dir itself
  //! \todo make portable function, possibly decided on by build system
#ifdef _WINDOWS
  HANDLE hList;
  WIN32_FIND_DATA FileData;
  snprintf(sCmdLine, sizeof(sCmdLine)/sizeof(sCmdLine[0]), "%s/*.*", directory_.c_str());
  if ((hList = FindFirstFile(sCmdLine, &FileData)) == INVALID_HANDLE_VALUE) {
    LOG_ERROR(_T("Couldn't remove files from temporary directory."));
    return;
  }
  bool bRMfinished = false;
  while (!bRMfinished) {
    snprintf(sCmdLine,
             sizeof(sCmdLine)/sizeof(sCmdLine[0]),
             "%s%s",
             directory_.c_str(),
             FileData.cFileName);
    remove(sCmdLine);
    if (!FindNextFile(hList, &FileData) && GetLastError() == ERROR_NO_MORE_FILES) {
      bRMfinished = true;
    }
  }
  FindClose(hList);
  _rmdir(directory_.c_str());
#else
  DIR *dir;
  struct dirent *dirent;
  if (!(dir = opendir(directory_.c_str()))) {
    LOG_ERROR(_T("Couldn't remove files in temporary directory."));
    return;
  }
  while ((dirent = readdir(dir))) {
    snprintf(sCmdLine,
             sizeof(sCmdLine)/sizeof(sCmdLine[0]),
             "%s%s",
             directory_.c_str(),
             dirent->d_name);
    unlink(sCmdLine);
  }
  closedir(dir);
  rmdir(directory_.c_str());
#endif
}

void GraphicsOutputDeviceMovie::PlotParameterScreen() {
  AIM_ASSERT(parameters_);
  char sStr[50];
  int lineno = 1;

  float fMarL = parameters_->GetFloat(_S("graph.margin.left"));
  float fMarT = parameters_->GetFloat(_S("graph.margin.top"));
  float fTextHeight = 1.0f / 50.0f * 1.2; // change this when fontsizing is there!

  gText2f(fMarL, 1-(fMarT+fTextHeight*lineno++),
          _S("AIM-C"));
  gText2f(fMarL,
          1-(fMarT+fTextHeight*lineno++),
          _S("(c) 2006-2010, Thomas Walters, Willem van Engen"));
  gText2f(fMarL,
          1-(fMarT+fTextHeight*lineno++),
          _S("http://aimc.acousticscale.org/"));
  lineno++;

  static const char *pPlotParams[] = {
    _S("input.buffersize"),
    _S("input.samplerate"),
    _S("bmm.freqstart"),
    _S("bmm.freqend"),
    _S("bmm.numchannels"),
    _S("preset.name"),
    _S("preset.title"),
    NULL
  };
  for (int i = 0; pPlotParams[i]; i++) {
    snprintf(sStr,
             sizeof(sStr)/sizeof(sStr[0]), _S("%s=%s"),
             pPlotParams[i],
             parameters_->GetString(pPlotParams[i]));
    gText2f(fMarL,
            1-(fMarT+fTextHeight*lineno++),
            sStr);
  }
}
}  // namespace aimc
