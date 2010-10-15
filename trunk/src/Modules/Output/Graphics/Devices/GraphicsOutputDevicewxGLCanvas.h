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
 * \brief Output device for output to a wxWidgets OpenGL canvas
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/21
 * \version \$Id: $
 */

#ifndef __GRAPHICS_OUTPUT_DEVICE_GL_CANVAS_H__
#define __GRAPHICS_OUTPUT_DEVICE_GL_CANVAS_H__

// Precompiled wxWidgets headers
#include "stdwx.h"

// Make sure GLCANVAS is compiled into wxWidgets
#if !wxUSE_GLCANVAS
#  error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#if defined (_MACOSX)
#  include <OpenGL/gl.h>
#  include <OpenGl/glext.h>
#elif defined (_WINDOWS)
#  include <GL/gl.h>
#  define GL_GET_PROC_ADDRESS wglGetProcAddress
#else
#  include <GL/gl.h>
#  define GL_GET_PROC_ADDRESS(x) glXGetProcAddress((const GLubyte*)x)
#endif
/* Define them just ourselves, easiest way to get it working cross-platform
 * and -Mesa/OpenGL-version. */
#ifndef APIENTRY
#  define APIENTRY
#endif
typedef void (APIENTRY * LOCAL_PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * LOCAL_PFNGLUNLOCKARRAYSEXTPROC) (void);

#ifdef FTGL_SUBDIR
#  include <FTGL/FTGLBitmapFont.h>
#else
#  include <FTGLBitmapFont.h>
#endif

#include "Output/GraphicsOutputDevice.h"

// Use by default
#define WITH_GL_VERTEX_ARRAYS

/*!
 * \class GraphicsOutputDevicewxGLCanvas "Output/GraphicsOutputDevicewxGLCanvas.h"
 * \brief Output class for output to a wxWidgets OpenGL canvas
 *
 * On windows, OpenGL needs a different context when two different threads
 * want to issue OpenGL commands. This is handled automatically for two
 * wxThread s.
 */
