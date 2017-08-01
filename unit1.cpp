#include <iostream>
#include <vector>
#include <tuple>
#include <string>

#include "cpuinfo.hpp"

int main(int argc, char** argv) {

  std::vector<CpuInfo::kv_tuple_vector> res;
  std::vector<uint8_t> cpusonline;

  if(!CpuInfo::cpuinfo_online(cpusonline)) {
    std::cout << "no cpus online" << std::endl;
  }

/*
  if(!CpuInfo::parse("/proc/cpuinfo", res)) {
    std::cout << "failed" << std::endl;
  } 
*/
  if(!CpuInfo::cpuinfo_summary(res)) {
    std::cout << "cpuinfo parse failed" << std::endl;
  }

  for(auto kvvec : res) {
    for(auto kv : kvvec) {
      std::cout << "*" << std::get<0>(kv) << '\t' << std::get<1>(kv) << std::endl;
    }
  }

  return 1;
}
