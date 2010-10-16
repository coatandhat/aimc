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

#include "Modules/Output/Graphics/Devices/GraphicsOutputDeviceMovie.h"

namespace aimc {

GraphicsOutputDeviceMovie::GraphicsOutputDeviceMovie(Parameters *pParam)
  : GraphicsOutputDeviceCairo(pParam) {  // or GraphicsOutputDevicePlotutils
  m_sMovieFile[0] = '\0';
  m_sSoundFile[0] = '\0';
}

bool GraphicsOutputDeviceMovie::Initialize(const char *sSoundFile,
                                           const char *sMovieFile) {
  FILE *f;
  AIM_ASSERT(sSoundFile);
  AIM_ASSERT(sMovieFile);

  // Check sound file exists
  if ((f = fopen(sSoundFile, "r")) == NULL) {
    LOG_ERROR(_T("Couldn't open sound file '%s' for movie creation."),
             sSoundFile);
    return false;
  }
  fclose(f);
  strcpy(m_sSoundFile, sSoundFile);

  // Check movie output file can be made
  if ( (f=fopen(sMovieFile, "w"))==NULL ) {
    LOG_ERROR(_T("Couldn't open movie file '%s' to write to."),
             sMovieFile);
    return false;
  }
  fclose(f);
  strcpy(m_sMovieFile, sMovieFile);

  // Get a temporary image output directory
  //! \warning Not really safe ... but windows has no mkdtemp()
  //! \todo Make build system check for mkdtemp() to use it when available. See TODO.txt.
  char *sTmpDir = NULL;
#ifdef _WINDOWS
  if ((sTmpDir = _tempnam(NULL, AIM_NAME))
      && _mkdir(sTmpDir) >= 0) {
    strcpy(m_sDir, sTmpDir);
    strcat(m_sDir, "\\"); // Make sure to end with trailing slash
  } else
#else
  strcpy(m_sDir, "/tmp/"AIM_NAME"-movie.XXXXXX");
  if (mkdtemp(m_sDir)) {
    strcat(m_sDir, "/"); // Make sure to end with trailing slash
  } else
#endif
  {
    LOG_ERROR(_T("Couldn't create a temporary directory for movie output."));
    if (sTmpDir) free(sTmpDir);
    return false;
  }
  if (sTmpDir) {
    free(sTmpDir);
  }

  // We want png for movie conversion
  //! \bug This may change the user preference in GUI, hmm what to do? See TODO.txt
  m_pParam->SetString("output.img.format", "png");
  //if ( !GraphicsOutputDevicePlotutils::Initialize(m_sDir) ) {
  if ( !GraphicsOutputDeviceCairo::Initialize(m_sDir) ) {
    return false;
  }

  return true;
}

void GraphicsOutputDeviceMovie::Start() {
  //GraphicsOutputDevicePlotutils::Start();
  GraphicsOutputDeviceCairo::Start();
  // Just output a single frame to get audio/video in sync, put params in there
  gGrab();
  PlotParameterScreen();
  gRelease();
}

void GraphicsOutputDeviceMovie::Stop() {
  // Make sure Plotutils is really done writing.
  //GraphicsOutputDevicePlotutils::Stop();
  GraphicsOutputDeviceCairo::Stop();
  CloseFile();

#ifdef __WX__
  // GUI only: popup dialog
#else
  printf("Generating movie ... \n");
#endif
  AIM_ASSERT(m_pParam);
  // Convert images and sound file to a movie
  //! \warning Movie files are overwritten without warning
  //! \bug ffmpeg only works with colour images, not with bw. So make sure to not use bw only in drawing..
  // Always convert to audio stream of 44.1kHz or problems may occur in playing or conversio.
  float fFps = 1000.0 / m_pParam->GetFloat("output.frameperiod");
  char sffmpegPath[1024];
  if (!m_pParam->IsSet("output.ffmpeg_path")) {
  strcpy(sffmpegPath,"ffmpeg");
  } else {
    strcpy(sffmpegPath, m_pParam->GetString("output.ffmpeg_path"));
  }
  char sCodecOptions[1024];
  if (!m_pParam->IsSet("output.ffmpeg_codec_options")) {
    strcpy(sCodecOptions,"");
  } else {
    strcpy(sCodecOptions, m_pParam->GetString("output.ffmpeg_codec_options"));
  }

  char sCmdLine[1024]; //!\todo check that snprintf does not want a larger buffer
  snprintf(sCmdLine, sizeof(sCmdLine)/sizeof(sCmdLine[0]),
    "%s -r %.2f -y -i \"%s\" -i \"%s%%06d.png\" "
    "-title \"%s\" -comment \"Generated by "AIM_NAME" "AIM_VERSION_STRING"\" "
    "-sameq -r %.2f -ar 44100 -acodec pcm_s16le %s \"%s\"",
    sffmpegPath, fFps, m_sSoundFile, m_sDir,
    m_pParam->GetString("output.movie.title"),
    fFps, sCodecOptions, m_sMovieFile);
    printf(sCmdLine);
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
  snprintf(sCmdLine, sizeof(sCmdLine)/sizeof(sCmdLine[0]), "%s/*.*", m_sDir);
  if ((hList = FindFirstFile(sCmdLine, &FileData)) == INVALID_HANDLE_VALUE) {
    LOG_ERROR(_T("Couldn't remove files from temporary directory."));
    return;
  }
  bool bRMfinished = false;
  while (!bRMfinished) {
    snprintf(sCmdLine,
             sizeof(sCmdLine)/sizeof(sCmdLine[0]),
             "%s%s",
             m_sDir,
             FileData.cFileName);
    remove(sCmdLine);
    if (!FindNextFile(hList, &FileData) && GetLastError() == ERROR_NO_MORE_FILES) {
      bRMfinished = true;
    }
  }
  FindClose(hList);
  _rmdir(m_sDir);
#else
  DIR *dir;
  struct dirent *dirent;
  if (!(dir = opendir(m_sDir))) {
    LOG_ERROR(_T("Couldn't remove files in temporary directory."));
    return;
  }
  while (dirent = readdir(dir)) {
    snprintf(sCmdLine,
             sizeof(sCmdLine)/sizeof(sCmdLine[0]),
             "%s%s",
             m_sDir,
             dirent->d_name);
    unlink(sCmdLine);
  }
  closedir(dir);
  rmdir(m_sDir);
#endif
}

void GraphicsOutputDeviceMovie::PlotParameterScreen() {
  AIM_ASSERT(m_pParam);
  char sStr[50];
  int lineno = 1;

  float fMarL = m_pParam->GetFloat(_S("graph.margin.left"));
  float fMarT = m_pParam->GetFloat(_S("graph.margin.top"));
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
             m_pParam->GetString(pPlotParams[i]));
    gText2f(fMarL,
            1-(fMarT+fTextHeight*lineno++),
            sStr);
  }
}
}  // namespace aimc
