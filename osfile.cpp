// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ct-clmsn
//
#include "osfile.hpp"

#include <list>
#include <algorithm>
#include <fstream>

#include <unistd.h>

bool read_systemfile(
  std::string const &path,        
  std::stringstream &filestrbuf) {

  std::ifstream ifs(path); //, std::ios::in);
  if(!ifs.is_open()) {
    return false;
  }

  char filecstrbuf[1024];

  while(ifs.good()) {
    ifs.read(filecstrbuf, 1024);
    filestrbuf << filecstrbuf;
  }

  return true;
}

int parse_os_index_file(
  std::string const &os_idx_file_path, 
  std::vector<uint8_t> &indices ) 
{
  std::string fline;
  std::ifstream fileistr(os_idx_file_path);

  // parsing code for the cpuset.cpus file...
  // assumes structure found in testfile-testfile4
  //
  if(fileistr.is_open()) {
    std::getline(fileistr, fline);
    const size_t endpos = fline.find_last_not_of(" \t\n");
    if( std::string::npos != endpos ) {
      fline = fline.substr( 0, endpos+1 );
    }

    if(fline.find(",") != std::string::npos) {
      std::istringstream lin(fline);
      std::string procstr;

      while(std::getline(lin, procstr, ',')) {

        if(procstr.find("-") != std::string::npos) {

          std::istringstream procstrm(procstr);
          std::string proctok;
          while(std::getline(procstrm, proctok, '-')) {
            indices.push_back(std::stoi(proctok));
          }

          int i = *std::prev(std::end(indices));
          int j = *std::end(indices);
          for(int ii = i; ii < j; ii++) {
            indices.push_back(ii);
          }
        }
        else {
          indices.push_back(std::stoi(procstr));
        }
      }
    }
    else if(fline.find("-") != std::string::npos) {
      std::istringstream procstrm(fline);
      std::string proctok;
      while(std::getline(procstrm, proctok, '-')) {
        indices.push_back(std::stoi(proctok));
      }

      int i = *std::prev(std::end(indices));
      int j = *std::end(indices);
      for(int ii = i; ii < j; ii++) {
        indices.push_back(ii);
      }
    }
    else {
      indices.push_back(std::stoi(fline));
    }

    fileistr.close();
  }

  return indices.size();
}

