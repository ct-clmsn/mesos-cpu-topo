package numa;

message CpuCoreInfo {
  required uint32 id = 1;
  required uint32 l1cache = 2;
  required uint32 l2cache = 3;
  required uint32 processingunits = 4;
  optional string model_name = 5;
  optional float mhz = 6;
  optional string vendor_custom = 7;
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


