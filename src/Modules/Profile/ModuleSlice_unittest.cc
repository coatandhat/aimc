// Copyright 2010, Thomas Walters
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
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2010/02/20
 * \version \$Id$
 */

#include <boost/scoped_ptr.hpp>
#include <gtest/gtest.h>

#include "Support/Parameters.h"
#include "Modules/Profile/ModuleSlice.h"

namespace aimc {
using boost::scoped_ptr;
class ModuleSliceTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    slice_.reset(new ModuleSlice(&parameters_));
  }

  // virtual void TearDown() {}

  scoped_ptr<ModuleSlice> slice_;
  Parameters parameters_;
};

TEST_F(ModuleSliceTest, SetsDefaultParameters) {
  EXPECT_EQ(false, parameters_.GetBool("slice.temporal"));
  EXPECT_EQ(true, parameters_.GetBool("slice.all"));
  EXPECT_EQ(0, parameters_.GetInt("slice.lower_index"));
  EXPECT_EQ(1000, parameters_.GetInt("slice.upper_index"));
  EXPECT_EQ(false, parameters_.GetBool("slice.normalize"));
}

TEST_F(ModuleSliceTest, DoesSomethingElse) {
}
}  // namespace aimc
