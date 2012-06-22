/*!
 * \file
 * \brief Time-representation graphics view definition
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/26
 * \version \$Id: GraphicsViewTime.h 459 2007-11-08 11:50:04Z tom $
 */
/* (c) 2006, University of Cambridge, Medical Research Council
 * http://www.pdn.cam.ac.uk/groups/cnbh/aimmanual
 */
#ifndef __GRAPHICS_VIEW_TIME_H__
#define __GRAPHICS_VIEW_TIME_H__

#include "Support/SignalBank.h"
#include "Modules/Output/Graphics/Devices/GraphicsOutputDevice.h"
#include "Modules/Output/Graphics/GraphicsView.h"

namespace aimc {
/*!
 * \class GraphicsViewTime "Output/GraphicsViewTime.h"
 * \brief Time-definition graphics view class
 *
 * This plots a Signal or SignalBank in the time domain.
 */
class GraphicsViewTime : public GraphicsView {
public:
  /*! \brief Create a new view
   *  \param pParam Main parameter store
   */
  GraphicsViewTime(Parameters *pParam);
  virtual ~GraphicsViewTime() { };

  virtual GraphicsViewTime *Clone(GraphicsOutputDevice *pDev);

private:
  virtual void PlotData(const vector<float> &signal,
	                      float sample_rate,
	                      float yOffset,
	                      float height,
	                      float xScale = 1.0);
  virtual void PlotStrobes(const vector<float>& signal,
                           const vector<int>& strobes,
                           float sample_rate,
                           float y_offset,
                           float height,
                           float x_scale,
                           float diameter);
  virtual void PlotAxes(const SignalBank &pBank);
};
}  // namesapce aimc
#endif /* __GRAPHICS_VIEW_TIME_H__ */
