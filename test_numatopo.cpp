#include "include/numa.pb.pb.h"
#include "numatopology.hpp"

using namespace mesos::internal::hardware::topology;

int main(int argc, char **argv) {

  const NumaDescriber::NumaDescription numadesc = NumaDescriber::create();

  //auto topo = NumaTopologyDescription::create();
}
