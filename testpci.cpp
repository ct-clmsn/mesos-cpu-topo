#include "pci.hpp"
#include <string>
#include <vector>
#include <iostream>

int main() {
  std::string path = "/usr/share/misc/pci.ids";

  pci_device_set_t dset(path);

  auto sz = pcibus_info(dset);
  std::cout << "count\t" << sz << std::endl;

  for(auto pcidata : dset.pcidata) {
    std::cout << pcidata() << std::endl;
  }

}
