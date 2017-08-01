#ifndef __CPUINFO__
#define __CPUINFO__ 1

#include <vector>
#include <string>
#include <tuple>

struct CpuInfo {

  typedef std::tuple<std::string, std::string> kv_tuple;
  typedef std::vector<kv_tuple> kv_tuple_vector;

  static bool parse(
    std::string const &cpuinfostrbuf,
    std::vector<kv_tuple_vector> &res);

  static bool cpuinfo_summary(
    std::vector<kv_tuple_vector> &res);

  static bool cpuinfo_online(
    std::vector<uint8_t> &res);

  static bool cpuinfo_details(
    const std::vector<uint8_t> &cpus);

};

#endif


