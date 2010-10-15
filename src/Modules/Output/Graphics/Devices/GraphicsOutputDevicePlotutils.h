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
 * \brief Output device for output to a graphics file using plotutils
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/10/13
 * \version \$Id: $
 */

#ifndef __GRAPHICS_OUTPUT_DEVICE_PLOTUTILS_H__
#define __GRAPHICS_OUTPUT_DEVICE_PLOTUTILS_H__

#include <stdlib.h>
#include <stdio.h>

#include <plot.h>

#include "Support/util.h"
#include "Modules/Output/Graphics/Devices/GraphicsOutputDevice.h"

/*!
 * \class GraphicsOutputDevicePlotutils "Output/GraphicsOutputDevicePlotutils.h"
 * \brief Output class for output to a graphics file using plotutils
 *
 * This class outputs a graphics operation to file. It only supports 2d though,
 * so the z-component is ignored.
 */
class GraphicsOutputDevicePlotutils : public GraphicsOutputDevice {
 public:
  GraphicsOutputDevicePlotutils(Parameters *pParam);
  virtual ~GraphicsOutputDevicePlotutils();

  /*! \brief Initializes this output device, prepares plotting tools.
   *  \param sDir Directory or filename where to put images, max length
   *         is _MAX_PATH. Must end with slash!!!
   *  \return true on success, false on failure.
   *
   *  sDir can be either a filename, in which case the output will be
   *  to that file, or a directory, in which case it will be filled
   *  with 6-digit numbered files. A new file is then created at every
   *  call to gGrab().
   *
   *  As usual, make sure to call this function before any other. If this
   *  Initialize() failed, you shouldn't try the other functions either.
   */
  bool Initialize(const char *sDir);
  bool Initialize();

  void gGrab();
  void gBeginLineStrip();
  void gBeginQuadStrip();
  using GraphicsOutputDevice::gVertex3f; // Because we overload it
  void gVertex3f(float x, float y, float z);
  void gColor3f(float r, float g, float b);
  void gEnd();
  void gText3f(float x, float y, float z, const char *sStr, bool bRotated = false);
  void gRelease();
  char* GetBuffer();
 protected:
  /*! \brief Open the file with given index for output
   *  \param index File number to open
   *  \return true on success, false on error
   *
   *  This opens a file for output and sets up the plotting library.
   */
  bool OpenFile(unsigned int index);

  //! \brief Closes a plot output file, if any is open.
  void CloseFile();

  //! \brief Internal initialisation function called by overloaded variants of Initialize()
  void Init();

  //! \brief The current output file's handle
  FILE *m_pOutputFile;
  //! \brief The plotutils plotter
  int m_iPlotHandle;
  //! \brief Output directory
  char m_sDir[PATH_MAX];
  //! \brief Current file number
  unsigned int m_iFileNumber;
  //! \brief true if this is the first vertex after gBegin()
  bool m_bIsFirstVertex;

  enum VertexType {
    VertexTypeNone,
    VertexTypeLine,
    VertexTypeQuad
  };
  //! \brief The current vertex type
  VertexType m_iVertexType;
  //! \brief Begin vertex of current quad
  float m_aPrevX[3], m_aPrevY[3];
  //! \brief Current number of quad vertices stored
  unsigned int m_iPrevVertexCount;

  //! \brief Whether to invert the colors or not
  bool m_bInvertColors;

  //! \brief The size of the memory bitmap buffer as provided by open_memstream
  size_t m_sMemoryBufferSize;

  //! \brief Are we using a memory buffer for writing to (as opposed to a file)?
  bool m_bUseMemoryBuffer;

  //! \brief Pointer to the memory buffer in question
  char *m_pMemoryBuffer;

  //! \brief Width of the bitmap
  unsigned int m_uWidth;

  //! \brief Height of the bitmap
  unsigned int m_uHeight;
};

#endif /* __GRAPHICS_OUTPUT_DEVICE_PLOTUTILS_H__ */
