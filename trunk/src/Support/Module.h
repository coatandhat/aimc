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

/*! \author: Thomas Walters <tom@acousticscale.org>
 *  \date 2010/01/23
 *  \version \$Id$
 */

#ifndef AIMC_SUPPORT_MODULE_H_
#define AIMC_SUPPORT_MODULE_H_

#include <set>
#include <string>

#include "Support/Common.h"
#include "Support/Parameters.h"
#include "Support/SignalBank.h"

namespace aimc {
using std::set;
using std::string;

/*! \brief Base class for all AIM-C modules.
 *
 * Module() is a base class, from which all AIM-C modules are derived. 
 * Classes deriving from module need to implement, at minimum, the pure
 * virtual public function Module::Process() and the pure virtual private 
 * functions Module::InitializeInternal() and Module::ResetInternal().
 * (Note: this is in contravention of 
 * <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml?showone=Inheritance#Inheritance">
 * this rule on inheritance</a> in the Google style guide, but it is designed 
 * to make the implementation of modules as simple as possible.)
 * 
 * The module constructor is called with a pointer to a set of Parameters.
 * In the constructor, the module sets the defaults for its various
 * parameters.
 * A module is initialized with a reference to a valid SignalBank. After the
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
 * targets with its ouptut_ SignalBank, if its output bank has been set up.
 *
 */
class Module {
 public:
  explicit Module(Parameters *parameters);

  virtual ~Module();

  /* \brief Initialize the module, including calling InitializeInternal().
   * Validate this module's output SignalBank, and initialize
   * any targets of the module if necessary.
   * \param input Input SignalBank.
   * \return true on success, false on failure.
   *
   * A call to Initialize() will first validate the input SignalBank passed to
   * it. If this SignalBank is valid, then it will call the
   * InitializeInternal() function of the child class; this will set up the 
   * child class, and may, if the module produces an output, initialize the
   * member variable SignalBank output_. If output_ is initialized after the 
   * call to InitializeInternal(), the module will Initialize its targets with
   * the output. In this way, it is possible to initialize an entire module 
   * tree with a single call to the Initialize() function of the root.
   *  
   * This function is declared virtual in order to deal with the edge case of
   * input modules which do not take a SignalBank as input, but rather
   * generate their own input. In this case, it is better to be able to
   * override the default Initialize function. When creating a new module, do
   * not create a new version of Initialize uness you're sure you know what
   * you're doing!
   */
  virtual bool Initialize(const SignalBank &input);

  /*! \brief Returns true if the module has been correctly initialized
   *  \return true if module has been initialized, false otherwise
   */
  bool initialized() const;

  /* \brief Add a target to this module. Whenever it generates a new
   * output, this module will push its output to all its targets.
   * \param target_module Pointer to a target Module to add.
   * \return true on success, false on failure.
   *
   * When a pointer is passed as a target to the module, the caller retains
   * ownership of the module that it points to. The pointed-to module must
   * continue to exist until it is deleted from the target list by a call
   * to DeleteTarget() or DeleteAllTargets(), or the current module is
   * destroyed. Bad things will happen if the Module pointed to is deleted
   * and Initialize(), Reset() or Process() is subsequently called.
   */
  bool AddTarget(Module* target_module);

  /*! \brief Remove a previously added target module from the list of targets 
   *  for this module. 
   *  \param target_module Pointer to the module to remove. This must be a
   *  pointer that was previously passed to AddTarget() 
   *  \return true on success, false on failure. 
   */
  bool RemoveTarget(Module* target_module);

  /*! \brief Remove all previously added target modules from the list of
   *  targets for this module.
   */
  void RemoveAllTargets();

  /*! \brief Process a buffer.
   * \param input SignalBank of the form which was passed to Initialize()
   *
   * Process is called once for each input SignalBank. When implemented in
   * classes inheriting from aimc::Module, P
   * this SignalBank contains the (read-only) input to the module. It is
   * expected that the input SignalBank will have the same number of channels, 
   * buffer length and sample rate as the SignalBank passed to Initialize. 
   */
  virtual void Process(const SignalBank &input) = 0;

  /*! \brief Reset the internal state of this module and all its children to
   *  their initial state.
   *
   * Like a call to Initialize() will cause all target modules to be
   * initialized, a call to Reset() will cause all target modules to be reset. 
   */
  void Reset();

  /*! \brief Return a pointer to the output SignalBank_ for this class.
   *  \return pointer to the SignalBank that this module uses to store its
   * output.
   */
  const SignalBank* GetOutputBank() const;

  string version() const {
    return module_version_;
  }

  string id() const {
    return module_identifier_;
  }

  string description() const {
    return module_description_;
  }

  string type() const {
    return module_type_;
  }

 protected:
  void PushOutput();

  virtual void ResetInternal() = 0;

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

#endif  // AIMC_SUPPORT_MODULE_H_
