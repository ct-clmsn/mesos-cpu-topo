#include "cpuinfo.hpp"
#include "osfile.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <iostream>

/* 
   /sys/devices/system/cpu/cpu%d/topology/
   /sys/devices/system/cpu/cpu0/node0/distance
*/

bool CpuInfo::parse(
  std::string const &cpuinf_path,
  std::vector<CpuInfo::kv_tuple_vector> &res) {
  
  std::stringstream cpuinfstrm;
  if(!read_systemfile(cpuinf_path, cpuinfstrm)) {
    return false;
  }
   
  std::string kvstr, kval, vval; 
  CpuInfo::kv_tuple_vector tupvec;

  while(std::getline(cpuinfstrm, kvstr)) {
    if(kvstr == "") {
      res.push_back(tupvec);
      tupvec.clear();
      tupvec.shrink_to_fit();
    }

    std::stringstream kvstrm(kvstr);
    std::getline(kvstrm, kval, ':');
    std::getline(kvstrm, vval, ':');
    boost::trim(kval); boost::trim(vval);

    // loop terminal case
    // b/c of how the file
    // is read, 1024 blocks
    // this test is needed
    // to find the end of
    // the file
    //
    //if(vval.size() == 0 && kval.size() == 0) {
    //  break;
    //}

    tupvec.push_back(std::make_tuple(kval, vval));
  } 

  return true;
}

bool CpuInfo::cpuinfo_summary(
    std::vector<CpuInfo::kv_tuple_vector> &res) {
    return CpuInfo::parse("/proc/cpuinfo", res);
}

bool CpuInfo::cpuinfo_online(
  std::vector<uint8_t> &res) {

  const std::string onlinefpath("/sys/devices/system/cpu/online");

  const int count = parse_os_index_file(onlinefpath, res);
  if(count < 1) {
    return false;
  }
  
  return true;
}

bool cpuinfo_details(
  const std::vector<uint8_t> &cpus) {
}

