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

#include "Support/Common.h"

/*! \class GraphicsOutputDevicewxGLCanvas
 *
 *  Graphics output takes a large part of the application's performance at the
 *  moment when it is inline with the Process() loop. Much is gained by
 *  putting it in a separate thread, which can be done using ShotTargetThreaded.
 *
 *  OpenGL-related documents:
 *   - http://www.opengl.org/
 *   - http://www.sgi.com/products/software/opengl/
 *   - http://developer.apple.com/graphicsimaging/opengl/
 *   - http://developer.nvidia.com/page/documentation.html
 *   - Vertex arrays
 *     - http://www.opengl.org/registry/specs/EXT/vertex_array.txt
 *     - http://www.awprofessional.com/articles/article.asp?p=461848&seqNum=2&rl=1
 *     - http://jdobry.webpark.cz/opengl/opengl_maximum_performance.html
 *   - Fonts and OpenGL
 *     - http://gltt.sourceforge.net/
 */

// And finally our own
#include "Support/util.h"
#include "Output/GraphicsOutputDevice.h"
#include "Output/GraphicsOutputDevicewxGLCanvas.h"

BEGIN_EVENT_TABLE(GraphicsOutputDevicewxGLCanvas, wxGLCanvas)
    EVT_SIZE(GraphicsOutputDevicewxGLCanvas::OnSize)
    EVT_PAINT(GraphicsOutputDevicewxGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(GraphicsOutputDevicewxGLCanvas::OnEraseBackground)
END_EVENT_TABLE()

// wxGLCanvas attributes
int GraphicsOutputDevicewxGLCanvas::GLAttrlist[] = {
  WX_GL_RGBA, 1,
  WX_GL_DOUBLEBUFFER, 1,
  WX_GL_MIN_RED, 5,
  WX_GL_MIN_GREEN, 5,
  WX_GL_MIN_BLUE, 5,
  WX_GL_MIN_ALPHA, 3,
  WX_GL_DEPTH_SIZE, 16,
  0
};

// OpenGL get procaddress function pointer, differs across platforms
typedef void (*(*glGetProcAddressPtr_t)(const char*))();

GraphicsOutputDevicewxGLCanvas::GraphicsOutputDevicewxGLCanvas(Parameters *pParam,
                                                               wxWindow *parent,
                                                               wxWindowID id,
                                                               const wxPoint& pos,
                                                               const wxSize& size,
                                                               long style,
                                                               const wxString& name)
  : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size,
               style|wxFULL_REPAINT_ON_RESIZE, name, GLAttrlist),
    GraphicsOutputDevice(pParam) {
  m_init = false;
  m_gllist = 0;
  m_pWorkerContext = NULL;
  m_bAntialiasing = true;
  m_pFont = NULL;
  m_sFontFile = NULL;
  m_iFontsize = -1;
#if !defined(_MACOSX)
  s_bWorkerNeedsInit = false;
#endif

#ifdef WITH_GL_VERTEX_ARRAYS
  m_iVertexType = 0xffff; // no gBegin() has happened yet
  m_bStaticColor = false;
  m_pVertices = NULL;
  m_iVerticesMax = 0;
  // Enable vertex arrays if possible
#ifdef _MACOSX
  m_glLockArraysEXT = ::glLockArraysEXT;
  m_glUnlockArraysEXT = ::glUnlockArraysEXT;
  m_bVertexArrayLock = true;
#else
  m_bVertexArrayLock = false;
  // OpenGL command needed to fetch entry point, do it in InitGL()
#endif /* _MACOSX */
#endif /* WITH_GL_VERTEX_ARRAYS */
}

GraphicsOutputDevicewxGLCanvas::~GraphicsOutputDevicewxGLCanvas() {
  // Cleanup OpenGL display list
  if (m_init) {
    glDeleteLists(m_gllist, 1);
  }
  DELETE_IF_NONNULL(m_pWorkerContext);
  DELETE_IF_NONNULL(m_pFont);
#ifdef WITH_GL_VERTEX_ARRAYS
  DELETE_ARRAY_IF_NONNULL(m_pVertices);
#endif
}

void GraphicsOutputDevicewxGLCanvas::Start() {
  // This seems to be needed to prevent a crash on windows, but why????
  SetCurrent();
  return GraphicsOutputDevice::Start();
}

