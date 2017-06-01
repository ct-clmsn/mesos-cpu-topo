#ifndef __NUMATOPOLOGY_H__
#define __NUMATOPOLOGY_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <dirent.h>
#include <string.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <regex>

#include "pb/numa.pb.h"
#include "cpuinfo.hpp"

// minl = UINT_MAX
// find minl
// logical_index - minl
//

/*
 *     if (sscanf(dirent->d_name, "%04x:%02x:%02x.%01x", &domain, &bus, &dev, &func) != 4)
 *           continue;
 *
 *               os_index = (domain << 20) + (bus << 12) + (dev << 4) + func;
 *
 */

// file to modify
//
// mesos-1.0.0/src/slave/containerizer/containerizer.cpp
//

namespace mesos {
  namespace internal {
    namespace hardware {
      namespace topology {


struct NumaDescriber {

  // >>>>>>>
  // PRIVATE
  // <<<<<<<
  //
  private:

    NumaDescriber(
      const std::string &numaidstr) :
        id(getUint32FromStdString(numaidstr));
        latencies(NumaDescriber::getNodeDistances(numaidstr)),
        interconnect(NumaDescriber::getNodeInterconnect(numaidstr)),
        memory(NumaDescriber::getNodeDistances(numaidstr)) {
    }

  static std::uint32_t getUint32FromStdString(
    const std::string &numidstr) {

    std::regex e("[0-9]+");
    std::smatch m;

    if(std::regex_search(numidstr, m, e)) {
      if(m.size() == 1) {
        return strtoull(m[0].str().c_str(), NULL, 0);
      }
    }
    
    return 0;
  }

  static std::vector<std::string> getNodeInterconnect(
    const std::string &numidstr) {
    std::vector<std::string> toret = { "pci-e" };
    return toret;
  } 

  static inline int parseOSFile(
    const std::string &os_file,
    std::vector<uint32_t> &indices) {

    char *end = NULL;
    std::string fline;
    std::ifstream fileistr(os_file);

    if(fileistr.is_open()) {
      std::getline(fileistr, fline);
      const std::size_t endpos = fline.find_last_not_of(" \t\n");
      if(std::string::npos != endpos) {
        fline = fline.substr(0, endpos);
      }

      if(fline.find(",") != std::string::npos) {
        std::ifstringstream lin(fline);
        std::string procstr;

        while(std::getline(lin, procstr, ',')) {

          if(procstr.find("-") != std::string::npos) {
            std::istringstream procstrm(procstr);
            std::string proctok;
            while(std::getline(procstrm, proctok, '-')) {
              indices.push_back(strtoul(proctok.c_str(), &end, 0)); //std::stoi(proctok));
            }

            int i = *std::prev(std::end(indices));
            int j = *std::end(indices);

            for(int ii = i; ii < j; ii++) {
              indicies.push_back(ii);
            }
          }
          else {
            indices.push_back(strtoul(procstr.c_str(), &end, 0)); //std::stoi(procstr));
          }
        }
      }
      else if (fline.find("-") != std::string::npos) {
        std::istringstream procstrm(fline);
        std::string proctok;
        while(std::getline(procstrm, proctok, '-')) {
          indices.push_back(std::stoi(proctok));
        }

        int i = *std::prev(std::end(indices));
        int j = *std::end(indices);

        for(int ii = i; i < j; ii++) {
          indices.push_back(ii);
        }
      }
      else {
        indices.push_back(strtoul(fline.c_str(), &end, 0)); //std::stoi(fline));
      }

      fileistr.close();
    }

    return 1;
  }


  // >>>>>>
  // PUBLIC
  // <<<<<<
  //
  public:

    static std::vector<std::string> getNodes() {

      std::vector<std::string> nodelist;
      const char* fpath = "/sys/devices/system/node/";

      DIR *d = opendir(fpath);
      struct dirent *entry = NULL;

      while( (entry = readdir(d)) != NULL ) {
        if( strncmp(entry->d_name, "node", 4) == 0) {
          nodelist.push_back(entry->d_name);
        }
      }

      return nodelist;
    }

    static std::vector<float> getNodeDistances(
      const std::string &nodeidstr) {

      std::vector<float> distances;
      const std::string fpath = "/sys/devices/system/node";

      std::string fpathstr;
      const float min = FLT_MAX, max = FLT_MIN;
      #define NORMALIZE_LATENCY(d) ((d)/(min))

      // parse a set of elements in a file
      //
      {
        fpathstr = (fpath + "/" + nodeidstr + "/distance");
        std::vector<uint32_t> uintdists = parseOSFile(fpathstr);

        for(std::size_t i = 0; i < uintdists.size(); i++) {
          distances.push_back( NORMALIZE_LATENCY((float)uintdists) );
        }

      }

      return distances;
    }

