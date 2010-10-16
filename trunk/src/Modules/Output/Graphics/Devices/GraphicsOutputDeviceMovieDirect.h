// Copyright 2006, Thomas Walters
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
 * \brief Output device for output direct to a movie via local calls to libavcodec
 *
 * \author Tom Walters <tom@acousticscale.org>
 * \date created 2007/10/8
 * \version \$Id$
 */

#ifndef __GRAPHICS_OUTPUT_DEVICE_MOVIE_DIRECT_H__
#define __GRAPHICS_OUTPUT_DEVICE_MOVIE_DIRECT_H__

#include "Modules/Output/Graphics/Devices/GraphicsOutputDeviceMovie.h"

extern "C" {
#include <ffmpeg/avformat.h>
#include <ffmpeg/swscale.h>
}

namespace aimc {

/*!
 * \class LibavformatWriter "Output/GraphicsOutputDeviceMovieDirect.h"
 * \brief Helper class to use libavcodec to write a movie file
 */
class LibavformatWriter {
 public:
  LibavformatWriter();
  ~LibavformatWriter() { };
  bool Init(const char *sMovieFile, int width, int height, float framerate);
  void WriteFrame(unsigned char *pFrameBuffer);
  void End();
 private:
  AVFrame *picture, *tmp_picture;
  uint8_t *video_outbuf;
  int frame_count, video_outbuf_size;
  int sws_flags;
  PixelFormat pixfmt;
  AVOutputFormat *fmt;
  AVFormatContext *oc;
  AVStream *video_st;
  double video_pts;
  int i;
  AVStream* add_video_stream(AVFormatContext *oc, CodecID codec_id, int width, int height, float framerate);
  AVFrame* alloc_picture(int pix_fmt, int width, int height);
  void open_video(AVFormatContext *oc, AVStream *st);
  void close_video(AVFormatContext *oc, AVStream *st);
  void fill_image(AVFrame *pict,unsigned char *pFrameBuffer , int width, int height);
};

/*!
 * \class GraphicsOutputDeviceMovie "Output/GraphicsOutputDeviceMovie.h"
 * \brief Output class for output to a movie
 */
class GraphicsOutputDeviceMovieDirect : public GraphicsOutputDeviceMovie {
 public:
  GraphicsOutputDeviceMovieDirect(AimParameters *pParam);
  virtual ~GraphicsOutputDeviceMovieDirect() { };
  /*! \brief Initializes this output device, prepares plotting tools.
   *  \param sSoundFile Sound file for the movie
   *  \param sMovieFile Movie filename to produce
   *  \return true on success, false on failure.
   *
   *  As usual, make sure to call this function before any other. If this
   *  Initialize() failed, you shouldn't try the other functions either.
   */
  bool Initialize(const char *sSoundFile, const char *sMovieFile);
  void Stop();
  void gRelease();
 private:
  LibavformatWriter* m_pOutputMovie;
};
}  // namespace aimc
#endif /* __GRAPHICS_OUTPUT_DEVICE_MOVIE_DIRECT_H__ */
