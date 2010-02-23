// Copyright 2008-2010, Thomas Walters
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
 * \file AIMCopy.cpp
 * \brief AIM-C replacement for HTK's HCopy
 *
 * The following subset of the command-line flags
 * should be implemented from HCopy:
 *  -A      Print command line arguments         off
 *  -C cf   Set config file to cf                default 
 * (should be able to take multiple config files)
 *  -S f    Set script file to f                 none
 *  //! \todo -T N    Set trace flags to N                 0
 *  -V      Print version information            off
 *  -D of   Write configuration data to of       none
 *
 * \author Thomas Walters <tom@acousticscale.org>
 * \date created 2008/05/08
 * \version \$Id$
 */

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <stdlib.h>

#include "Modules/Input/ModuleFileInput.h"
#include "Modules/BMM/ModuleGammatone.h"
#include "Modules/BMM/ModulePZFC.h"
#include "Modules/NAP/ModuleHCL.h"
#include "Modules/Strobes/ModuleParabola.h"
#include "Modules/SAI/ModuleSAI.h"
#include "Modules/SSI/ModuleSSI.h"
#include "Modules/Profile/ModuleSlice.h"
#include "Modules/Profile/ModuleScaler.h"
#include "Modules/Features/ModuleGaussians.h"
#include "Modules/Output/FileOutputHTK.h"
#include "Support/Common.h"
#include "Support/FileList.h"
#include "Support/Parameters.h"

using std::ofstream;
using std::pair;
using std::vector;
using std::string;
int main(int argc, char* argv[]) {
  string sound_file;
  string data_file;
  string config_file;
  string script_file;
  bool write_data = false;
  bool print_version = false;

  string version_string(
    " AIM-C AIMCopy\n"
    "  (c) 2006-2010, Thomas Walters and Willem van Engen\n"
    "  http://www.acoustiscale.org/AIMC/\n"
    "\n");

  if (argc < 2) {
    printf("%s", version_string.c_str());
    printf("AIMCopy is intended as a drop-in replacement for HTK's HCopy\n");
    printf("command. It is used for making features from audio files for\n");
    printf("use with HTK.\n");
    printf("Usage: \n");
    printf("  -A      Print command line arguments  off\n");
    printf("  -C cf   Set config file to cf         none\n");
    printf("  -S f    Set script file to f          none\n");
    printf("  -V      Print version information     off\n");
    printf("  -D g    Write configuration data to g none\n");
    return -1;
  }

  // Parse command-line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i],"-A") == 0) {
      for (int j = 0; j < argc; j++)
        printf("%s ",argv[j]);
      printf("\n");
      fflush(stdout);
      continue;
    }
    if (strcmp(argv[i],"-C") == 0) {
      if (++i >= argc) {
        aimc::LOG_ERROR(_T("Configuration file name expected after -C"));
        return(-1);
      }
      config_file = argv[i];
      continue;
    }
    if (strcmp(argv[i],"-S") == 0) {
      if (++i >= argc) {
        aimc::LOG_ERROR(_T("Script file name expected after -S"));
        return(-1);
      }
      script_file = argv[i];
      continue;
    }
    if (strcmp(argv[i],"-D") == 0) {
      if (++i >= argc) {
        aimc::LOG_ERROR(_T("Data file name expected after -D"));
        return(-1);
      }
      data_file = argv[i];
      write_data = true;
      continue;
    }
    if (strcmp(argv[i],"-V") == 0) {
      print_version = true;
      continue;
    }
    aimc::LOG_ERROR(_T("Unrecognized command-line argument: %s"), argv[i]);
  }

  if (print_version)
    printf("%s", version_string.c_str());

  aimc::Parameters params;

  if (!params.Load(config_file.c_str())) {
    aimc::LOG_ERROR(_T("Couldn't load parameters from file %s"),
                    config_file.c_str());
    return -1;
  }

  vector<pair<string, string> > file_list = aimc::FileList::Load(script_file);
  if (file_list.size() == 0) {
    aimc::LOG_ERROR("No data read from file %s", script_file.c_str());
    return -1;
  }

  // Set up AIM-C processor here
  aimc::ModuleFileInput input(&params);
  aimc::ModuleGammatone bmm(&params);
  aimc::ModuleHCL nap(&params);
  aimc::ModuleSlice profile(&params);
  aimc::ModuleScaler scaler(&params);
  aimc::ModuleGaussians features(&params);
  aimc::FileOutputHTK output(&params);

  input.AddTarget(&bmm);
  bmm.AddTarget(&nap);
  nap.AddTarget(&profile);
  profile.AddTarget(&scaler);
  scaler.AddTarget(&features);
  features.AddTarget(&output);

  if (write_data) {
    ofstream outfile(data_file.c_str());
    if (outfile.fail()) {
      aimc::LOG_ERROR("Couldn't open data file %s for writing",
                      data_file.c_str());
      return -1;
    }
    outfile << "# AIM-C AIMCopy\n";
    outfile << "# Module versions:\n";
    outfile << "# " << input.id() << " : " << input.version() << "\n";
    outfile << "# " << bmm.id() << " : " << bmm.version() << "\n";
    outfile << "# " << nap.id() << " : " << nap.version() << "\n";
    outfile << "# " << profile.id() << " : " << profile.version() << "\n";
    outfile << "# " << scaler.id() << " : " << scaler.version() << "\n";
    outfile << "# " << features.id() << " : " << features.version() << "\n";
    outfile << "# " << output.id() << " : " << output.version() << "\n";
    outfile << "#\n";
    outfile << "# Parameters:\n";
    outfile << params.WriteString();
    outfile.close();
  }

  for (unsigned int i = 0; i < file_list.size(); ++i) {
    aimc::LOG_INFO(_T("In:  %s"), file_list[i].first.c_str());
    aimc::LOG_INFO(_T("Out: %s"), file_list[i].second.c_str());

    output.OpenFile(file_list[i].second.c_str(), 10.0f);
    if (input.LoadFile(file_list[i].first.c_str())) {
      input.Process();
    } else {
      printf("LoadFile failed for file %s\n", file_list[i].first.c_str());
    }
    input.Reset();
  }

  return 0;
}
