// Copyright 2007-2010, Thomas Walters, Willem van Engen
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
 * \brief Output device for output to a graphics file using cairo (LGPL)
 *
 * \author Tom Walters <tom@acousticscale.org> and Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2007/09/17
 * \version \$Header: $
 */

#include "Support/Common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#ifdef _WINDOWS
#  include <direct.h> // for _mkdir&_rmdir
#endif

//#include "cairo-quartz.h"

#include "Modules/Output/Graphics/Devices/GraphicsOutputDeviceCairo.h"

namespace aimc {

GraphicsOutputDeviceCairo::GraphicsOutputDeviceCairo(Parameters *pParam)
    : GraphicsOutputDevice(pParam) {
  m_bOutputFile = false;
  m_iFileNumber = 0;
  m_iVertexType = VertexTypeNone;
  m_bUseMemoryBuffer=false;
  parameters_->DefaultString("output.img.format", "png");
}

void GraphicsOutputDeviceCairo::Reset(Parameters* global_parameters) {
  Initialize(global_parameters);
}

bool GraphicsOutputDeviceCairo::Initialize(Parameters *global_parameters) {
  global_parameters_ = global_parameters;
#ifdef _WINDOWS
  string pathsep("\\");
#else
  string pathsep("/");
#endif
  directory_ = global_parameters->GetString("output_filename_base") + pathsep;
    //! \todo Make build system check for mkdtemp() to use it when available. See TODO.txt.
#ifdef _WINDOWS
  _mkdir(directory_.c_str());
#else
  mkdir(directory_.c_str(), S_IRWXU);
#endif
  InitialzeInternal();
  return true;
}

bool GraphicsOutputDeviceCairo::Initialize(string directory) {
  directory_ = directory;
  InitialzeInternal();

  /* Try to open an image to see if everything is allright. We want to avoid
   * errors in the main Process()ing loop. */
  /*if (!OpenFile(0)) {
    //! \todo Better error message that is more specific about the cause.
    LOG_ERROR(_T("Could not open output directory '%s' using graphics format '%s'."),
      directory_.c_str(), parameters_->DefaultString("output.img.format", "png"));
    return false;
  }
  CloseFile();*/

  return true;
}

/*bool GraphicsOutputDeviceCairo::Initialize() {
    Init();
    m_bUseMemoryBuffer = true;
    return(true);
}*/

void GraphicsOutputDeviceCairo::InitialzeInternal() {
   AIM_ASSERT(parameters_);

  parameters_->DefaultString("output.img.color.background", "black");

  m_bInvertColors = parameters_->DefaultBool("output.img.color.invert", "false");

  // Output size.
  m_iWidth = parameters_->DefaultInt("output.img.width", 800);
  m_iHeight = parameters_->DefaultInt("output.img.height", 600);
  
  // Cairo's RGB24 format has 32-bit pixels with the upper 8 bits unused.
  // This is not the same as the plotutils PNG format. This information is transferred by the
  // function GetPixelFormat. The pixel format is dealt with by the reciever.
  m_cSurface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
                                          m_iWidth,
                                          m_iHeight);
  m_cCr = cairo_create(m_cSurface);
  cairo_scale(m_cCr, (float)m_iWidth, (float)m_iHeight);
  // Now setup things for this plotter.
  cairo_select_font_face(m_cCr,
                         parameters_->DefaultString("output.img.fontname",
                                                    "HersheySans"),
                         CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (m_cCr, 0.02);
}

unsigned char* GraphicsOutputDeviceCairo::GetBuffer() {
  if(m_bUseMemoryBuffer)
    return (cairo_image_surface_get_data (m_cSurface));
  else
    return NULL;
}

bool GraphicsOutputDeviceCairo::OpenFile(unsigned int index) {
  const char *strPlottype = parameters_->GetString("output.img.format");
  if (!m_bUseMemoryBuffer) {
    struct stat fileinfo;
    // Get filename without trailing slash
    char filename[PATH_MAX];
    strncpy(filename, directory_.c_str(), sizeof(filename)/sizeof(filename[0]));
#ifdef _WINDOWS
    if (filename[strlen(filename)-1]=='\\') {
      filename[strlen(filename)-1]='\0';
    }
#else
    if (filename[strlen(filename)-1]=='/') {
      filename[strlen(filename)-1]='\0';
    }
#endif
    // Enumerate files it m_sDir is a directory.
    if (stat(filename, &fileinfo) == 0 && (fileinfo.st_mode & S_IFDIR)) {
      // We have a directory: enumerate with index
      snprintf(filename, sizeof(filename) / sizeof(filename[0]),
               "%s%06d.%s",
               directory_.c_str(),
               index,
               strPlottype);
      // If type is 'auto', fallback to 'png'
      if (strcmp(strPlottype, "auto")==0)
        strPlottype = "png";
    } else {
      // We have a (probably non-existant) file. Auto-detect type by extension if requested
      strncpy(filename,
              directory_.c_str(),
              sizeof(filename)/sizeof(filename[0]));
      char *pDot = strrchr(filename, '.');
      if (!pDot) {
        LOG_ERROR(_T("Please supply extension on filename when using 'auto' format: '%s'"),
                  filename);
        return false;
      }
      strPlottype = &pDot[1];
    }
    image_filename_ = filename;
    m_bOutputFile= true; //! \todo Should check that it's possible to write to the file
  }

  return true;
}

void GraphicsOutputDeviceCairo::CloseFile() {
  // And the output file
  if (m_bOutputFile) {
    cairo_surface_write_to_png(m_cSurface, image_filename_.c_str());
    m_bOutputFile = false;
  }
  cairo_set_source_rgb (m_cCr, 0.0, 0.0, 0.0);
  cairo_paint (m_cCr);
  //cairo_destroy(m_cCr);
  //cairo_surface_destroy(m_cSurface);
}

GraphicsOutputDeviceCairo::~GraphicsOutputDeviceCairo() {
  AIM_ASSERT(!m_iPlotHandle);
  CloseFile();
}

void GraphicsOutputDeviceCairo::gGrab() {
  // Open file.
  if (!OpenFile(m_iFileNumber)) {
    return;
  }
  // Setup plotting area.
  cairo_set_line_width (m_cCr, 0.001f);
  gColor3f (0.0f, 0.0f, 0.0f);
  cairo_paint (m_cCr);
  gColor3f(1.0f, 1.0f, 0.0f);
}

int GraphicsOutputDeviceCairo::GetPixelFormat() {
    return AIM_PIX_FMT_RGB24_32;
}

void GraphicsOutputDeviceCairo::gBeginLineStrip() {
  m_bIsFirstVertex = true;
  m_iVertexType = VertexTypeLine;
  //! \todo Make line width user-settable
  cairo_set_line_width (m_cCr, 0.001f);
}

void GraphicsOutputDeviceCairo::gBeginQuadStrip() {
  m_bIsFirstVertex = true;
  m_iVertexType = VertexTypeQuad;
  m_iPrevVertexCount = 0;
  cairo_set_line_width (m_cCr, 0.001f);
}

void GraphicsOutputDeviceCairo::gColor3f(float r, float g, float b) {
  if (m_bInvertColors) {
    r = 1.0 - r;
    g = 1.0 - g;
    b = 1.0 - b;
  }
  cairo_set_source_rgb (m_cCr, r, g, b);
}

void GraphicsOutputDeviceCairo::gVertex3f(float x, float y, float z) {
  switch(m_iVertexType) {
  case VertexTypeLine:
    if (m_bIsFirstVertex) {
      m_bIsFirstVertex = false;
      //pl_fmove(x, y);
      cairo_move_to(m_cCr, x, 1.0 - y);
    } else {
      //pl_fcont(x, y);
      cairo_line_to(m_cCr, x, 1.0 - y);
    }
    break;
  case VertexTypeQuad:
    /* Store vertices until we have four in a row.
     * The order of vertices when processing quads is:
     *    1-----3-----5
     *    |     |     |
     *    0-----2-----4
     */
    if (m_iPrevVertexCount >= 3) {
      // Plot this quad
      //cairo_set_source_rgb(m_cCr, 0.2, 1 - m_aPrevY[0], m_aPrevX[0]);
      cairo_rectangle (m_cCr, m_aPrevX[2],
                       1 - m_aPrevY[2], m_aPrevX[2] - m_aPrevX[0],
                       y - m_aPrevY[2]);
      cairo_fill (m_cCr);

      /*cairo_move_to(m_cCr, , );
      cairo_line_to(m_cCr, , 1 - m_aPrevY[1]);
      cairo_line_to(m_cCr, x, y);
      cairo_line_to(m_cCr, m_aPrevX[2], 1 - m_aPrevY[2]);*/

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

void GraphicsOutputDeviceCairo::gEnd() {
  if(m_iVertexType==VertexTypeLine)
    cairo_stroke (m_cCr);
  else
    cairo_fill (m_cCr);
  m_iVertexType = VertexTypeNone;
}

void GraphicsOutputDeviceCairo::gText3f(float x,
                                        float y,
                                        float z,
                                        const char *sStr,
                                        bool bRotated) {
  //cairo_text_extents_t te;
  if (bRotated) {
    cairo_rotate(m_cCr, M_PI/2);
    //cairo_move_to(m_cCr, x ,1-y);
    cairo_show_text(m_cCr, sStr);
    //cairo_identity_matrix(m_cCr);
    cairo_rotate(m_cCr, -M_PI/2);
  } else {
    cairo_move_to(m_cCr, x ,1-y);
    cairo_show_text(m_cCr, sStr);
  }
}

void GraphicsOutputDeviceCairo::gRelease() {
  AIM_ASSERT(m_iPlotHandle>0);
  CloseFile();
  // Finished this one, up to the next!
  m_iFileNumber++;
}
}  // namespace aimc
