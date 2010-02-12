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

/*! \file
 *  \brief Base class for all AIM-C modules.
 */

/*! The module construcor is called with a pointer to a set of Parameters.
 * In the constructor, the module sets the defaults for its various
 * parameters.
 * A module is initialized with a pointer to a valid SignalBank
 * (source modules can be initialized with the NULL pointer). After the
 * Initialize(SignalBank*) function has been called, a call to GetOutputBank()
 * returns a pointer to a SignalBank in which the results
 * of the module's processing will be placed. Modules can use the output_
 * SignalBank to store their output, or leave it uninitialized if they do not
 * produce an output.
 * At each call to Process(input), the module takes the
 * SignalBank 'input' (which must, unless otherwise specified, have the same
 * number of channels, sample rate, buffer size and centre frequencies as the
 * SignalBank which was passed to Initialize()), processes it, and places the
 * output in the internal SignalBank output_.
 * Modules can have an arbitrary number of unique targets. Each
 * completed output frame is 'pushed' to all of the targets of the module
 * in turn when PushOutput() is called. To achieve this, after each complete
 * output SignalBank is filled, the module calls the Process() function of
 * each of its targets in turn.
 * When Initialize() is first called. The module Initialize()s all of its
 * targets with its ouptut_ SignalBank, if it's output bank has been set up.
 */

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \date 2010/01/23
 *  \version \$Id: Module.h 4 2010-02-03 18:44:58Z tcw $
 */

#ifndef _AIMC_SUPPORT_MODULE_H_
#define _AIMC_SUPPORT_MODULE_H_

#include <set>
#include <string>

#include "Support/Common.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
using std::set;
using std::string;
class Module {
 public:
  explicit Module(Parameters *parameters);

  virtual ~Module();

  /* \brief Validate this module's output SignalBank, and initialize
   * any targets of the module if necessary.
   * \param input Input SignalBank.
   * \param output true on success, false on failure.
   */
  bool Initialize(const SignalBank &input);

  /*! \brief
   */
  bool initialized() const;

  /* \brief Add a target to this module. Whenever it generates a new
   * output, this module will push its output to all its targets.
   * \param input Target module to add.
   * \param output true on success, false on failure.
   */
  bool AddTarget(Module* target_module);

  /*! \brief
   */
  bool DeleteTarget(Module* target_module);

  /*! \brief
   */
  void DeleteAllTargets();

  /*! \brief Process a buffer
   */
  virtual void Process(const SignalBank &input) = 0;

  /*! \brief Reset the internal state of the module to that when it was
   * initialised
   */
  virtual void Reset() = 0;

  /*! \brief
   */
  const SignalBank* GetOutputBank() const;

 protected:
  void PushOutput();

  virtual bool InitializeInternal(const SignalBank &input) = 0;

  bool initialized_;
  set<Module*> targets_;
  SignalBank output_;
  Parameters* parameters_;

  string module_identifier_;
  string module_type_;
  string module_description_;
  string module_version_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Module);
};
}

#endif  // _AIMC_SUPPORT_MODULE_H_
