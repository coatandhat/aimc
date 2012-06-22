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

/*!
 * \file
 * \brief General graphics view definition
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/26
 * \version \$Id: $
 */

#ifndef __GRAPHICS_VIEW_H__
#define __GRAPHICS_VIEW_H__

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"
#include "Modules/Output/Graphics/Scale/Scale.h"
#include "Modules/Output/Graphics/Devices/GraphicsOutputDevice.h"
#include "Modules/Output/Graphics/GraphAxisSpec.h"

namespace aimc {

/*!
 * \class GraphicsView "Modules/Output/Graphics/GraphicsView.h"
 * \brief General graphics view module
 *
 * This class is a general graph drawing class for one or more Signals.
 * \sa Signal, SignalBank
 */
class GraphicsView : public Module {
public:
  /*! \brief Create a new view
   *  \param params A parameter store
   *
   */
  GraphicsView(Parameters *params);
  virtual ~GraphicsView();

  /*! \brief Create a copy of this GraphicsView
   *  \param pDev Output device to bind to
   *  \return Newly created GraphicsView, to be freed by the caller.
   *  \deprecated Possibly, use Initialize()
   */
  virtual GraphicsView *Clone(GraphicsOutputDevice *pDev) = 0;

  /*! \brief (Re-)initialize this graphics view and it's device
   *  \param pParam Main parameter store to bind to
   *  \param pDev Graphics output device to draw to
   *  \return true on success, false on error
   *
   *  It is possible to call Initialize() multiple times for binding
   *  to another graphics device or updating the parameters. This method
   *  does read all parameters than can be changed at run-time.
   *
   *  One of the Initialize() functions _must_ be called before actually
   *  using it for plotting.
   */
  virtual bool InitializeInternal(const SignalBank &bank);
  virtual void ResetInternal();
  /*! \brief Set the axes' scale
   *  \param iXScale Scale type of the horizontal axis
   *  \param iYScale Scale type of the vertical axis for signal data
   *  \param iFreqScale Scale type of the vertical axis
   *  \deprecated Possibly, use AimParameters and Initialize
   *  \todo const arguments
   */
  virtual void SetAxisScale(Scale::ScaleType iXScale,
                            Scale::ScaleType iYScale,
                            Scale::ScaleType iFreqScale);


  virtual void Process(const SignalBank &bank);

 protected:
  /*! \brief Plot the data of a signal
   *  \param pSig Signal to plot
   *  \param yOffset Vertical offset (between 0 and 1), where to plot 0 signal value.
   *  \param height Height of the signal to plot (between 0 and 1)
   *  \param xScale Scaling in x-direction. 1.0 makes it cover the whole length. 0.5 only the left half.
   */
  virtual void PlotData(const vector<float> &signal,
			                  float sample_rate,
                        float yOffset,
                        float height,
                        float xScale) = 0;
  
  virtual void PlotStrobes(const vector<float>& signal,
                           const vector<int>& strobes,
                           float sample_rate,
                           float y_offset,
                           float height,
                           float x_scale,
                           float diameter) = 0;
  
  virtual void PlotStrobe(float x,
                          float y,
                          float val,
                          float height,
                          float diameter);

  /*! \brief Plot the axes for a signal bank
   *  \param pSig Signal to plot the axes for
   */
  virtual void PlotAxes(const SignalBank &bank) = 0;

  //! \brief Calls the correct m_pDev->gBegin*() for the current PlotType
  virtual void BeginDataStrip();
  /*! \brief Plot a data point (with smart reduction)
   *  \param x X-coordinate of point (in OpenGL space)
   *  \param y y-coordinate of point (in OpenGL space)
   *  \param val Value to plot (from -0.5..0.5)
   *  \param height Height to use for plotting (in OpenGL space)
   *  \param isLast True if this is the last point of this batch to draw
   *
   *  This method tries to reduce the number of actual graphics output
   *  operations using it's interpolation methods (e.g., a line).
   */
  virtual void PlotDataPoint(float x,
                             float y,
                             float val,
                             float height,
                             bool isLast);

  /*! \brief Plot a data point (directly)
   *  \param x X-coordinate of point (in OpenGL space)
   *  \param y y-coordinate of point (in OpenGL space)
   *  \param val Value to plot (from -0.5..0.5)
   *  \param height Height to use for plotting (in OpenGL space)
   */
  virtual void PlotDataPointDirect(float x, float y, float val, float height);

  //! \brief Where to plot to
  GraphicsOutputDevice *m_pDev;

  //! \brief Axes specifications
  GraphAxisSpec *m_pAxisX, *m_pAxisY, *m_pAxisFreq;

  //! \brief Graph margins
  float m_fMarginLeft, m_fMarginRight, m_fMarginTop, m_fMarginBottom;

  //! \brief Whether labels are plotted or not
  bool m_bPlotLabels;

  //! \brief Graphics device type
  enum GraphType {
    GraphTypeNone,
    GraphTypeLine,
    GraphTypeColormap
  };
  GraphType m_iGraphType;

  /*! \brief Minimum distance between subsequent values for plotting a point
   *
   *  This value is set in Initialize() from parameter graph.mindistance.
   */
  float m_fMinPlotDistance;

  //! \brief true if this next point is the first of the strip
  bool m_bFirstPoint;
  //! \brief Value of previously plotted point
  float m_fPrevVal, m_fPrevX, m_fPrevY, m_fPrevHeight;
  //! \brief Number of times m_fValPrev was within range m_fMinPlotDistance
  int m_iPrevValEqual;
  float frame_rate_;
  int previous_start_time_;

  bool initialized_;
  bool plotting_strobes_;
};
}  // namespace aimc
#endif /* __GRAPHICS_VIEW_H__ */
