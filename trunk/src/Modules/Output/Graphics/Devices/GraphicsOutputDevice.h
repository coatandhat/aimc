// Copyright 2006-2010, Willem van Engen, Thomas Walters
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

#ifndef __GRAPHICS_OUTPUT_DEVICE_H__
#define __GRAPHICS_OUTPUT_DEVICE_H__

#include "Support/Parameters.h"

namespace aimc {

/*!
 * \class GraphicsOutputDevice "Output/GraphicsOutputDevice.h"
 * \brief General output device class
 *
 * This class provides basic drawing primitives in a device-independent manner.
 * It is structured like the OpenGL interface. Following is an example drawing
 * five horizontal lines, with the colour changing between red and blue over
 * time.
 * \code
 *  GraphicsOutputDevice oOutput = static_cast<GraphicsOutputDevice>(new GraphicsOutputDeviceSomething());
 *  unsigned int a=0;
 *  ...
 *  oOutput->Start();
 *  while ( bIsRunning ) {
 *    // Start a drawing operation
 *    oOutput->gGrab();
 *    // Draw five horizontal lines
 *    for (int y=0; y<5; y++) {
 *      // Start a new line
 *      oOutput->gBegin();
 *      // Give each line it's own colour
 *      oOutput->gColor3f( (a%255)/255, 0, 1-(a%255)/255 );
 *      // Draw the line
 *      oOutput->gVertex2f(0, y);
 *      oOutput->gVertex2f(1, y);
 *      // End the line
 *      oOutput->gEnd();
 *    }
 *    oOutput->gRelease();
 *    Sleep(1);
 *    a++;
 *  }
 *  oOutput->Stop();
 * \endcode
 *
 * This class includes some basic overloading definitions to ease the
 * children's implementation. Note that the basic operations that need
 * implementation are glVertex3f(x,y,z) and glColor(r,g,b).
 */
class GraphicsOutputDevice {
 public:
  GraphicsOutputDevice(Parameters *pParam);
  virtual ~GraphicsOutputDevice() { };

  /*! \brief Initialize the module, sets up everything to Start().
   *  \return true on success, false on error
   *
   *   Initialize() needs to be called before any other function.
   *
   *  This method is called in it's form as displayed here by the GraphicsView,
   *  but you may want to setup your own Initialize(...) function with
   *  different arguments and call it yourself.
   *
   *  Thus make sure you do all memory allocations here. They can be cleaned
   *  up by the destructor. Because Initialize() may fail, it's not put in
   *  the constructor, so it can return a value.
   *
   *  \sa Module::Initialize()
   */

  /*! \overload
   *  This function reloads the parameters; make sure to have at least the
   *  function with maximum parameters called once.
   */
  virtual bool Initialize(Parameters *global_parameters) {
    global_parameters_ = global_parameters;
    return true;
  };

  /*! \brief Create a new drawing
   *  Run this before any other drawing command.
   *  \sa glRelease()
   */
  virtual void gGrab() = 0;

  //! \brief Start a new vertex group for drawing a line strip
  virtual void gBeginLineStrip() = 0;
  //! \brief Start a new vertex group for drawing a quad strip
  virtual void gBeginQuadStrip() = 0;

  /*! \brief Specify a vertex to draw
   *  \param[in] x X-coordinate of the vertex
   *  \param[in] y Y-coordinate of the vertex
   *  \param[in] z Z-coordinate of the vertex
   *  \param[in] r Red component of colour
   *  \param[in] g Green component of colour
   *  \param[in] b Blue component of colour
   *
   *  Currently, only lines are implemented.
   */
  virtual void gVertex3f(float x, float y, float z, float r, float g, float b);

  /*! \overload
   *  This will add a vertex with the last specified colour.
   */

  virtual void gVertex3f(float x, float y, float z) = 0;
  /*! \overload
   *  This will add a vertex in the 2d-plane with z=0.
   */

  virtual void gVertex2f(float x, float y, float r, float g, float b);

  /*! \overload
   *  This will add a vertex in the 2d-plane with z=0 with the last
   *  specified colour.
   */
  virtual void gVertex2f(float x, float y);

  /*! \brief Sets the current colour
   *  \param[in] r Red component
   *  \param[in] g Green component
   *  \param[in] b Blue component
   */
  virtual void gColor3f(float r, float g, float b) = 0;

  //! \brief End a vertex group
  virtual void gEnd() = 0;

  /*! \brief Render a text string
   *  \param[in] x X-coordinate of the text's alignment point
   *  \param[in] y Y-coordinate of the text's alignment point
   *  \param[in] z Z-coordinate of the text's alignment point
   *  \param[in] sStr Text to render
   *  \param[in] bRotated \c true for vertically rotated text
   *
   *  Current alignment is horizontal:left and vertical:bottom
   *  \todo Allow multiple alignment points
   */
  virtual void gText3f(float x,
                       float y,
                       float z,
                       const char *sStr,
                       bool bRotated = false) = 0;

  /*! \overload
   *  This will render a text string in the 2d-plane with z=0.
   */
  virtual void gText2f(float x,
                      float y,
                      const char *sStr,
                      bool bRight = false);

  /*! \brief Finish drawing
   *  Call this when a drawing is finished. It also makes sure that the
   *  rendering is actually done.
   *  \sa glGrab()
   */
  virtual void gRelease() = 0;

  /*! \brief Called when animation starts
   *
   *  You may wonder what Start() and Stop() do here. Some implementations
   *  may want to behave differently with respect to updating, if an
   *  animation is running or not (e.g. updating).
   */
  virtual void Start() { m_bRunning = true; }

  //! \brief Called when animation stops
  virtual void Stop()  { m_bRunning = false; }
  
  virtual void Reset(Parameters* global_parameters) = 0;
  

 protected:
  //! \brief True when animation is running
  bool m_bRunning;
  //! \brief Parameter store
  Parameters *parameters_;
  Parameters *global_parameters_;

  //! \brief Pixel Formats
  enum PixelFormat {AIM_PIX_FMT_RGB24_32, AIM_PIX_FMT_RGB24_24};
};
}  // namespace aimc
#endif /* __GRAPHICS_OUTPUT_DEVICE__ */
