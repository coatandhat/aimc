// Copyright 2010, Thomas Walters
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
