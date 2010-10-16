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

#ifndef __GRAPH_AXIS_SPEC_H__
#define __GRAPH_AXIS_SPEC_H__

#include "Support/Parameters.h"
#include "Modules/Output/Graphics/Scale/Scale.h"

namespace aimc {

/*! \class GraphAxisSpec "Output/GraphAxisSpec.h"
 *  \brief Axis specification for a GraphicsView
 */
class GraphAxisSpec {
public:
  /*! \brief Create a new GraphAxisSpec
   *  \param fMin Minimum value on the axis to show
   *  \param fMax Maximum value on the axis to show
   *  \param iScale Scale to use
   *
   *  Please see SetScale() and SetRange() for more details.
   */
  GraphAxisSpec(float fMin, float fMax, Scale::ScaleType iScale);
  //! \brief Create a new GraphAxisSpec from defaults
  GraphAxisSpec();

  ~GraphAxisSpec();

  /*! \brief Set the scale to use
   *  \param iScale Scale to use
   */
  void SetDisplayScale(Scale::ScaleType iScale);
  /*! \brief Set the minumum and maximum values to show on the axis
   *  \param fMin Minimum value on the axis to show
   *  \param fMax Maximum value on the axis to show
   *
   *  Either fMin _must_ be smaller than fMax, or fMin==fMax but then
   *  this function must be called later to fulfil the former condition
   *  before it is used to scale data.
   */
  void SetDisplayRange(float fMin, float fMax);
  /*! \brief Set the label of this axis
   *  \param sLabel New label, or NULL to remove label
   */
  void SetLabel(const char *sLabel);

  /*! \brief Read axis specification from parameter store
   *  \param pParam Parameter store to read from
   *  \param sPrefix Prefix to use, e.g. "view.x"
   *  \param fMin Default minumum value for 'auto'
   *  \param fMax Default maximum value for 'auto'
   *  \param iScale Default scale for 'auto'
   *  \return true if no error occured in reading
   */
  bool Initialize(Parameters *pParam,
                  const char *sPrefix,
                  float fMin,
                  float fMax,
                  Scale::ScaleType iScale);

protected:
  //! \brief Minimum value on the axis to display
  float m_fMin;
  //! \brief Maximum value on the axis to display
  float m_fMax;
  //! \brief Scale to use
  Scale *m_pScale;
  //! \brief Axis label, NULL for no label
  const char *m_sLabel;

  friend class GraphicsView;
  friend class GraphicsViewTime;
};

}  // namespace aimc

#endif /* __GRAPH_AXIS_SPEC_H__ */
