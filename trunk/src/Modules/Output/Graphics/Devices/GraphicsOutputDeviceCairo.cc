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

#include "Support/util.h"
#include "Output/GraphicsOutputDeviceCairo.h"

GraphicsOutputDeviceCairo::GraphicsOutputDeviceCairo(Parameters *pParam)
	: GraphicsOutputDevice(pParam) {
	m_bOutputFile = false;
	m_iFileNumber = 0;
	m_iVertexType = VertexTypeNone;
	m_bUseMemoryBuffer=false;
}

bool GraphicsOutputDeviceCairo::Initialize(const char *sDir) {
  Init();

	//! \todo Output to file if sDir is a file, to directory with
  //! multiple images if it's a directory.
	strncpy(m_sDir, sDir, sizeof(m_sDir)/sizeof(m_sDir[0]));

	/* Try to open an image to see if everything is allright. We want to avoid
	 * errors in the main Process()ing loop. */
	if ( !OpenFile(0) ) {
		//! \todo Better error message that is more specific about the cause.
		AIM_ERROR(_T("Could not open output directory '%s' using graphics format '%s'."),
			m_sDir, m_pParam->GetString("output.img.format") );
		return false;
	}
	CloseFile();

	return true;
}

bool GraphicsOutputDeviceCairo::Initialize() {
    Init();
    m_bUseMemoryBuffer = true;
    return(true);
}

void GraphicsOutputDeviceCairo::Init() {
 	AIM_ASSERT(m_pParam);
	/*
	 * Set parameters
	 */
	m_pParam->GetString("output.img.color.background");

	m_bInvertColors = m_pParam->GetBool("output.img.color.invert");

	// Output size!
	m_iWidth = m_pParam->GetUInt("output.img.width");
	m_iHeight = m_pParam->GetUInt("output.img.height");
}

unsigned char* GraphicsOutputDeviceCairo::GetBuffer() {
  if(m_bUseMemoryBuffer)
    return (cairo_image_surface_get_data (m_cSurface));
  else
    return NULL;
}

bool GraphicsOutputDeviceCairo::OpenFile(unsigned int index) {
  const char *strPlottype = m_pParam->GetString("output.img.format");
  if (!m_bUseMemoryBuffer) {
    struct stat fileinfo;
    // Get filename without trailing slash
    strncpy(m_sFilename, m_sDir, sizeof(m_sFilename)/sizeof(m_sFilename[0]));
#ifdef _WINDOWS
    if (m_sFilename[strlen(m_sFilename)-1]=='\\') {
      m_sFilename[strlen(m_sFilename)-1]='\0';
    }
#else
    if (m_sFilename[strlen(m_sFilename)-1]=='/') {
      m_sFilename[strlen(m_sFilename)-1]='\0';
    }
#endif
    // Enumerate files it m_sDir is a directory.
    if (stat(m_sFilename, &fileinfo) == 0 && (fileinfo.st_mode & S_IFDIR)) {
      // We have a directory: enumerate with index
      snprintf(m_sFilename, sizeof(m_sFilename)/sizeof(m_sFilename[0]),"%s%06d.%s",
               m_sDir,
               index,
               strPlottype);
      // If type is 'auto', fallback to 'png'
      if (strcmp(strPlottype, "auto")==0)
        strPlottype = "png";
    } else {
      // We have a (probably non-existant) file. Auto-detect type by extension if requested
      strncpy(m_sFilename, m_sDir, sizeof(m_sFilename)/sizeof(m_sFilename[0]));
      char *pDot = strrchr(m_sFilename, '.');
      if (!pDot) {
        AIM_ERROR(_T("Please supply extension on filename when using 'auto' format: '%s'"),
                  m_sFilename);
        return false;
      }
      strPlottype = &pDot[1];
    }
    m_bOutputFile= true; //! \todo Should check that it's possible to write to the file
  }
  // Cairo's RGB24 format has 32-bit pixels with the upper 8 bits unused.
  // This is not the same as the plotutils PNG format. This information is transferred by the
  // function GetPixelFormat. The pixel format is dealt with by the reciever.
  m_cSurface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,
                                           m_iWidth,
                                           m_iHeight);
  m_cCr = cairo_create (m_cSurface);
  cairo_scale(m_cCr, (float)m_iWidth, (float)m_iHeight);
	// Now setup things for this plotter.
	cairo_select_font_face(m_cCr,
                         m_pParam->GetString("output.img.fontname"),
                         CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (m_cCr, 0.015);
	return true;
}

void GraphicsOutputDeviceCairo::CloseFile() {
	// Plotting library
	if (m_iPlotHandle>0) {
	    cairo_destroy(m_cCr);
		m_iPlotHandle = 0;
	}
	// And the output file
	if (m_bOutputFile) {
	  cairo_surface_write_to_png(m_cSurface, m_sFilename);
		m_bOutputFile = false;
	}
	cairo_surface_destroy(m_cSurface);
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
		r = 1-r;
		g = 1-g;
		b = 1-b;
	}
  cairo_set_source_rgb (m_cCr, r, g, b);
}

void GraphicsOutputDeviceCairo::gVertex3f(float x, float y, float z) {
	switch(m_iVertexType) {
	case VertexTypeLine:
		if (m_bIsFirstVertex) {
			m_bIsFirstVertex = false;
			//pl_fmove(x, y);
			cairo_move_to(m_cCr, x, 1-y);
		} else {
			//pl_fcont(x, y);
			cairo_line_to(m_cCr, x, 1-y);
		}
		break;
	case VertexTypeQuad:
		/* Store vertices until we got four in a row.
		 * The order of vertices when processing quads is:
		 *    1-----3-----5
		 *    |     |     |
		 *    0-----2-----4
		 */
		if (m_iPrevVertexCount >= 3) {
			// Plot this quad
			cairo_move_to(m_cCr, m_aPrevX[0], 1-m_aPrevY[0]);
			cairo_line_to(m_cCr, m_aPrevX[1], 1-m_aPrevY[1]);
			cairo_line_to(m_cCr, x, y);
			cairo_line_to(m_cCr, m_aPrevX[2], 1-m_aPrevY[2]);
			cairo_close_path (m_cCr);

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
  cairo_text_extents_t te;
	if (bRotated) {
	  cairo_rotate(m_cCr, M_PI/2);
		cairo_move_to(m_cCr, x ,1-y);
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
