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

#include "Support/Common.h"

#include "Output/GraphicsView.h"
#include "Output/GraphicsOutputDevice.h"

GraphicsView::GraphicsView(Parameters *parameters) : Module(parameters) {
  module_description_ = "";
  module_identifier_ = "graphics";
  module_type_ = "output";
  module_version_ = "$Id: $";
  
  m_pDev = NULL;
  m_bPlotLabels = false;
  m_pAxisX = new GraphAxisSpec();
  AIM_ASSERT(m_pAxisX);
  m_pAxisY = new GraphAxisSpec();
  AIM_ASSERT(m_pAxisY);
  m_pAxisFreq = new GraphAxisSpec();
  AIM_ASSERT(m_pAxisFreq);

  m_pAxisY->Initialize(m_pParam,
                       _S("graph.y"),
                       -1,
                       1,
                       Scale::SCALE_LINEAR);
  m_fMarginLeft = m_pParam->GetFloat(_S("graph.margin.left"));
  m_fMarginRight = m_pParam->GetFloat(_S("graph.margin.right"));
  m_fMarginTop = m_pParam->GetFloat(_S("graph.margin.top"));
  m_fMarginBottom = m_pParam->GetFloat(_S("graph.margin.bottom"));
  m_bPlotLabels = m_pParam->GetBool(_S("graph.plotlabels"));

  const char *sGraphType = m_pParam->GetString(_S("graph.type"));
  if (strcmp(sGraphType, _S("line"))==0)
    m_iGraphType = GraphTypeLine;
  else if (strcmp(sGraphType, _S("colormap"))==0)
    m_iGraphType = GraphTypeColormap;
  else if (strcmp(sGraphType, _S("none"))==0)
    m_iGraphType = GraphTypeNone;
  else {
    ret = false;
    AIM_ERROR(_T("Unrecognized graph type: '%s'"), sGraphType);
  }

  if (strcmp(m_pParam->GetString(_S("graph.mindistance")),"auto") == 0)
    // -1 means detect later, based on type and Fire() argument
    m_fMinPlotDistance = -1;
  else
    m_fMinPlotDistance = m_pParam->GetFloat(_S("graph.mindistance"));

}

GraphicsView::~GraphicsView() {
  DELETE_IF_NONNULL(m_pAxisX);
  DELETE_IF_NONNULL(m_pAxisY);
  DELETE_IF_NONNULL(m_pAxisFreq);
}

bool

bool GraphicsView::InitializeInternal(const SignalBank &bank) {
  if (!m_pDev) {
    LOG_ERROR("Output device not connected");
    return false;
  }

  float y_min = bank.centre_frequency(0);
  float y_max = bank.centre_frequency(bank.channel_count() - 1);
  if (!m_pAxisFreq->Initialize(m_pParam,
                               "graph.freq",
                               y_min,
                               y_max,
                               Scale::SCALE_ERB)) {
    LOG_ERROR("");
    return false;
  }

  float x_min = 0.0;
  float x_max = 1000.0 * bank.buffer_length() / bank.sample_rate();
  if (!m_pAxisX->Initialize(m_pParam,
                            "graph.x",
                            x_min,
                            x_max,
                            Scale::SCALE_LINEAR)) {
     LOG_ERROR("");
     return false;
  }

  /* Inform graphics output of maximum number of vertices between
   * gBegin*() and gEnd(), for any type of plot. Colormap needs most.
   */
  if (!m_pDev->Initialize(MAX(10, bank.buffer_length() * 2 + 2))) {
    LOG_ERROR("");
    return false;
  }
}