bool GraphicsOutputDevicewxGLCanvas::Initialize(unsigned int iVerticesMax) {
  AIM_ASSERT(m_pParam);
  // Give a chance to update anti-aliasing settings
  if (m_bAntialiasing != m_pParam->GetBool("output.antialias")) {
    m_bAntialiasing = m_pParam->GetBool("output.antialias");
    if (SetCurrent()) {
      InitGL();
#if !defined(_MACOSX)
      {
        wxMutexLocker lock(s_mutexOpenGL);
        s_bWorkerNeedsInit = true;
      }
#endif
    }
  }

#ifdef WITH_GL_VERTEX_ARRAYS
  // Re-allocate vertices
  if (iVerticesMax > m_iVerticesMax) {
    DELETE_IF_NONNULL(m_pVertices);
    m_iVerticesMax = iVerticesMax;
    // If color is static, we need not store the color
    if (m_bStaticColor)
      m_pVertices = new GLfloat[(iVerticesMax+1)*3];
    else
      m_pVertices = new GLfloat[(iVerticesMax+1)*6];
  }
#endif

  // Change font if requested
  const char *sFontFile = m_pParam->GetString("output.gl.fontfile");
  unsigned int iFontsize = m_pParam->GetUInt("output.fontsize");
  if (!m_sFontFile
      || !strcmp(m_sFontFile,sFontFile)==0
      || m_iFontsize!=(int)iFontsize) {
    wxMutexLocker lock(s_mutexOpenGL);
    DELETE_IF_NONNULL(m_pFont);
    wxString sWorkingFontFilename = wxString::FromAscii(sFontFile);
    if (!wxFileExists(sWorkingFontFilename)) {
      sWorkingFontFilename = wxString::FromAscii(aimDataDir());
      sWorkingFontFilename += _T("/");
      sWorkingFontFilename += wxString::FromAscii(sFontFile);
    }
    //if (!wxFileExists(sWorkingFontFilename))
    //sWorkingFontFilename.replace("Font:").append(sFontFile);
    m_pFont = static_cast<FTFont*>(new FTGLBitmapFont(sWorkingFontFilename.fn_str()));
    if (!m_pFont || m_pFont->Error()) {
      aimERROR(_T("Couldn't load font '%s'"), sFontFile);
      DELETE_IF_NONNULL(m_pFont);
    } else {
      // Display lists don't mix with our own usage :(
      // May not be needed for a Bitmap font
      //m_pFont->UseDisplayList(false);
      if ( !m_pFont->FaceSize(iFontsize) ) {
        AIM_ERROR(_T("Couldn't select font size %u on font '%s'"), iFontsize, sFontFile);
        DELETE_IF_NONNULL(m_pFont);
      }
    }
    m_sFontFile = sFontFile;
    m_iFontsize = iFontsize;
  }
  return true;
}
bool GraphicsOutputDevicewxGLCanvas::Initialize() {
  return Initialize(0);
}

void GraphicsOutputDevicewxGLCanvas::Render() {
  wxPaintDC dc(this);
  // We want to initialize first from main thread.
  if (!m_init) {
    if (!SetCurrent()) return;
    InitGL();
  }
  // Render saved list only if not animating (redrawn anyway in that case)
  if (!m_bRunning) {
    if (!SetCurrent()) {
      return;
    }
    glClear(GL_COLOR_BUFFER_BIT/*|GL_DEPTH_BUFFER_BIT*/);
    glCallList(m_gllist);
    SwapBuffers();
  }
}

void GraphicsOutputDevicewxGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event)) {
  Render();
}

void GraphicsOutputDevicewxGLCanvas::OnSize(wxSizeEvent& event) {
  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);

  // set GL viewport
  // (not called by wxGLCanvas::OnSize on all platforms...)
  if (SetCurrent()) {
    DoResize();
    // It is only sensible to update the other thread when it's running
    // Don't acquire the mutex when s_bWorkerNeedsInit already to avoid deadlock
    if (/*m_bRunning &&*/ !s_bWorkerNeedsInit) {
      wxMutexLocker lock(s_mutexOpenGL);
      s_bWorkerNeedsInit = true;
    }
  }
}

void GraphicsOutputDevicewxGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
}

bool GraphicsOutputDevicewxGLCanvas::SetCurrent() {
  bool bRet=true;

#ifndef __WXMOTIF__
  bRet = (GetContext()!=NULL);
  if (bRet)
#endif
  {
    wxGLCanvas::SetCurrent();
  }
  return bRet;
}

void GraphicsOutputDevicewxGLCanvas::DoResize() {
  int w, h;
  GetClientSize(&w, &h);
  glViewport(0, 0, (GLint)w, (GLint)h);
}

