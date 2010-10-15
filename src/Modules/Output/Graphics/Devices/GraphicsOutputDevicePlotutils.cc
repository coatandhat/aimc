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
 * \version \$Id: GraphicsOutputDevicePlotutils.cpp 493 2007-11-27 10:59:20Z tom $
 */
#include "Support/Common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "Support/util.h"
#include "Modules/Output/Graphics/Devices/GraphicsOutputDevicePlotutils.h"

GraphicsOutputDevicePlotutils::GraphicsOutputDevicePlotutils(Parameters *pParam)
    : GraphicsOutputDevice(pParam) {
  m_iPlotHandle = 0;
  m_pOutputFile = NULL;
  m_iFileNumber = 0;
  m_iVertexType = VertexTypeNone;
  m_bUseMemoryBuffer = false;
  m_pMemoryBuffer = NULL;
  m_uWidth=0;
  m_uHeight=0;
}

bool GraphicsOutputDevicePlotutils::Initialize(const char *sDir) {
  Init();

  //! \todo Output to file if sDir is a file, to directory with multiple
  //! images if it's a directory.
  strncpy(m_sDir, sDir, sizeof(m_sDir)/sizeof(m_sDir[0]));

  /* Try to open an image to see if everything is allright. We want to avoid
   * errors in the main Process()ing loop. */
  if (!OpenFile(0)) {
    //! \todo Better error message that is more specific about the cause.
    AIM_ERROR(_T("Could not open output directory '%s' using graphics format '%s'."),
             m_sDir, m_pParam->GetString("output.img.format") );
    return false;
  }
  CloseFile();
  return true;
}

void GraphicsOutputDevicePlotutils::Init() {
  AIM_ASSERT(m_pParam);
  /*
   * Set parameters
   */
  pl_parampl("BG_COLOR", (void*)m_pParam->GetString("output.img.color.background"));
  // Handle GIFs as other output formats, don't merge frames into single GIF.
  pl_parampl("GIF_ANIMATION", (void*)"no");
  pl_parampl("INTERLACE", (void*)"no");

  m_bInvertColors = m_pParam->GetBool("output.img.color.invert");

  // Output size!
  m_uWidth = m_pParam->GetUInt("output.img.width");
  m_uHeight = m_pParam->GetUInt("output.img.height");
  char strSize[100];
  snprintf(strSize, sizeof(strSize)/sizeof(strSize[0]), "%ux%u", m_uWidth, m_uHeight);
  pl_parampl("BITMAPSIZE", (void*)strSize);
}

bool GraphicsOutputDevicePlotutils::Initialize() {
  Init();
  m_bUseMemoryBuffer = true;
  return(true);
}

bool GraphicsOutputDevicePlotutils::OpenFile(unsigned int index) {
  const char *strPlottype = m_pParam->GetString("output.img.format");
  if (!m_bUseMemoryBuffer) {
    char sFilename[PATH_MAX];
    struct stat fileinfo;

    // Get filename without trailing slash
    strncpy(sFilename, m_sDir, sizeof(sFilename)/sizeof(sFilename[0]));
#ifdef _WINDOWS
    if (sFilename[strlen(sFilename)-1]=='\\') {
      sFilename[strlen(sFilename)-1]='\0';
    }
#else
    if (sFilename[strlen(sFilename)-1]=='/') {
      sFilename[strlen(sFilename)-1]='\0';
    }
#endif
    // Enumerate files it m_sDir is a directory.
    if ( stat(sFilename, &fileinfo)==0 && (fileinfo.st_mode & S_IFDIR) ) {
      // We have a directory: enumerate with index
      snprintf(sFilename, sizeof(sFilename)/sizeof(sFilename[0]),"%s%06d.%s", m_sDir, index, strPlottype);
      // If type is 'auto', fallback to 'png'
      if (strcmp(strPlottype, "auto")==0)
          strPlottype = "png";
      } else {
        // We have a (probabely non-existant) file. Auto-detect type by extension if requested
        strncpy(sFilename, m_sDir, sizeof(sFilename)/sizeof(sFilename[0]));
        char *pDot = strrchr(sFilename, '.');
        if (!pDot) {
          AIM_ERROR(_T("Please supply extension on filename when using 'auto' format: '%s'"), sFilename);
          return false;
        }
        strPlottype = &pDot[1];
      }
      if ((m_pOutputFile = fopen(sFilename, "wb")) == NULL) {
        return false;
      }
    } else {
#if !defined(_WINDOWS) && !defined(_MACOSX)
    if ((m_pOutputFile=open_memstream(&m_pMemoryBuffer, &m_sMemoryBufferSize))==NULL)
#endif
      return false;
      strPlottype="pnm"; // Force pnm format or this doesn't work
    }

  /*
   * Create a plotter
   *
   * Plotutils knows the following types for file output:
   *   pnm gif ai ps gif pcl hpgl tek meta
   */
  if ((m_iPlotHandle = pl_newpl(strPlottype,
                                NULL,
                                m_pOutputFile,
                                stderr)) < 0 ) {
    return false;
  }
  pl_selectpl(m_iPlotHandle);

  if ( pl_openpl() < 0) {
    return false;
  }

  // Now setup things for this plotter
  pl_fontname(m_pParam->GetString("output.img.fontname"));
  //! \todo Make fontsize work in Plotutils, currently disabled
  //pl_ffontsize(m_pParam->GetUInt("output.fontsize"));

  return true;
}

