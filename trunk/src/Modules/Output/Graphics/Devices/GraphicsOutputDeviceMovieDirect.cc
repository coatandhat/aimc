// Copyright 2007, Thomas Walters
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
 * \date created 2007/10/05
 * \version \$Id: $
 */

#include "Support/Common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Modules/Output/Graphics/Devices/GraphicsOutputDeviceMovieDirect.h"

namespace aimc {

GraphicsOutputDeviceMovieDirect::GraphicsOutputDeviceMovieDirect(Parameters *params)
  : GraphicsOutputDeviceMovie(params) {
  m_sMovieFile[0] = '\0';
  m_sSoundFile[0] = '\0';
}

bool GraphicsOutputDeviceMovieDirect::Initialize(const char *sSoundFile,
                                                 const char *sMovieFile) {
  // We want pnm for direct movie conversion as the data format is nice and simple
  //! \bug This may change the user preference in GUI, hmm what to do? See TODO.txt
  //m_pParam->SetString("output.img.format", "pnm");

  // Initialise GraphicsOutputDevicePlotutils for memory buffer use
  if(!GraphicsOutputDeviceCairo::Initialize())
    return false;

  int width = m_pParam->GetUInt("output.img.width");
  int height = m_pParam->GetUInt("output.img.height");
  //float framerate = 1000.0f/m_pParam->GetFloat("output.frameperiod");
  float framerate=1000.0f/20.0f;

  m_pOutputMovie = new LibavformatWriter;
  m_pOutputMovie->Init(sMovieFile,  width, height, framerate);

  return true;
}

void GraphicsOutputDeviceMovieDirect::Stop() {
  // Make sure Plotutils is really done writing.
  GraphicsOutputDeviceCairo::Stop();
  m_pOutputMovie->End();
  delete m_pOutputMovie;

}

void GraphicsOutputDeviceMovieDirect::gRelease() {
  // write the buffer
  unsigned char *buf = GraphicsOutputDeviceCairo::GetBuffer();

  if (buf != NULL)
    m_pOutputMovie->WriteFrame(buf);

  GraphicsOutputDeviceCairo::gRelease();
}


// Everything below here is hacked from the Libavformat API example:
/*
 * Libavformat API example: Output a media file in any supported
 * libavformat format. The default codecs are used.
 *
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


LibavformatWriter::LibavformatWriter() {
    sws_flags = SWS_BICUBIC;
    pixfmt= PIX_FMT_RGB24;
    fmt = NULL;
}

bool LibavformatWriter::Init(const char *sMovieFile,
                             int width,
                             int height,
                             float framerate) {
  /* initialize libavcodec, and register all codecs and formats */
  av_register_all();

  /* auto detect the output format from the name. default is mpeg. */
  fmt = guess_format(NULL, sMovieFile, NULL);
  if (!fmt) {
    printf("Could not deduce output format from file extension: using MPEG.\n");
    fmt = guess_format("mpeg", NULL, NULL);
  }
  if (!fmt) {
    fprintf(stderr, "Could not find suitable output format\n");
    return false;
  }

  /* allocate the output media context */
  oc = av_alloc_format_context();
  if (!oc) {
    fprintf(stderr, "Memory error\n");
    return false;
  }
  oc->oformat = fmt;
  snprintf(oc->filename, sizeof(oc->filename), "%s", sMovieFile);

  /* add the audio and video streams using the default format codecs
     and initialize the codecs */
  video_st = NULL;
  fmt->video_codec=CODEC_ID_PNG;
  if (fmt->video_codec != CODEC_ID_NONE) {
    video_st = add_video_stream(oc, fmt->video_codec, width, height, framerate);
  }

  /* set the output parameters (must be done even if no
     parameters). */
  if (av_set_parameters(oc, NULL) < 0) {
    fprintf(stderr, "Invalid output format parameters\n");
    return false;
  }

  dump_format(oc, 0, sMovieFile, 1);

  /* now that all the parameters are set, we can open the audio and
     video codecs and allocate the necessary encode buffers */
  if (video_st) {
    open_video(oc, video_st);
  }

  /* open the output file, if needed */
  if (!(fmt->flags & AVFMT_NOFILE)) {
    if (url_fopen(&oc->pb, sMovieFile, URL_WRONLY) < 0) {
      fprintf(stderr, "Could not open '%s'\n", sMovieFile);
        return false;
      }
  }

  /* write the stream header, if any */
  av_write_header(oc);
    return true;
}


/**************************************************************/
/* video output */