void GraphicsView::Process(const SignalBank &bank) {
  float height = 1.0 / bank.channel_count();
  float heightMinMargin = height * (1.0f - m_fMarginBottom - m_fMarginTop);
  float xScaling = 1.0f;

  m_pDev->gGrab();
  PlotAxes(bank);
  m_pDev->gColor3f(1.0f, 1.0f, 0.8f);
  for (int i = 0; i < bank.channel_count(); i++) {
    float yOffs = bank.centre_frequency(i);
    yOffs = m_pAxisFreq->m_pScale->FromLinearScaled(yOffs) + 0.5f;
    /* Don't plot below zero and stop when above 1, since yOffs is
     * monotonically increasing. Because of rounding errors, we need
     * to check for yOffs < -1e-6 instead of yOffs < 0. */
    if (yOffs < -1e-6)
      continue;
    if (yOffs > 1)
      break;

    // Scale to single channel graphing.
    yOffs = yOffs * (1.0f - height) + height / 2.0;
    yOffs = yOffs * (1.0f - m_fMarginTop - m_fMarginBottom) + m_fMarginBottom;
    PlotData(bank[i], yOffs, heightMinMargin, xScaling);
  }
  m_pDev->gRelease();
}

void GraphicsView::SetAxisScale(Scale::ScaleType iHori,
                                Scale::ScaleType iVert,
                                Scale::ScaleType iFreq) {
  AIM_ASSERT(m_pAxisX);
  AIM_ASSERT(m_pAxisY);
  AIM_ASSERT(m_pAxisFreq);
  m_pAxisX->SetDisplayScale(iHori);
  m_pAxisY->SetDisplayScale(iVert);
  m_pAxisFreq->SetDisplayScale(iFreq);
}

void GraphicsView::BeginDataStrip() {
  m_iPrevValEqual=0;
  m_bFirstPoint=true;
  switch(m_iGraphType) {
  case GraphTypeLine:
    m_pDev->gBeginLineStrip();
    break;
  case GraphTypeColormap:
    m_pDev->gBeginQuadStrip();
    break;
  case GraphTypeNone:
    // Nothing: just for testing computation overhead of graphing
    break;
  }
}

void GraphicsView::PlotDataPoint(float x,
                                 float y,
                                 float val,
                                 float height,
                                 bool isLast) {
  AIM_ASSERT(m_pDev);

  /* Reduce the number of points plotted by eliminating duplicate values:
   *
   *             oooo                         o--o
   *                 o                       /    o
   *        oooo       ooo       =>      o--o      o--o
   *    oooo              ooo        o--o              o-o
   *
   * with 'o' points that are plotted, and '-' by the graphics output
   * device interpolated points. We could be smarter and include
   * first-order interpolation, but we leave that as an exercise for
   * the reader. Please send your patches :)
   *
   * So, we don't draw points that are too close to the previous value.
   * But if the value changes (or it's the last point), we must draw the
   * previous point too.
   */
  if (!m_bFirstPoint
      && !isLast
      && fabs(m_fPrevVal-val) < m_fMinPlotDistance) {
    m_iPrevValEqual++;
    // Don't set m_fPrevVal to avoid not catching slow changes
    m_fPrevX = x;
    m_fPrevY = y;
    m_fPrevHeight = height;
    return;
  } else {
    if (m_iPrevValEqual > 0) {
      // Draw previous point
      PlotDataPointDirect(m_fPrevX, m_fPrevY, m_fPrevVal, m_fPrevHeight);
    }
    m_iPrevValEqual = 0;
    m_fPrevVal = val;
    m_bFirstPoint = false;
  }
  PlotDataPointDirect(x, y, val, height);
}

void GraphicsView::PlotDataPointDirect(float x,
                                       float y,
                                       float val,
                                       float height) {
  // Draw it in the right way
  switch(m_iGraphType) {
  case GraphTypeLine:
    m_pDev->gVertex2f(x, y + val * height);
    break;
  case GraphTypeColormap:
    //! \todo make it a real colormap instead of grayscale
    m_pDev->gColor3f(val + 0.5, val + 0.5, val + 0.5);
    m_pDev->gVertex2f(x, y - height / 2);
    m_pDev->gVertex2f(x, y + height / 2);
    break;
  case GraphTypeNone:
    // Nothing: just for testing computation overhead of graphing
    break;
  default:
    // Shouldn't happen
    AIM_ASSERT(0);
  }
}
