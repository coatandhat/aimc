// Copyright 2009-2010, Thomas Walters
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

/*! \file
 *  \brief Slaney's gammatone filterbank
 *
 *  \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2009/11/13
 *  \version \$Id$
 */

#include <vector>

#include "Support/Module.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
using std::vector;
class ModuleGammatone : public Module {
 public:
  ModuleGammatone(Parameters *params);
  virtual ~ModuleGammatone();
  //! \brief Process a buffer
  virtual void Process(const SignalBank &input);

 private:
  virtual bool InitializeInternal(const SignalBank& input);
  virtual void ResetInternal();
  vector<vector<float> > forward_;
  vector<vector<float> > back_;
  vector<vector<float> > state_;
  vector<float> centre_frequencies_;
  int num_channels_;
  float max_frequency_;
  float min_frequency_;
};
}  // namespace aimc