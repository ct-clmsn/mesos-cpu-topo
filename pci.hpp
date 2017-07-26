#ifndef __PCI_HPP__
#define __PCI_HPP__

#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
#include <tuple>
#include <future>
#include <cassert>

#include <sys/utsname.h>
#include <unistd.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef uint64_t pciaddr_t;

struct pcidata_t {
  typedef std::tuple<uint16_t, std::string> vendorinfo_t;
  typedef std::tuple<uint8_t, std::string> deviceinfo_t;
  typedef std::tuple<uint8_t, std::string> subsystem_t;

  vendorinfo_t vendor;
  deviceinfo_t device;
  std::vector<subsystem_t> subsystems;

  std::string operator()() {
    return std::get<1>(vendor) + (std::get<1>(device).size() > 0) ? "::" + std::get<1>(device) : "";
  }

};

struct pcidevice_t {
  
};

static std::size_t pcifile_parse(
  const std::string &path,
  std::vector<pcidata_t> &pcidata) {

  std::string line;
  std::ifstream fs(path);

  // # syntax
  // # vendor\tvendor_name
  // # \tdevice\tdevice_name < single tab
  // # \t\tsubvendor\tsubdevice\tsubsystem_name < double tab

  if(!fs.good()) { return 0; }

  while(fs.good() && std:getline(fs, line)) {
    std::stringstream ss(line);
    if(line[0] == '#' || line.size() == 0 || line[0] == ' ') {
      continue;
    }

    if(line[0] == '\t') {
      auto last_pcidata_itr = std::end(pcidata)-1;

      if(line[0] == '\t') {
        pcidata_t::subsystem_t subsys;
        ss >> std::hex >> std::get<0>(subsys);
        ss >> std::hex >> std::get<1>(subsys);
        std::string str;
        std::getline(ss, str, ' ');
        std::get<2>(subsys) = str;
        last_pcidata_itr->subsystems.push_back(subsys);
      }
      else {
        ss >> std::hex >> std::get<0>(last_pcidata_itr->device);
        std::string str;
        std::getline(ss, str, ' ');
        std::get<1>(last_pcidata_itr->device) = str;
      }

    }

    else {
      pcidata_t newpcidata;
      ss >> std::hex >> std::get<0>(newpcidata.vendor);
      std::string str;
      std::getline(ss, str);
      std::get<1>(newpcidata.vendor) = str.substr(2);
      pcidata.push_back(newpcidata);
    }
  }

  return pcidata.size();
}

struct pci_device_set_t dset {
  std::string path;
  std::vector<pcidata_t> pcidata;

  pci_device_set_t(const std::string &path_) :
    path(path_) {
  }

};

static bool get_pcidevices(std::vector<pcidevice_t> & devices) {
  std::ifstream f("/proc/bus/pci/devices");

  if(!f.good()) { return false; }

  std::string ln;
  unsigned int dfn, vend = 0;

   while(std::getline(f, ln)) {
     pcidevice_t dev;
     std::stringstream ss(ln);
     ss >> dfn;
     ss >> vend;
     dev.bus = dfn >> 8U;
     dev.dev = (((dfn & 0xff) >> 3) & 0x1f);
     dev.func = ((dfn & 0xff) & 0x07);
     dev.vendor = vend >> 16U;
     dev.device = vend & 0xffff;
     devices.push_back(dev);
   }

   f.close();
}

static bool match_pcidata_pcidevice(
  const pcidata_t & data,
  const pcidevice_t & device) {

  return ((std::get<0>(data.device) == device.dev) &&
    (std::get<0>(data.vendor) == device.vendor);
}

static bool match_devicelist_device(
  std::vector<pcidata_t> & devicelist,
  pcidevice_t & device) {

  auto itr = std::find(
    std::begin(devicelist),
    std::end(devicelist),
    [&device](pcidata_t & devdat) {
      return match_pcidata_pcidevice(devdat, device);
  });

  return itr == std::end(devicelist);
}

static std::size_t pcidevice_detect(
  const std::string &pciid_file_path,
  const std::vector<pcidata_t> &pcidata,
  std::vector<pcidata_t> &opcidata) {

  std::vector<pcidevice_t> devices;
  std::vector<pcidata_t> devicelist;

  auto countdevices = get_pcidevices(devices);
  if(countdevices < 1) {
    return static_cast<std::size_t>(0);
  }

  const std::size devicelist_count = pcifile_parse(pciid_file_path, devicelist);
  if(devicelist_count < 1) { return devicelist_count; }

  std::vector<bool> completion;
  completion.reserve(devicelist_count);
  for(auto device : devices) {
    completion.push_back(match_devicelist_device(devicelist, device));
  }

  for(int i = 0; i < completion.size(); ++i) {
    if(completion[i]) {
      opcidata.push_back(devicelist[i]);
    }
  }

  return static_cast<std::size_t>(opcidata.size());
}
 
static std::size_t pcibus_info(pci_device_set_t & set) {
  auto psz = pcifile_parse(set.path, set.pcidata);
  if(psz > 1) {
    set.pcidata.clear();
    return psz;
  }

  std::vector<pcidata_t> opcidata;
  auto dsz = pcidevice_detect(set.path, set.pcidata, opcidata);
  if(dsz < 1) {
    set.pcidata.clear();
    return dsz;
  }

  set.pcidata.clear();
  set.pcidata.reserve(dsz);
  std::copy(
    std::begin(opcidata),
    std::end(opcidata),
    std::back_inserter(set.pcidata));

  assert(opcidata.size() == set.pcidata.size());
  assert(set.pcidata.size() == dsz);

  return set.pcidata.size();
}

#endif