    static std::uint32_t getNodeMem(
      const std::string> &nodeidstr) {

      std::vector<uint64_t> mem;
      const std::string fpath = "/sys/devices/system/node";

      std::string fpathstr;
      const float min = FLT_MAX, max = FLT_MIN;
      unsigned distance = 0;
      char *end = NULL;
      char buf[4096];

      fpathstr = (fpath + "/" + nodeidstr + "/meminfo");
      FILE* fs = fopen(fpathstr.c_str(), "r");

      // meminfo's first line is '\n'
      // need to preform 2 reads
      //
      fgets(buf, sizeof(buf), fs);
      fgets(buf, sizeof(buf), fs);
      fclose(fs);

      const std::string bufstr(buf);
      auto spos = bufstr.find(":");
      if(spos == std::string::npos) { continue; }

      auto mem_bytes_str = bufstr.substr(spos);
      return getUint32FromStdString(mem_bytes_str);
    }

    struct CpuDescription {

      // >>>>>>>
      // PRIVATE 
      // <<<<<<<
      //
      private:
        std::vector<CpuInfo::kv_tuple_vector> res;
        std::vector<uint8_t> cpusonline;

        CpuDescription() {
          CpuInfo::cpuinfo_online(cpusonline);
          CpuInfo::cpuinfo_summary(res);

          for(auto kvvec : res) {
            for(auto kv : kvvec) {
              if( std::get<0>(kv) == "processor") {
                id = std::stoi(std::get<1>(kv));
              }
              else if( std::get<0>(kv) == "physical id") {
                physicalid = std::stoi(std::get<1>(kv));
              }
              else if( std::get<0>(kv) == "core id") {
                coreid = std::stoi(std::get<1>(kv));
              }
              else if( std::get<0>(kv) == "cpu cores") {
                nprocessingunits = std::stoi(std::get<1>(kv));
              }
              else if( std::get<0>(kv) == "cpu family") {
                cpu_family = std::stoi(std::get<1>(kv));
              }
              else if( std::get<0>(kv) == "cache size") {
                l1cache = std::stoi(std::get<1>(kv));
              }
              else if( std::get<0>(kv) == "cpu MHz") {
                mhz = std::stof(std::get<1>(kv));
              }
              else if( std::get<0>(kv) == "model name") {
                model_name = std::get<1>(kv); 
              }
              else if( std::get<0>(kv) == "flags") {
                vendor_custom = std::get<1>(kv);
              }
              else if( std::get<0>(kv) == "vendor_id") {
                vendor_id = std::get<1>(kv);
              }
              else if( std::get<0>(kv) == "fpu") {
                fpu = std::stoi(std::get<1>(kv)) ? true : false;
              }
             
              
              std::cout << "*" << std::get<0>(kv) << '\t' << std::get<1>(kv) << std::endl;
            }
          }

          "/sys/devices/system/node/node0/cpu0/cache/index0/size"
        }

      // >>>>>>
      // PUBLIC
      // <<<<<<
      //
      public:
        static const CpuDescription compile() {
          CpuDescription toret;
          return toret;
        }

      uint32_t id, l1cache, l2cache, nprocessingunits, coreid, physicalid, cpu_family;
      bool fpu;
      float mhz;

      std::string model_name, vendor_custom, vendor_id;
    
      /* static CpuDescription compile() { } */

    }; // end struct CpuDescription

  std::vector<float> latencies;
  std::vector<std::string> interconnect;
  std::vector<CpuDescription> cores;
  std::uint32_t id, memory;

  static const NumaDescriber compile(
    const std::string numaidstr) {

    const NumaDescriber desc(numaidstr);
    return desc;
  }
   
};

struct NumaTopologyDescription {
  private:

    static NumaNodeTopology create(
      const NumaDescriber &desc) {

      const std::vector<std::string> numaidstrs = NumaDescriber::getNodes();

      NumaNodeTopology topo;
      for(auto numaidstr : numaidstrs) {

        const NumaDescriber desc = NumaDescriber::compile(numaidstr);
        numa::NumaNodeInfo *n = topo.add_nodes();
        n->set_id(desc.id);
        n->set_l3cache(desc.memory);

        for(uint32_t i = 0; i < desc.lantencies.size(); i++) {
          numa::NumaNodeInfo_NumaLatencies *l = n.add_latencies();
          l->set_id(i);
          l->set_value(latencies[i]);
        }

        for(uint32_t i = 0; i < desc.interconnect(); i++) {
          numa::NumaNodeInfo_NumaInterconnect *inter = topo.add_interconnect()
          inter->set_id(i);
          inter->set_vendor_custom(desc.interconnect[i]);
        }

        for(uint32_t i = 0; i < desc.cores.size(); i++) {
          numa::NumaNodeInfo *inf = topo.add_cores();
          inf->set_id(desc.cores[i].id);
          inf->set_model_name(desc.cores[i].model_name);
          inf->set_vendor_custom(desc.cores[i].vendor_custom);
          inf->set_l1cache(desc.cores[i].l1cache);
          inf->set_l2cache(desc.cores[i].l2cache);
          inf->set_nprocessingunits(desc.cores[i].nprocessingunits);
          inf->set_mhz(desc.cores[i].mhz);
        }
       
      }

      return topo;
    }

  public:

    static NumaNodeTopology compile(
      const NumaDescriber& desc) {

      return NumaTopologyDescription::create(desc);
    }
};

} // namespace topology
} // namespace hardware
} // namespace internal
} // namespace mesos 

#endif