class GraphicsOutputDevicewxGLCanvas : public wxGLCanvas,
                                       public GraphicsOutputDevice {
 public:
  GraphicsOutputDevicewxGLCanvas(Parameters *pParam,
                                 wxWindow *parent,
                                 wxWindowID id = wxID_ANY,
                                 const wxPoint& pos = wxDefaultPosition,
                                 const wxSize& size = wxDefaultSize,
                                 long style = 0,
                                 const wxString& name
                                     = _T("GraphicsOutputDeviceGLCanvas"));
  virtual ~GraphicsOutputDevicewxGLCanvas();
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnEraseBackground(wxEraseEvent& event);

  /*! \param iVerticesMax Maximum number of vertices to be draw inside a gBegin()..gEnd()
   *
   *  When iVerticesMax is zero, this variable will not be updated. Note that is _has_
   *  to be set at least once before using this class.
   */
  bool Initialize(unsigned int iVerticesMax);
  bool Initialize();

  void Start();

  void gGrab();
  void gBeginLineStrip();
  void gBeginQuadStrip();
  using GraphicsOutputDevice::gVertex3f; // Because we overload it
  void gVertex3f(float x, float y, float z);
  void gColor3f(float r, float g, float b);
  void gEnd();
  void gText3f(float x, float y, float z, const char *sStr, bool bRotated = false);
  void gRelease();
 protected:
  /*! \brief Smarter SetCurrent() replacement
   *  \return true on success, false on error
   *
   *  This function tries GetContext() first. If that fails, it returns false.
   */
  bool SetCurrent();
  void Render();

  /*! \brief Initialize the OpenGL environment.
   *
   *  This must be called after the canvas is realized but before any other
   *  OpenGL operation is done. Make sure to run SetCurrent() beforehand.
   *  Usually only needed when m_init is false.
   */
  void InitGL();

  /*! \brief Handle a resize (notify OpenGL of the new area)
   *
   *  This is a separate function, because in multi-threading environments
   *  multiple contexts have to call it.
   */
  void DoResize();

  /*! \brief Only need to initialize OpenGL once.
   *
   *  This is false at start and true when OpenGL has been initialized.
   *  No mutex needed, since it's set once at InitGL() and only read afterwards.
   */
  bool m_init;

  /*! \brief Vertex list for last drawing so it can be updated on repaint.
   *
   *  No mutex needed, since it's set once at InitGL() and only read afterwards.
   */
  GLuint m_gllist;

  //! \brief OpenGL context for worker thread, use when wxIsMainThread() returns false.
  wxGLContext *m_pWorkerContext;

  //! \brief Mutex for inter-thread communication
  wxMutex s_mutexOpenGL;

  //! \brief When true, OpenGL needs to be reinitialized (in the worker thread)
  bool s_bWorkerNeedsInit;

  //! \brief OpenGL attributes used for initialization.
  static int GLAttrlist[];

  //! \brief Whether to use anti-aliasing or not
  bool m_bAntialiasing;

  //! \brief FTGL Font class
  FTFont *m_pFont;
  //! \brief Current font filename
  const char *m_sFontFile;
  //! \brief Current font size
  int m_iFontsize;

#if defined(WITH_GL_VERTEX_ARRAYS) || defined(DOXYGEN)
  //! \brief OpenGL vertex type of the current m_pVertices, or 0xffff is outside gBegin()..gEnd()
  int m_iVertexType;
  //! \brief Maximum number of vertices begin gBegin()..gEnd()
  unsigned int m_iVerticesMax;
  //! \brief Vertex array to draw at gEnd(), this becomes m_pVertices[m_iVerticesMax*3]
  GLfloat *m_pVertices;
  //! \brief The current number of vertices inside m_pVertices
  unsigned int m_iVertexCount;

  /*! \brief Whether to use coloring in vertex lists
   *
   *  This variable must not change after Initialize(), or the program may crash.
   *  When this variable is true, color information is stored in vertex lists. If
   *  it is false, only vertex data is stored.
   *
   *  This variable exists for performance reasons, but is currently only set in
   *  the constructor of this object.
   */
  bool m_bStaticColor;
  //! \brief Current color for vertex list drawing
  float m_fCurColorR, m_fCurColorG, m_fCurColorB;

  //! \brief Whether to use vertex array locking or not
  bool m_bVertexArrayLock;
  //! \brief Pointer to vertex array locking function; can be NULL.
  LOCAL_PFNGLLOCKARRAYSEXTPROC m_glLockArraysEXT;
  //! \brief Pointer to vertex array unlocking function; can be NULL.
  LOCAL_PFNGLUNLOCKARRAYSEXTPROC m_glUnlockArraysEXT;
#endif

  /*! \brief wxMutexGuiEnter() / wxMutexGuiLeave() wrapper
   *
   *  This is a wxMutexLocker-alike for the main gui mutex. Any method that
   *  is public, can be called from within another thread and does OpenGL or
   *  other gui calls must use this. Example:
   *  \code
   *  void DoFoo() {
   *    AimwxGuiLocker __lock__;
   *    glAmazingMethod();
   *  }
   *  \endcode
   *
   *  It is mostly on X-Windows (Xorg/XFree86) that the gui mutex appears to
   *  be needed. Otherwise the error "Xlib: unexpected async reply" can occur.
   *
   *  On windows, the ui may occasionally lock up for a short while with these
   *  mutexes. Since they aren't really needed on that platform, it's left out
   *  alltogether.
   */
  class AimwxGuiLocker {
   public:
    inline AimwxGuiLocker() {
#ifndef _WINDOWS
      if (!wxIsMainThread()) wxMutexGuiEnter();
#endif
    }
    inline ~AimwxGuiLocker() {
#ifndef _WINDOWS
      if (!wxIsMainThread()) wxMutexGuiLeave();
#endif
    }
  };
  DECLARE_EVENT_TABLE()
};
#endif /* __GRAPHICS_OUTPUT_DEVICE_GL_CANVAS_H__ */
