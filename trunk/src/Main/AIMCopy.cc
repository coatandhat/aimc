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

#include "Support/Common.h"
#include "Support/FileList.h"
#include "Support/ModuleTree.h"
#include "Support/Parameters.h"

namespace aimc {
using std::ofstream;
using std::pair;
using std::vector;
using std::string;
class AIMCopy {
 public:
  AIMCopy();
  
  bool Initialize(string script_filename,
                  string config_filename);
  
  bool WriteConfig(string config_dump_filename,
                   string config_graph_filename);
  
  bool Process();
  
 private:
  bool initialized_;
  Parameters global_parameters_;
  ModuleTree tree_;
  vector<pair<string, string> > script_;
};


AIMCopy::AIMCopy() : initialized_(false) {
  
}
  
bool AIMCopy::Initialize(string script_filename,
                         string config_filename) {
  
  LOG_INFO("AIMCopy: Loading script");
  script_ = FileList::Load(script_filename);
  if (script_.size() == 0) {
   LOG_ERROR("No data read from script file %s", script_filename.c_str());
   return false;
  }
    
  LOG_INFO("AIMCopy: Loading configuration");
  if (!tree_.LoadConfigFile(config_filename)) {
    LOG_ERROR(_T("Failed to load configuration file"));
    return false;
  }
  LOG_INFO("AIMCopy: Successfully loaded configuration");
  initialized_ = true;
  return true;
}
  
bool AIMCopy::WriteConfig(string config_dump_filename,
                          string config_graph_filename) {
  if (!initialized_) {
    return false;
  }
  
  if (script_.size() > 0) {
    global_parameters_.SetString("input_filename", script_[0].first.c_str());
    global_parameters_.SetString("output_filename_base", script_[0].second.c_str());
    LOG_INFO("AIMCopy: Initializing tree for initial parameter write.");
    if (!tree_.Initialize(&global_parameters_)) {
      LOG_ERROR(_T("Failed to initialize tree."));
      return false;
    }
    tree_.Reset();
  } else {
    LOG_ERROR(_T("No input files in script."));
    return false;
  }
  
  if (!config_dump_filename.empty()) {
    LOG_INFO("AIMCopy: Dumping configuration.");
    ofstream output_stream;
    output_stream.open(config_dump_filename.c_str());
    if (output_stream.fail()) {
      LOG_ERROR(_T("Failed to open configuration file %s for writing."),
                config_dump_filename.c_str());
      return false;
    }

    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    output_stream << "# AIM-C AIMCopy\n";
    output_stream << "# Run at: " << asctime(timeinfo);
    char * descr = getenv("USER");
    if (descr) {
      output_stream << "# By user: " << descr <<"\n";
    }      
    tree_.PrintConfiguration(output_stream);
    output_stream.close();
  }
    
  if (!config_graph_filename.empty()) {
    ofstream output_stream;
    output_stream.open(config_graph_filename.c_str());
    if (output_stream.fail()) {
      LOG_ERROR(_T("Failed to open graph file %s for writing."),
                config_graph_filename.c_str());
      return false;
    }
    tree_.MakeDotGraph(output_stream);
    output_stream.close();
  }
  return true;
}
  
bool AIMCopy::Process() {
  if (!initialized_) {
    return false;
  }
  bool tree_initialized = false;
  for (unsigned int i = 0; i < script_.size(); ++i) {
    global_parameters_.SetString("input_filename", script_[i].first.c_str());
    global_parameters_.SetString("output_filename_base", script_[i].second.c_str());
    if (!tree_initialized) {
      if (!tree_.Initialize(&global_parameters_)) {
        return false;
      }
      tree_initialized = true;
    } else {
      tree_.Reset();
    }
    aimc::LOG_INFO(_T("%s -> %s"),
                  script_[i].first.c_str(),
                  script_[i].second.c_str());
    tree_.Process();
  }
  // A final call to Reset() is required to close any open files.
  global_parameters_.SetString("input_filename", "");
  global_parameters_.SetString("output_filename_base", "");
  tree_.Reset();
  return true;
}

}  // namespace aimc

int main(int argc, char* argv[]) {
  std::string data_file;
  std::string dot_file;
  std::string config_file;
  std::string script_file;

  const std::string version_string(
    " AIM-C AIMCopy\n"
    "  (c) 2006-2010, Thomas Walters and Willem van Engen\n"
    "  http://www.acoustiscale.org/AIMC/\n"
    "\n");                
    
  const std::string usage_string(
    "AIMCopy is intended as a drop-in replacement for HTK's HCopy\n"
    "command. It is used for making features from audio files for\n"
    "use with HTK.\n"
    "Usage: \n"
    "  <flag>  <meaning>                                 <default>\n"
    "  -A      Print command line arguments              off\n"
    "  -C cf   Set config file to cf                     none\n"
    "  -S f    Set script file to f                      none\n"
    "  -V      Print version information                 off\n"
    "  -D d    Write complete parameter set to file d    none\n"    
    "  -G g    Write graph to file g                     none\n");

  if (argc < 2) {
    std::cout << version_string.c_str();
    std::cout << usage_string.c_str();
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
      continue;
    }
    if (strcmp(argv[i],"-G") == 0) {
      if (++i >= argc) {
        aimc::LOG_ERROR(_T("Graph file name expected after -D"));
        return(-1);
      }
      dot_file = argv[i];
      continue;
    }
   if (strcmp(argv[i],"-V") == 0) {
      std::cout << version_string;
      continue;
    }
    aimc::LOG_ERROR(_T("Unrecognized command-line argument: %s"), argv[i]);
  }

  std::cout << "Configuration file: " << config_file << std::endl;
  std::cout << "Script file: " << script_file << std::endl;
  std::cout << "Data file: " << data_file << std::endl;
  std::cout << "Graph file: " << dot_file << std::endl;
  
  aimc::AIMCopy processor;
  aimc::LOG_INFO("main: Initializing...");
  if (!processor.Initialize(script_file, config_file)) {
    return -1;
  }
  
  aimc::LOG_INFO("main: Writing confg...");
  if (!processor.WriteConfig(data_file, dot_file)) {
    return -1;
  }
  
  aimc::LOG_INFO("main: Processing...");
  if (!processor.Process()) {
    return -1;
  }

  return 0;
}
