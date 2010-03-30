// Copyright 2008-2010, Thomas Walters
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
#include <time.h>

#include "Modules/Input/ModuleFileInput.h"
#include "Modules/BMM/ModuleGammatone.h"
#include "Modules/BMM/ModulePZFC.h"
#include "Modules/NAP/ModuleHCL.h"
#include "Modules/Strobes/ModuleParabola.h"
#include "Modules/Strobes/ModuleLocalMax.h"
#include "Modules/SAI/ModuleSAI.h"
#include "Modules/SSI/ModuleSSI.h"
#include "Modules/SNR/ModuleNoise.h"
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
  aimc::ModuleNoise noise_maker(&params);
  aimc::ModuleGammatone bmm(&params);
  aimc::ModuleHCL nap(&params);
  aimc::ModuleLocalMax strobes(&params);
  aimc::ModuleSAI sai(&params);
  aimc::ModuleSSI ssi(&params);

  params.SetBool("slice.all", false);
  params.SetInt("slice.lower_index", 77);
  params.SetInt("slice.upper_index", 150);
  aimc::ModuleSlice slice_1(&params);

  params.SetInt("slice.lower_index", 210);
  params.SetInt("slice.upper_index", 240);
  aimc::ModuleSlice slice_2(&params);

  params.SetInt("slice.lower_index", 280);
  params.SetInt("slice.upper_index", 304);
  aimc::ModuleSlice slice_3(&params);

  params.SetInt("slice.lower_index", 328);
  params.SetInt("slice.upper_index", 352);
  aimc::ModuleSlice slice_4(&params);

  params.SetBool("slice.all", true);
  aimc::ModuleSlice slice_5(&params);

  params.SetFloat("nap.lowpass_cutoff", 100.0);
  aimc::ModuleHCL smooth_nap(&params);
  params.SetBool("slice.all", true);
  aimc::ModuleSlice nap_profile(&params);
  aimc::ModuleScaler nap_scaler(&params);
  aimc::ModuleGaussians nap_features(&params);
  aimc::FileOutputHTK nap_out(&params);

  aimc::ModuleGaussians features_1(&params);
  aimc::ModuleGaussians features_2(&params);
  aimc::ModuleGaussians features_3(&params);
  aimc::ModuleGaussians features_4(&params);
  aimc::ModuleGaussians features_5(&params);

  aimc::FileOutputHTK output_1(&params);
  aimc::FileOutputHTK output_2(&params);
  aimc::FileOutputHTK output_3(&params);
  aimc::FileOutputHTK output_4(&params);
  aimc::FileOutputHTK output_5(&params);

  input.AddTarget(&noise_maker);
  noise_maker.AddTarget(&bmm);
  bmm.AddTarget(&nap);
  bmm.AddTarget(&smooth_nap);
  smooth_nap.AddTarget(&nap_profile);
  nap_profile.AddTarget(&nap_scaler);
  nap_scaler.AddTarget(&nap_features);
  nap_features.AddTarget(&nap_out);
  nap.AddTarget(&strobes);
  strobes.AddTarget(&sai);
  sai.AddTarget(&ssi);

  ssi.AddTarget(&slice_1);
  ssi.AddTarget(&slice_2);
  ssi.AddTarget(&slice_3);
  ssi.AddTarget(&slice_4);
  ssi.AddTarget(&slice_5);

  slice_1.AddTarget(&features_1);
  slice_2.AddTarget(&features_2);
  slice_3.AddTarget(&features_3);
  slice_4.AddTarget(&features_4);
  slice_5.AddTarget(&features_5);

  features_1.AddTarget(&output_1);
  features_2.AddTarget(&output_2);
  features_3.AddTarget(&output_3);
  features_4.AddTarget(&output_4);
  features_5.AddTarget(&output_5);

  if (write_data) {
    ofstream outfile(data_file.c_str());
    if (outfile.fail()) {
      aimc::LOG_ERROR("Couldn't open data file %s for writing",
                      data_file.c_str());
      return -1;
    }
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);


    outfile << "# AIM-C AIMCopy\n";
    outfile << "# Run on: " << asctime(timeinfo);
    char * descr = getenv("USER");
    if (descr) {
      outfile << "# By user: " << descr <<"\n";
    }
    outfile << "#Module chain: ";
    outfile << "#input";
    outfile << "# noise_maker";
    outfile << "#  gt";
    outfile << "#   nap";
    outfile << "#    slice";
    outfile << "#     scaler";
    outfile << "#      features";
    outfile << "#       output";
    outfile << "#    local_max";
    outfile << "#     sai_weighted";
    outfile << "#      ssi";
    outfile << "#       slice";
    outfile << "#        features";
    outfile << "#          output";
    outfile << "#       slice";
    outfile << "#        features";
    outfile << "#          output";
    outfile << "#       slice";
    outfile << "#        features";
    outfile << "#          output";
    outfile << "#       slice";
    outfile << "#        features";
    outfile << "#          output";
    outfile << "#       slice";
    outfile << "#        features";
    outfile << "#          output";
    outfile << "# ";
    outfile << "# Module versions:\n";
    outfile << "# " << input.id() << " : " << input.version() << "\n";
    outfile << "# " << bmm.id() << " : " << bmm.version() << "\n";
    outfile << "# " << nap.id() << " : " << nap.version() << "\n";
    outfile << "# " << strobes.id() << " : " << strobes.version() << "\n";
    outfile << "# " << sai.id() << " : " << sai.version() << "\n";
    outfile << "# " << slice_1.id() << " : " << slice_1.version() << "\n";
    // outfile << "# " << profile.id() << " : " << profile.version() << "\n";
    // outfile << "# " << scaler.id() << " : " << scaler.version() << "\n";
    outfile << "# " << features_1.id() << " : " << features_1.version() << "\n";
    outfile << "# " << output_1.id() << " : " << output_1.version() << "\n";
    outfile << "#\n";
    outfile << "# Parameters:\n";
    outfile << params.WriteString();
    outfile.close();
  }

  for (unsigned int i = 0; i < file_list.size(); ++i) {
    // aimc::LOG_INFO(_T("In:  %s"), file_list[i].first.c_str());
    aimc::LOG_INFO(_T("Out: %s"), file_list[i].second.c_str());

    string filename = file_list[i].second + ".slice_1";
    output_1.OpenFile(filename.c_str(), 10.0f);
    filename = file_list[i].second + ".slice_2";
    output_2.OpenFile(filename.c_str(), 10.0f);
    filename = file_list[i].second + ".slice_3";
    output_3.OpenFile(filename.c_str(), 10.0f);
    filename = file_list[i].second + ".slice_4";
    output_4.OpenFile(filename.c_str(), 10.0f);
    filename = file_list[i].second + ".ssi_profile";
    output_5.OpenFile(filename.c_str(), 10.0f);
    filename = file_list[i].second + ".smooth_nap_profile";
    nap_out.OpenFile(filename.c_str(), 10.0f);

    if (input.LoadFile(file_list[i].first.c_str())) {
      input.Process();
    } else {
      printf("LoadFile failed for file %s\n", file_list[i].first.c_str());
    }
    input.Reset();
  }

  return 0;
}
