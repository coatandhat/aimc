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
 * \author Tom Walters <tom@acousticscale.org>
 * \date created 2007/08/22
 * \version \$Id: StrobeList.h 1 2010-02-02 11:04:50Z tcw $
 */

#ifndef _AIMC_SUPPORT_STROBE_LIST_H_
#define _AIMC_SUPPORT_STROBE_LIST_H_

#include <math.h>
#include <deque>

namespace aimc {
using std::deque;
struct StrobePoint {
  int time;
  float weight;
  float working_weight;
  StrobePoint() {
    time = 0;
    weight = 0.0f;
    working_weight = 0.0f;
  }
};

class StrobeList {
 public:
  /*! \brief Create a new strobe list
   */
  inline StrobeList() {
    strobes_.resize(0);
  };

  inline ~StrobeList() {
  };

  /*! \brief Return the strobe time (in samples, can be negative)
   */
  inline StrobePoint Strobe(int strobe_number) {
    return strobes_.at(strobe_number);
  };

  /*! \brief Set the strobe weight
   */
  inline void SetWeight(int strobe_number, float weight) {
    strobes_.at(strobe_number).weight = weight;
  };

  /*! \brief Set the strobe's working weight
   */
  inline void SetWorkingWeight(int strobe_number, float working_weight) {
    strobes_.at(strobe_number).working_weight = working_weight;
  };

  /*! \brief Add a strobe to the list (must be in order)
   */
  inline void AddStrobe(int time, float weight) {
    StrobePoint s;
    s.time = time;
    s.weight = weight;
    strobes_.push_back(s);
  };

  /*! \brief Delete a strobe from the list
   */
  inline void DeleteFirstStrobe() {
    strobes_.pop_front();
  };

  /*! \brief Get the number of strobes
   */
  inline int strobe_count() const {
    return strobes_.size();
  };

  /*! \brief Shift the position of all strobes by subtracting offset from
   *  the time value of each
   */
  inline void ShiftStrobes(int offset) {
    for (unsigned int i = 0; i < strobes_.size(); ++i)
      strobes_[i].time -= offset;
  };

 private:
  deque<StrobePoint> strobes_;
};
}  // namespace aimc

#endif /* _AIMC_STROBE_LIST_H_ */

