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
 * \brief Time-representation graphics view
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/26
 * \version \$Id: GraphicsViewTime.cpp 607 2008-06-25 00:14:14Z tom $
 */

#include "Support/Common.h"

#include <stdio.h>

#include "Support/SignalBank.h"
#include "Modules/Output/Graphics/GraphicsView.h"
#include "Modules/Output/Graphics/GraphicsViewTime.h"

namespace aimc {

GraphicsViewTime::GraphicsViewTime(Parameters *pParam)
  : GraphicsView(pParam) {
    module_description_ = "Graphics output.";
    module_identifier_ = "graphics_time";
    module_type_ = "output";
    module_version_ = "$Id: $";
}

GraphicsViewTime *GraphicsViewTime::Clone(GraphicsOutputDevice *pDev) {
  GraphicsViewTime *pView = new GraphicsViewTime(parameters_);
  // Copy everything
  pView->m_pAxisX->SetDisplayRange(m_pAxisX->m_fMax, m_pAxisX->m_fMin);
  pView->m_pAxisX->SetDisplayScale(m_pAxisX->m_pScale->getType());
  pView->m_pAxisY->SetDisplayRange(m_pAxisY->m_fMax, m_pAxisY->m_fMin);
  pView->m_pAxisY->SetDisplayScale(m_pAxisY->m_pScale->getType());
  pView->m_pAxisFreq->SetDisplayRange(m_pAxisFreq->m_fMax, m_pAxisFreq->m_fMin);
  pView->m_pAxisFreq->SetDisplayScale(m_pAxisFreq->m_pScale->getType());
  return pView;
}

void GraphicsViewTime::PlotAxes(const SignalBank &bank) {
  m_pDev->gColor3f(0.0f, 0.7f, 0.7f);
  // Vertical axis
  m_pDev->gBeginLineStrip();
  m_pDev->gVertex2f(m_fMarginLeft, m_fMarginBottom);
  m_pDev->gVertex2f(m_fMarginLeft, 1.0f - m_fMarginTop);
  m_pDev->gEnd();
  // Horizontal axis
  m_pDev->gBeginLineStrip();
  m_pDev->gVertex2f(m_fMarginLeft, m_fMarginBottom);
  m_pDev->gVertex2f(1.0f-m_fMarginRight, m_fMarginBottom);
  m_pDev->gEnd();

  //if (!m_bPlotLabels)
  //  return;

  // Labels
  char sTxt[80];
  snprintf(sTxt, sizeof(sTxt) / sizeof(sTxt[0]),
           _S("%s [%.0f..%.0f Hz, %s scale]"),
           m_pAxisFreq->m_sLabel ? m_pAxisFreq->m_sLabel : "",
           m_pAxisFreq->m_fMin, m_pAxisFreq->m_fMax,
           m_pAxisFreq->m_pScale->getName());
  m_pDev->gText2f(0.0025f, 0.35f, sTxt, true);
  snprintf(sTxt, sizeof(sTxt) / sizeof(sTxt[0]),
	         _S("%s [%.2f..%.2f ms, %s scale]"),
           m_pAxisX->m_sLabel ? m_pAxisX->m_sLabel : "",
           m_pAxisX->m_fMin,
           m_pAxisX->m_fMax,
           m_pAxisX->m_pScale->getName());

  m_pDev->gText2f(m_fMarginLeft, 0.0025f, sTxt, false);

  // Frame time
  snprintf(sTxt, sizeof(sTxt)/sizeof(sTxt[0]), _S("t=%.0f ms"),
           1000.0 * bank.start_time() / bank.sample_rate());
  m_pDev->gText2f(0.8f, 0.0025f, sTxt, false);
}

void GraphicsViewTime::PlotStrobes(const vector<float>& signal,
                                   const vector<int>& strobes,
                                   float sample_rate,
                                   float y_offset,
                                   float height,
                                   float x_scale,
                                   float diameter) {
  x_scale *= 1000.0 / sample_rate;
  m_pDev->gColor3f(1.0f, 0.0f, 0.0f);
  for (vector<int>::const_iterator i = strobes.begin();
       i != strobes.end(); ++i) {
    float x = *i * x_scale;
    float y = signal[*i];
    x = m_pAxisX->m_pScale->FromLinearScaled(x) + 0.5f;
    y = m_pAxisY->m_pScale->FromLinearScaled(y);

    if (x < 0.0)
      continue;

    // Now fit it into the drawing area.
    x = x * (1.0f - m_fMarginLeft - m_fMarginRight) + m_fMarginLeft;  // fit inside x-axis area
    PlotStrobe(x, y_offset, y, height, diameter);
  }
}

void GraphicsViewTime::PlotData(const vector<float> &signal,
                                float sample_rate,
                                float yOffset,
                                float height,
                                float xScale) {
  AIM_ASSERT(xScale >= 0 && xScale <= 1);
  AIM_ASSERT(height > 0  && height <= 1);
  AIM_ASSERT(yOffset >= 0 && yOffset <= 1);
  AIM_ASSERT(m_pAxisX && m_pAxisX->m_pScale);
  AIM_ASSERT(m_pAxisY && m_pAxisY->m_pScale);

  // Make sure we get time is ms as x value
  xScale *= 1000.0 / sample_rate;
  m_pDev->gColor3f(1.0f, 1.0f, 0.8f);
  BeginDataStrip();

  // Draw the signal.
  float x = 0;
  float y = 0;
  for (unsigned int i = 0; i < signal.size(); i++) {
    // Find out where to draw and do so
    x = xScale * i;
    y = signal[i];
    x = m_pAxisX->m_pScale->FromLinearScaled(x) + 0.5f;
    y = m_pAxisY->m_pScale->FromLinearScaled(y);

    if (x < 0.0)
      continue;

    // Now fit it into the drawing area
    x = x * (1.0f - m_fMarginLeft - m_fMarginRight) + m_fMarginLeft;  // fit inside x-axis area
    PlotDataPoint(x, yOffset, y, height, false);
    /* There's no point in drawing anything when x>1.0, outside of view.
     * x is continuously increasing, so we can just stop completely. We need to
     * plot this point however, to finish the final line. */
    if (x > 1.0)
      break;
  }
  // Redraw the last point in case it's needed
  PlotDataPoint(x, yOffset, y, height, true);

  m_pDev->gEnd();
}
}  // namespace aimc