void GraphicsOutputDevicewxGLCanvas::InitGL() {
  /* No SetCurrent() here, because this can be called from different GL contexts.
   * Convenient for multi-threaded operation. */
  //aimERROR(_T("InitGL Called"));
#if defined(WITH_GL_VERTEX_ARRAYS) && !defined(_MACOSX)
  if (!m_init) {
    /* This needs to be done here, because OpenGL commands may need SetCurrent()
     * and an already shown window. */
    char *extensions = (char *)glGetString(GL_EXTENSIONS);
    if (!extensions) {
      AIM_INFO(_T("Could not query OpenGL extensions, vertex arrays disabled"));
    }  else if (strstr(extensions, "GL_EXT_compiled_vertex_array")) {
      m_glLockArraysEXT = (LOCAL_PFNGLLOCKARRAYSEXTPROC)GL_GET_PROC_ADDRESS("glLockArraysEXT");
      m_glUnlockArraysEXT = (LOCAL_PFNGLUNLOCKARRAYSEXTPROC)GL_GET_PROC_ADDRESS("glUnlockArraysEXT");
      if (!m_glLockArraysEXT || !m_glUnlockArraysEXT)
        AIM_ERROR(_T("OpenGL error on GL_EXT_compiled_vertex_array"));
      else
        m_bVertexArrayLock = true;
    }
  }
#endif
  DoResize();
  glClearColor(0, 0, 0, 1);
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );

  glEnable(GL_VERTEX_ARRAY);

  // Window limits in OpenGL co-ordiantes
  //! \todo Make this configurable, or change and document fixed values
  glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
  glTranslatef(0.0, 0.0, 0.0);

  if (m_bAntialiasing) {
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE, GL_ONE);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  } else {
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
  }
  glLineWidth(1.0);

  // Get a free display list only the first time
  if (!m_init) {
#if !defined(_MACOSX)
    // Windows and Linux need a separate worker context
    aimASSERT(wxIsMainThread());
#if wxCHECK_VERSION(2,8,0)
    m_pWorkerContext = new wxGLContext(this, m_glContext);
#else
    m_pWorkerContext = new wxGLContext(true,
                                       this,
                                       wxNullPalette,
                                       m_glContext);
#endif
    aimASSERT(m_pWorkerContext);
    s_bWorkerNeedsInit = true;
#endif
    m_gllist = glGenLists(1);
    aimASSERT(m_gllist);
    // Empty window at start
    glNewList(m_gllist, GL_COMPILE_AND_EXECUTE);
    glEndList();
    m_init = true;
  }
}

// Call before any other render* functions
void GraphicsOutputDevicewxGLCanvas::gGrab() {
  AimwxGuiLocker __lock__;
#if !defined(_MACOSX)
  // Detect if we're the main thread or not.
  if (!wxIsMainThread()) {
    // We're called by a worker thread, make sure there's a right context
    AIM_ASSERT(m_pWorkerContext);
#if wxCHECK_VERSION(2,8,0)
    m_pWorkerContext->SetCurrent(*this);
#else
    m_pWorkerContext->SetCurrent();
#endif
    // Update OpenGL settings if needed
    wxMutexLocker lock(s_mutexOpenGL);
    if (s_bWorkerNeedsInit) {
      InitGL();
      s_bWorkerNeedsInit = false;
    }
  } else
#endif
  {
    // Either called by main thread, or we need no special worker glContext
    if (!SetCurrent()) {
      return;
    }
    // Init OpenGL once, but after SetCurrent
    if (!m_init) {
      InitGL();
    }
  }
  glClear(GL_COLOR_BUFFER_BIT);

  // Start and store in a display list for redrawing
  glNewList(m_gllist, GL_COMPILE);
}

void GraphicsOutputDevicewxGLCanvas::gBeginLineStrip() {
#ifdef WITH_GL_VERTEX_ARRAYS
  aimASSERT(m_iVertexType == 0xffff); // Previous gBegin*() must be gEnd()ed
  // New lines vertex array
  m_iVertexCount = 0;
  m_iVertexType = GL_LINE_STRIP;
#else
  AimwxGuiLocker __lock__;
  glBegin(GL_LINE_STRIP);
#endif
}

