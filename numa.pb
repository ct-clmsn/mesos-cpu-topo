package numa;

message CpuCoreInfo {
  required uint32 id = 1;
  required repeated uint32 cache_sizes = 2;
  required uint32 processingunits = 3;
  optional string model_name = 4;
  optional float mhz = 5;
  optional repeated string vendor_custom = 6;
}

message NumaNodeInfo {
  message NumaLatencies {
    repeated uint32 id = 1;
    repeated float value = 2;
  }

  message NumaInterconnect {
    required uint32 id = 1;
    optional string vendor_custom = 2;
  }

  required uint32 id = 1;
  repeated NumaLatencies latencies = 2;
  repeated NumaInterconnect interconnect = 3;
  required uint32 l3cache = 4; // memory?
  repeated CpuCoreInfo cores = 5;
}

message NumaNodeTopology {
  repeated NumaNodeInfo nodes = 1;
}


