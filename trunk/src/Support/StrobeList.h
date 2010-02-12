// Copyright 2007-2010, Thomas Walters
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*!
 * \file
 * \brief Modifiable List of Strobe Points - helper for SAI generation
 *
 * \author Tom Walters <tcw24@cam.ac.uk>
 * \date created 2007/08/22
 * \version \$Id: StrobeList.h 1 2010-02-02 11:04:50Z tcw $
 */

#ifndef _AIMC_STROBE_LIST_H_
#define _AIMC_STROBE_LIST_H_

#include <math.h>

#include "Support/util.h"

typedef struct StrobePoint {
  int iTime;
  float fWeight;
  float fWorkingWeight;
  StrobePoint() {
   iTime=0;
   fWeight=0.0f;
   fWorkingWeight=0.0f;
  }
};

class StrobeList;
/*!
 * \class Signal "Support/StrobeList.h"
 * \brief Modifiable List of Strobe Points, which must be ordered
 *
 */
class StrobeList {
 public:
  /*! \brief Create a new strobe list
   */
  inline StrobeList() {
    m_iStrobeCount = 0;
    m_iMaxStrobes=0;
    m_iOffset=0;
    m_pStrobes = NULL;
  };

  inline void Create(unsigned int iMaxStrobes) {
    m_iMaxStrobes = iMaxStrobes;
    m_pStrobes = new StrobePoint[m_iMaxStrobes+1];
  }

  inline ~StrobeList() {
    if (m_pStrobes != NULL)
      delete [] m_pStrobes;
  };

  //! \brief Return the strobe time (in samples, can be negative)
  inline unsigned int getTime(unsigned int iStrobeNumber) {
    return m_pStrobes[GetBufferNumber(iStrobeNumber)].iTime;
  };

  //! \brief Return the strobe weight
  inline float getWeight(unsigned int iStrobeNumber) {
    return m_pStrobes[GetBufferNumber(iStrobeNumber)].fWeight;
  };

  //! \brief Return the strobe's working weight
  inline float getWorkingWeight(unsigned int iStrobeNumber) {
    return m_pStrobes[GetBufferNumber(iStrobeNumber)].fWorkingWeight;
  };

    //! \brief Set the strobe weight
  inline void setWeight(unsigned int iStrobeNumber, float fWeight) {
    m_pStrobes[GetBufferNumber(iStrobeNumber)].fWeight = fWeight;
  };

    //! \brief Set the strobe's working weight
  inline void setWorkingWeight(unsigned int iStrobeNumber,
                               float fWorkingWeight) {
    m_pStrobes[GetBufferNumber(iStrobeNumber)].fWorkingWeight = fWorkingWeight;
  };

  //! \brief Add a strobe to the list (must be in order)
  inline void addStrobe(int iTime, float fWeight) {
    if (m_iStrobeCount + 1 <= m_iMaxStrobes) {
      m_iStrobeCount++;
      m_pStrobes[GetBufferNumber(m_iStrobeCount)].iTime=iTime;
      m_pStrobes[GetBufferNumber(m_iStrobeCount)].fWeight=fWeight;
    }
  };

  //! \brief Delete a strobe from the list
  inline void deleteFirstStrobe() {
    if (m_iStrobeCount > 0) {
      m_iOffset = (m_iOffset+1) % m_iMaxStrobes;
      m_iStrobeCount--;
    }
  };

  //! \brief Get the number of strobes
  inline unsigned int getStrobeCount() {
    return m_iStrobeCount;
  };

  //! \brief Shift the position of all strobes by subtracting iOffset from
  //! the time value of each
  inline void shiftStrobes(unsigned int iOffset) {
    for (unsigned int i=1; i<m_iStrobeCount+1; i++)
      m_pStrobes[GetBufferNumber(i)].iTime-=iOffset;
  };

 protected:

 private:
  unsigned int m_iStrobeCount;
  unsigned int m_iMaxStrobes;
  unsigned int m_iOffset;
  StrobePoint* m_pStrobes;

  inline unsigned int GetBufferNumber(unsigned int iStrobeIndex) {
    aimASSERT(((iStrobeIndex-1+m_iOffset) % m_iMaxStrobes)<m_iMaxStrobes);
    return ((iStrobeIndex-1+m_iOffset) % m_iMaxStrobes) ;
  };
};

#endif /* _AIMC_STROBE_LIST_H_ */