/* add a video output stream */
AVStream* LibavformatWriter::add_video_stream(AVFormatContext *oc,
                                              CodecID codec_id,
                                              int width,
                                              int height,
                                              float framerate) {
  AVCodecContext *c;
  AVStream *st;

  st = av_new_stream(oc, 0);
  if (!st) {
    fprintf(stderr, "Could not alloc stream\n");
    return NULL;
  }

  c = st->codec;
  c->codec_id = codec_id;
  c->codec_type = CODEC_TYPE_VIDEO;

  /* put sample parameters */
  /* resolution must be a multiple of two */
  c->width = width;
  c->height = height;
  /* time base: this is the fundamental unit of time (in seconds) in terms
     of which frame timestamps are represented. for fixed-fps content,
     timebase should be 1/framerate and timestamp increments should be
     identically 1. */
  c->time_base.den = (int)framerate;
  c->time_base.num = 1;
  c->gop_size = 12; /* emit one intra frame every twelve frames at most */
  c->pix_fmt = pixfmt;
  // some formats want stream headers to be separate
  if(!strcmp(oc->oformat->name, "mp4")
     || !strcmp(oc->oformat->name, "mov")
     || !strcmp(oc->oformat->name, "3gp")) {
      c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
  return st;
}

AVFrame* LibavformatWriter::alloc_picture(int pix_fmt,
                                          int width,
                                          int height) {
  AVFrame *picture;
  uint8_t *picture_buf;
  int size;

  picture = avcodec_alloc_frame();
  if (!picture) {
    return NULL;
  }
  size = avpicture_get_size(pix_fmt, width, height);
  picture_buf = (uint8_t*)av_malloc(size);
  if (!picture_buf) {
    av_free(picture);
    return NULL;
  }
  avpicture_fill((AVPicture *)picture, picture_buf,
                pix_fmt, width, height);
  return picture;
}

void LibavformatWriter::open_video(AVFormatContext *oc, AVStream *st) {
  AVCodec *codec;
  AVCodecContext *c;
  c = st->codec;

  /* find the video encoder */
  codec = avcodec_find_encoder(c->codec_id);
  if (!codec) {
    fprintf(stderr, "codec not found\n");
    exit(1);
  }

  /* open the codec */
  if (avcodec_open(c, codec) < 0) {
    fprintf(stderr, "could not open codec\n");
    exit(1);
  }

  video_outbuf = NULL;
  if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
    /* allocate output buffer */
    /* XXX: API change will be done */
    /* buffers passed into lav* can be allocated any way you prefer,
       as long as they're aligned enough for the architecture, and
       they're freed appropriately (such as using av_free for buffers
       allocated with av_malloc) */
    video_outbuf_size = 200000;
    video_outbuf = (uint8_t*)av_malloc(video_outbuf_size);
  }

  /* allocate the encoded raw picture */
  picture = alloc_picture(c->pix_fmt, c->width, c->height);
  if (!picture) {
    fprintf(stderr, "Could not allocate picture\n");
    exit(1);
  }

  /* if the output format is not RGB32, then a temporary RGB32
     picture is needed too. It is then converted to the required
     output format */
    tmp_picture = NULL;
    if (c->pix_fmt != PIX_FMT_RGB32) {
      tmp_picture = alloc_picture(PIX_FMT_RGB32, c->width, c->height);
      if (!tmp_picture) {
        fprintf(stderr, "Could not allocate temporary picture\n");
        exit(1);
      }
  }
}


void LibavformatWriter::close_video(AVFormatContext *oc,
                                    AVStream *st) {
  avcodec_close(st->codec);
  av_free(picture->data[0]);
  av_free(picture);
  if (tmp_picture) {
    av_free(tmp_picture->data[0]);
    av_free(tmp_picture);
  }
  av_free(video_outbuf);
}

void LibavformatWriter::WriteFrame(unsigned char *pFrameBuffer) {
  /* compute current video time */
  if (video_st)
    video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
  else
    video_pts = 0.0;

  int out_size, ret;
  AVCodecContext *c;
  static struct SwsContext *img_convert_ctx;
  c = video_st->codec;
  if (c->pix_fmt != PIX_FMT_RGB32) {
    /* as we only generate a RGB32 picture, we must convert it
       to the codec pixel format if needed */
    if (img_convert_ctx == NULL) {
      img_convert_ctx = sws_getContext(c->width,
                                       c->height,
                                       PIX_FMT_RGB32,
                                       c->width,
                                       c->height,
                                       c->pix_fmt,
                                       sws_flags,
                                       NULL,
                                       NULL,
                                       NULL);
      if (img_convert_ctx == NULL) {
        fprintf(stderr, "Cannot initialize the conversion context\n");
        exit(1);
      }
    }
    fill_image(tmp_picture, pFrameBuffer, c->width, c->height);
    sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
              0, c->height, picture->data, picture->linesize);
  } else {
    fill_image(picture, pFrameBuffer, c->width, c->height);
  }
  /* encode the image */
  out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
  /* if zero size, it means the image was buffered */
  if (out_size > 0) {
    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, video_st->time_base);
    if(c->coded_frame->key_frame) {
      pkt.flags |= PKT_FLAG_KEY;
      pkt.stream_index= video_st->index;
      pkt.data= video_outbuf;
      pkt.size= out_size;
      /* write the compressed frame in the media file */
      ret = av_write_frame(oc, &pkt);
    }
  } else {
    ret = 0;
  }
  if (ret != 0) {
    fprintf(stderr, "Error while writing video frame\n");
    exit(1);
  }
}

void LibavformatWriter::End() {
  /* close each codec */
  if (video_st) {
    close_video(oc, video_st);
  }
  /* write the trailer, if any */
  av_write_trailer(oc);

  /* free the streams */
  for(i = 0; i < oc->nb_streams; i++) {
    av_freep(&oc->streams[i]->codec);
    av_freep(&oc->streams[i]);
  }

  if (!(fmt->flags & AVFMT_NOFILE)) {
    /* close the output file */
    url_fclose(&oc->pb);
  }
  /* free the stream */
  av_free(oc);
}

void LibavformatWriter::fill_image(AVFrame *pict,
                                  unsigned char *pFrameBuffer,
                                  int width,
                                  int height) {
  memcpy((void*)&(pict->data[0][0]), (void*)pFrameBuffer, width*height*4);
}
}  // namespace aimc
