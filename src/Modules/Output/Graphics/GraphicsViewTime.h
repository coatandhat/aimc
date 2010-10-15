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

#include "Support/Signal.h"
#include "Support/SignalBank.h"
#include "Output/GraphicsOutputDevice.h"
#include "Output/GraphicsView.h"

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
  GraphicsViewTime(AimParameters *pParam);
  virtual ~GraphicsViewTime() { };

  virtual GraphicsViewTime *Clone(GraphicsOutputDevice *pDev);

private:
  void PlotData(Signal* pSig, float yOffset, float height, float xScale = 1.0);
  void PlotAxes(Signal* pSig);
  void PlotAxes(SignalBank* pBank);

};

#endif /* __GRAPHICS_VIEW_TIME_H__ */
