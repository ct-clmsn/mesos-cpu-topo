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

struct utils {

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

};

struct NumaDescriber {

  public:

  static std::vector<std::string> getNodeInterconnect(
    const std::string &numidstr) {
    std::vector<std::string> toret = { "pci-e" };
    return toret;
  }

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
      std::vector<uint32_t> uintdists;
      utils::parseOSFile(fpathstr, uintdists);

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

  struct NumaDescription {

    struct CpuDescription {

      private:

      std::vector<CpuInfo::kv_tuple_vector> res;
      std::vector<uint8_t> cpusonline;

      CpuDescription(const std::vector<CpuInfo::kv_tuple_vector>& res) {
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
          }
        }

      }

      public:
  
      static std::vector<CpuDescription> create() {
        CpuInfo::cpuinfo_online(cpusonline);
        CpuInfo::cpuinfo_summary(res);

        CpuDescription toret(res);
        return toret;
      }

      uint32_t id, nprocessingunits, coreid, physicalid, cpu_family;
      std::vector<uint32_t> caches;
      bool fpu;
      float mhz;

      std::string model_name, vendor_custom, vendor_id;
    
    }; // end struct CpuDescription

    struct NumaNodeInfo {

      std::vector<float> latencies;
      std::vector<std::string> interconnect;
      std::vector<CpuDescription> cores;
      std::uint32_t id;

      NumaNodeInfo(const std::string &numaidstr) :
        id(utils::getUint32FromStdString(numaidstr));
        latencies(NumaDescriber::getNodeDistances(numaidstr)),
        interconnect(NumaDescriber::getNodeInterconnect(numaidstr)) {
      }

      // returns cache sizes in the order provided by:
      // /sys/devices/system/node/nodeM/cpuN/cache/indexO/size
      //
      static bool getCacheSizes(
        const NumaDescription &numanode,
        CpuDescription &cpu) {

        const std::string coreid(cpu.coreid);
        const std::string dirpath =
          "/sys/devices/system/node/node" + numanode.id + "/cpu" + coreid + "/cache"; ///index0/size

        DIR *d = opendir(dirpath.c_str());

        if(errno == ENOENT) {
          return false;
        }

        struct dirent *entry = NULL;

        while( (entry = readdir(d)) != NULL ) {
          if( strncmp(entry->d_name, "index", 5) == 0) {
            const std::string cache_level(entry->d_name);
            const std::string fpath = dirpath + "/" + cache_level + "/size";

            utils::parseOSFile<uint32_t>(fpath, cpu.caches);
          }
        }

        return true;
      }

      public:

      static const NumaNodeInfo create(const std::string &numaidstr) {
        std::vector<CpuDescription> cpudescs = CpuDescription::create();

        NumaNodeInfo ndesc(numaidstr);
        for(auto cpudesc : cpudescs) {
          if(NumaNodeInfo::getCacheSizes(ndesc, cpudesc, cache_sizes)) {
            ndesc.push_back(cpudesc);
          }
        }

        return ndesc;
      }

    };

    private:

    NumaDescription(const size_t nodecount) {
      nodes.reserve(nodecount);
    }

    public:

    const static NumaDescription create() {

      const std::vector<std::string> numaidstrs = 
        NumaDescription::getNodes();

      NumaDescription desc(numaidstrs.size());

      for(auto numaidstr : numaidstrs) {
        NumaNodeInfo node = NumaNodeInfo::create(numaidstr);
        desc.nodes.push_back(node);
      }

      return desc;
    }

    std::vector<NumaNodeInfo> nodes;

  };
   
  static NumaDescription create() {
    NumaDescription desc = NumaDescription::create();
    return desc; 
  }

};

struct NumaTopologyDescription {
  private:

    static NumaNodeInfoTopology create() {

      NumaNodeInfoTopology topo;

      const NumaDescription numadesc = NumaDescriber::create();

      for(auto numanode : numadesc.nodes) {

        numa::NumaNodeInfoInfo *n = topo.add_nodes();
        n->set_id(numanode.id);

        for(uint32_t i = 0; i < numanode.lantencies.size(); ++i) {
          numa::NumaNodeInfoInfo_NumaLatencies *l = n.add_latencies();
          l->set_id(i);
          l->set_value(numanode.latencies[i]);
        }

        for(uint32_t i = 0; i < numanode.interconnect(); ++i) {
          numa::NumaNodeInfoInfo_NumaInterconnect *inter = topo.add_interconnect()
          inter->set_id(i);
          inter->set_vendor_custom(numanode.interconnect[i]);
        }

        for(uint32_t i = 0; i < numanode.cores.size(); ++i) {
          numa::NumaNodeInfoInfo *inf = topo.add_cores();
          inf->set_id(numanode.cores[i].id);
          inf->set_model_name(numanode.cores[i].model_name);
          inf->set_vendor_custom(numanode.cores[i].vendor_custom);
          inf->set_nprocessingunits(numanode.cores[i].nprocessingunits);
          inf->set_mhz(numanode.cores[i].mhz);

          // TODO this is wonky fix.
          for(auto j = 0; j < nuamnode.cores[i].caches.size(); ++j) {
            inf->set_caches(numanode.cores[i].caches[j]);
          }

      }

      return topo;
    }

  public:

    static NumaTopology create(
      const NumaDescriber& desc) {
      return NumaTopologyDescription::create(desc);
    }
};

} // namespace topology
} // namespace hardware
} // namespace internal
} // namespace mesos 

#endif