char* GraphicsOutputDevicePlotutils::GetBuffer() {
  if(m_bUseMemoryBuffer && (m_pMemoryBuffer!=NULL))
    return (&m_pMemoryBuffer[m_sMemoryBufferSize-(m_uWidth*m_uHeight*3)]);
  else
    return NULL;
}

void GraphicsOutputDevicePlotutils::CloseFile() {
  // Plotting library
  if (m_iPlotHandle>0) {
    pl_closepl();

    pl_selectpl(0);
    pl_deletepl(m_iPlotHandle);
    m_iPlotHandle = 0;
  }

  // And the output file
  if (m_pOutputFile) {
    fclose(m_pOutputFile);
    m_pOutputFile = NULL;
  }
}

GraphicsOutputDevicePlotutils::~GraphicsOutputDevicePlotutils() {
  // Output file should be closed by gRelease()
  AIM_ASSERT(!m_pOutputFile);
  AIM_ASSERT(!m_iPlotHandle);
  CloseFile();
}

PixelFormat GraphicsOutputDevicePlotutils::GetPixelFormat() {
    return AIM_PIX_FMT_RGB24_24;
}

void GraphicsOutputDevicePlotutils::gGrab() {
  // Open file
  if (!OpenFile(m_iFileNumber)) {
    return;
  }
  // Setup plotting area
  pl_fspace(0.0, 0.0, 1.0, 1.0);
  pl_flinewidth(0.0001);
  pl_pencolorname("darkblue");
  pl_erase();
}

void GraphicsOutputDevicePlotutils::gBeginLineStrip() {
  m_bIsFirstVertex = true;
  m_iVertexType = VertexTypeLine;
  pl_filltype(0);
}

void GraphicsOutputDevicePlotutils::gBeginQuadStrip() {
  m_bIsFirstVertex = true;
  m_iVertexType = VertexTypeQuad;
  m_iPrevVertexCount = 0;
  pl_filltype(1);
}

void GraphicsOutputDevicePlotutils::gColor3f(float r, float g, float b) {
  if (m_bInvertColors) {
    r = 1-r;
    g = 1-g;
    b = 1-b;
  }
  int ir = (int)(r*0xffff);
  int ig = (int)(g*0xffff);
  int ib = (int)(b*0xffff);
  ir = MIN(0xffff, MAX(0,ir));
  ig = MIN(0xffff, MAX(0,ig));
  ib = MIN(0xffff, MAX(0,ib));
  pl_color(ir, ig, ib);
}

void GraphicsOutputDevicePlotutils::gVertex3f(float x, float y, float z) {
  switch(m_iVertexType) {
  case VertexTypeLine:
    if (m_bIsFirstVertex) {
      m_bIsFirstVertex = false;
      pl_fmove(x, y);
    } else {
      pl_fcont(x, y);
    }
    break;
  case VertexTypeQuad:
    /* Store vertices until we got four in a row.
     * The order of vertices when processing quads is:
     *    1-----3-----5
     *    |     |     |
     *    0-----2-----4
     */
    if (m_iPrevVertexCount>=3) {
      // Plot this quad
      pl_fmove(m_aPrevX[0],m_aPrevY[0]);
      pl_fcont(m_aPrevX[1],m_aPrevY[1]);
      pl_fcont(x,y);
      pl_fcont(m_aPrevX[2],m_aPrevY[2]);
      pl_endpath();
      // Last vertices of this quad are the first of the next
      m_aPrevX[0] = m_aPrevX[2];
      m_aPrevY[0] = m_aPrevY[2];
      m_aPrevX[1] = x;
      m_aPrevY[1] = y;
      m_iPrevVertexCount = 2;
    } else {
      // Not at the fourth, keep storing
      m_aPrevX[m_iPrevVertexCount] = x;
      m_aPrevY[m_iPrevVertexCount] = y;
      m_iPrevVertexCount++;
    }
    break;
  default:
    // Should not happen
    AIM_ASSERT(0);
  }
}

void GraphicsOutputDevicePlotutils::gEnd() {
  pl_endpath();
  m_iVertexType = VertexTypeNone;
}

void GraphicsOutputDevicePlotutils::gText3f(float x,
                                            float y,
                                            float z,
                                            const char *sStr,
                                            bool bRotated) {
  if (bRotated) {
    pl_textangle(90);
    pl_fmove(x, y);
    pl_alabel('l', 't', sStr);
  } else {
    pl_textangle(0);
    pl_fmove(x, y);
    pl_alabel('l', 'b', sStr);
  }
}

void GraphicsOutputDevicePlotutils::gRelease() {
  AIM_ASSERT(m_pOutputFile);
  AIM_ASSERT(m_iPlotHandle>0);
  CloseFile();
  m_iFileNumber++;
}
