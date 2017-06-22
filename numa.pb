syntax = "proto3";
package numa;

message CpuCoreInfo {
  uint32 id = 1;
  repeated uint32 cache_sizes = 2;
  uint32 processingunits = 3;
  string model_name = 4;
  float mhz = 5;
  repeated string vendor_custom = 6;
}

message NumaNodeInfo {
  message NumaLatencies {
    repeated uint32 id = 1;
    repeated float value = 2;
  }

  message NumaInterconnect {
    uint32 id = 1;
    string name = 2;
    string vendor_custom = 3;
  }

  message NumaBus {
    uint32 id = 1;
    string name = 2;
    string vendor_custom = 3;
  }

  uint32 id = 1;
  repeated NumaLatencies latencies = 2;
  repeated NumaInterconnect interconnect = 3;
  repeated CpuCoreInfo cores = 4;
}

message NumaNodeTopology {
  repeated NumaNodeInfo nodes = 1;
}