void GraphicsOutputDevicewxGLCanvas::gBeginQuadStrip() {
#ifdef WITH_GL_VERTEX_ARRAYS
  aimASSERT(m_iVertexType == 0xffff); // Previous gBegin*() must be gEnd()ed
  // New quads vertex array
  m_iVertexCount = 0;
  m_iVertexType = GL_QUAD_STRIP;
#else
  AimwxGuiLocker __lock__;
  glBegin(GL_QUAD_STRIP);
#endif
}

void GraphicsOutputDevicewxGLCanvas::gVertex3f(float x, float y, float z) {
#ifdef WITH_GL_VERTEX_ARRAYS
  aimASSERT(m_iVertexType != 0xffff); // Must be inside gBegin*()
  if (m_iVertexCount>=m_iVerticesMax) {
    static bool errShown=false;
    if (!errShown) {
      aimERROR(_T("Error: max vertex count reached: %d"), m_iVertexCount);
      errShown=true;
    }
    return;
  }
  if (m_bStaticColor) {
    m_pVertices[m_iVertexCount*3+0] = x;
    m_pVertices[m_iVertexCount*3+1] = y;
    m_pVertices[m_iVertexCount*3+2] = z;
  } else {
    m_pVertices[m_iVertexCount*6+0] = m_fCurColorR;
    m_pVertices[m_iVertexCount*6+1] = m_fCurColorG;
    m_pVertices[m_iVertexCount*6+2] = m_fCurColorB;
    m_pVertices[m_iVertexCount*6+3] = x;
    m_pVertices[m_iVertexCount*6+4] = y;
    m_pVertices[m_iVertexCount*6+5] = z;
  }
  m_iVertexCount++;
#else
  AimwxGuiLocker __lock__;
  glVertex3f(x,y,z);
#endif
}

void GraphicsOutputDevicewxGLCanvas::gColor3f(float r, float g, float b) {
#ifdef WITH_GL_VERTEX_ARRAYS
  if (m_iVertexType==0xffff || m_bStaticColor) {
    // If not inside vertex array run, use the ordinary command
    glColor3f(r, g, b);
  }
  if (!m_bStaticColor) {
    // Set current color for vertex array usage
    m_fCurColorR = r;
    m_fCurColorG = g;
    m_fCurColorB = b;
  }
#else
  AimwxGuiLocker __lock__;
    glColor3f(r, g, b);
#endif
}

void GraphicsOutputDevicewxGLCanvas::gEnd() {
#ifdef WITH_GL_VERTEX_ARRAYS
  aimASSERT(m_iVertexType != 0xffff); // Must be inside gBegin*()
  AimwxGuiLocker __lock__;

  // Draw the vertex array
  glEnableClientState(GL_VERTEX_ARRAY);

  // Draw vertices
  if (m_bStaticColor)
    glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
  else
    glInterleavedArrays(GL_C3F_V3F, 0, m_pVertices);
  if (m_bVertexArrayLock) m_glLockArraysEXT(0, m_iVertexCount);
  glDrawArrays(m_iVertexType, 0, m_iVertexCount);
  if (m_bVertexArrayLock) m_glUnlockArraysEXT();

  glDisableClientState(GL_VERTEX_ARRAY);

  // Remember we're outside a gBegin()..gEnd() loop
  m_iVertexType = 0xffff;
#else
  AimwxGuiLocker __lock__;
  glEnd();
#endif
}

void GraphicsOutputDevicewxGLCanvas::gText3f(float x,
                                             float y,
                                             float z,
                                             const char *sStr,
                                             bool bRotated) {
#ifdef WITH_GL_VERTEX_ARRAYS
  aimASSERT(m_iVertexType == 0xffff); // Must be outside gBegin*()
#endif

  if (!m_pFont)
    return;

  //! \todo make rotation work
  if (bRotated)
    return;

  {
    AimwxGuiLocker __lock__;
    /*
    if (bRotated) {
      glPushMatrix();
      glTranslatef(x,y,z);
      glRotatef(90.0f, 0, 0, 1.0f);
      glRasterPos3f(0,0,0);
      m_pFont->Render(sStr);
      glPopMatrix();
    } else {
    */
    glRasterPos3f(x, y, z);
    m_pFont->Render(sStr);
  }
}

void GraphicsOutputDevicewxGLCanvas::gRelease() {
#ifdef WITH_GL_VERTEX_ARRAYS
  aimASSERT(m_iVertexType == 0xffff); // Must be gEnd()ed
#endif
  AimwxGuiLocker __lock__;
  glEndList();
  glCallList(m_gllist);
  //glFlush();
   SwapBuffers(); // Doesn't matter in what context
}
