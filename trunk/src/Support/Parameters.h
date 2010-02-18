// Copyright 2006-2010, Willem van Engen, Thomas Walters
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
 *  \file
 *  \brief Main parameters store
 *
 *  \author Willem van Engen <cnbh@willem.engen.nl>
 *  \author Thomas Walters <tom@acousticscale.org>
 *  \date created 2006/09/21
 *  \version \$Id: Parameters.h 4 2010-02-03 18:44:58Z tcw $
 */

#ifndef _AIMC_SUPPORT_PARAMETERS_H_
#define _AIMC_SUPPORT_PARAMETERS_H_

#include <string>

#include "Support/SimpleIni.h"

namespace aimc {
/*!
 * \class Parameters "Support/Parameters.h"
 * \brief Main parameter store for parameters
 */
class Parameters {
 public:
  Parameters();
  ~Parameters();

  /*!
   * \brief Load parameter file
   * \param sParamFilename Filename of parameter file to read
   * \return true on succes, false on error
   *
   */
  bool Load(const char *sParamFilename);

  /*! \brief Save Parameter File
   *  \param sParamFilename Filename of parameter file to save
   *  \return true on success, false on error
   */
  bool Save(const char *sParamFilename);

  /*!
   * \brief Load parameter file and merge parameters with current, overwriting
   * duplicates.
   * \param sParamFilename Filename of parameter file to read
   * \return true on succes, false on error
   */
  bool Merge(const char *sParamFilename);

  /*! \brief Get a parameter, setting it with a default value if it is not
   *  already set
   *  \param sName Name of parameter
   *  \param val Value of the parameter
   */
  const char * DefaultString(const char* sName, const char *val);

  /*! \overload
   */
  int DefaultInt(const char* sName, int val);

  /*! \overload
   */
  unsigned int DefaultUInt(const char* sName, unsigned int val);

  /*! \overload
   */
  float DefaultFloat(const char* sName, float val);

  /*! \overload
   */
  bool DefaultBool(const char* sName, bool val);


  /*! \brief Set a parameter
   *  \param sName Name of parameter
   *  \param val Value of parameter
   */
  void SetString(const char *sName, const char *val);

  /*! \overload
   */
  void SetInt(const char *sName, int val);

  /*! \overload
   */
  void SetUInt(const char *sName, unsigned int val);

  /*! \overload
   */
  void SetFloat(const char *sName, float val);

  /*! \overload
   */
  void SetBool(const char *sName, bool val);

  /*! \brief Get the value of a parameter
   *  \param sName Name of parameter
   *  \return Value of parameter
   *
   *  The specified parameter _must_ exist. So put every parameter you may
   *  possibly require into the default parameter file. This method returns
   *  the empty string when a value doesn't exist, but that is only to keep
   *  the application from crashing: don't count on it.
   *  You can use IsSet() however to check for a parameter's existence.
   */
  const char *GetString(const char *sName);

  /*! \overload
   */
  int GetInt(const char *sName);

  /*! \overload
   */
  unsigned int GetUInt(const char *sName);

  /*! \overload
   */
  float GetFloat(const char *sName);

  /*! \overload
   */
  bool GetBool(const char *sName);

  /*! \brief Returns if the parameters exists or not
   *  \param sName Name of parameter
   *  \return true if exists, false if not
   */
  bool IsSet(const char *sName);

  /*! \brief Sets a parameter assignment from a string
   *  \param sCmd String to parse
   *  \return true if parsing succeeded, false if an error occured.
   *
   *  This function parses a string like "foo.bar=50" and sets the parameter
   *  accordingly. Use this function to parse parameter assignments as given
   *  by the user, for example on the command-line or in a parameter file.
   */
  bool Parse(const char *sCmd);

  /*! \brief Delete a parameter. GetSection may not return correctly after
   * this call, so it may not be possible to repopulate the parameter grid
   * after deleting a parameter
   * \param sName Parameter name
   * \return true on success
   */
  bool Delete(const char *sName);

  /*! \brief Append parameters to a string
   * \param sName pointer to a string
   * \return true on success
   */
  std::string WriteString();

  /*! \brief Maximum length of a parameter name in characters
   */
  static const unsigned int MaxParamNameLength = 128;

 protected:
  /*!
   * \brief Load parameter file
   * \param sParamFilename Filename of parameter file to read
   * \return true on succes, false on error
   */
  bool LoadFile(const char *sParamFilename);
  /*!
   * \brief Load parameter file and merge parameters with current,
   *  overwriting duplicates.
   * \param sParamFilename Filename of parameter file to read
   * \return true on succes, false on error
   */
  bool MergeFile(const char *sParamFilename);

  /*! \brief Default ini-section to use
   *
   *  Since SimpleIni is an ini-parser but we don't want the sections,
   *  we use the empty section. This gives use the behaviour desired.
   */
  static const char *m_SDefaultIniSection;

  /*! \brief Parameter file object
   */
  CSimpleIniCase *m_pIni;
  /*! \brief \c preset.include nesting counter to avoid loops
   */
  unsigned int m_iNestCount;
  /*! \brief maximum value m_iNestCount may reach
   */
  static const unsigned int m_iNestCountMaximum = 16;
};
}

#endif  // _AIMC_SUPPORT_PARAMETERS_H_

