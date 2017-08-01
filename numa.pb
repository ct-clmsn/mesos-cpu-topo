syntax = "proto3";
package numa;

message CpuCoreInfo {
  uint32 id = 1;
  repeated uint32 cache_sizes = 2;
  uint32 processingunits = 3;
  string model_name = 4;
  float mhz = 5;
  string vendor_custom = 6;
}

message PciBusDevice {
  string device_str;
}

message NumaNodeInfo {
  message NumaLatency {
    uint32 id = 1;
    float value = 2;
  }

  message NumaInterconnect {
    uint32 id = 1;
    string name = 2;
    map<string, string> device_information = 3;
    string vendor_custom = 4;
  }

  message NumaBus {
    uint32 id = 1;
    string name = 2;
    string vendor_custom = 3;
  }

  uint32 id = 1;
  repeated NumaLatency latencies = 2;
  repeated NumaBus bus = 3;
  repeated NumaInterconnect interconnect = 4;
  repeated PciBusDevice devices = 5;
  repeated CpuCoreInfo cores = 6;
}

message NumaNodeTopology {
  repeated NumaNodeInfo nodes = 1;
}


