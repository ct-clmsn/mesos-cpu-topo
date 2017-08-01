#ifndef __CPUSETASSIGNER_HPP__ 
#define __CPUSETASSIGNER_HPP__ 1

#include "cgroupcpusets.hpp"
#include "TopologyResourceInformation.hpp"

#include <string>
#include <vector>

#include <mesos/resources.hpp>

#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/owned.hpp>
#include <process/process.hpp>

#include <stout/try.hpp>
#include <stout/path.hpp>

class CpusetAssignerProcess : public process::Process<CpusetAssignerProcess> {

public:
  CpusetAssignerProcess() {
  }

  ~CpusetAssignerProcess() {
  }

  process::Future<bool> assign(
    const mesos::ContainerID& containerId,
    const pid_t pid,
    const std::set<int> &cpuset_to_assign,
    const std::set<int> &gpuset_to_assign) {

    std::vector<int> cpumem;
    std::for_each(std::begin(cpuset_to_assign), std::end(cpuset_to_assign),
      [&cpumem, this] (int cpu_os_id) { 
        cpumem.push_back(this->loc.getNumaForCore(cpu_os_id).get());
    });

    const std::string containerIdStr = containerId.value();
    assign_cpuset_group_cpus(containerIdStr, cpuset);
    assign_cpuset_group_mems(containerIdStr, cpumem);
    attach_cpuset_group_pid(containerIdStr, pid);
    
    return true;
  }

private:

  TopologyResourceInformation loc;

};

class CpusetAssigner {

public:

  CpusetAssigner() {
  }

  process::Future<bool> assign(
    const mesos::ContainerID& containerId,
    const pid_t pid,
    const float ncpus_req,
    const float ngpus_req) {
    return dispatch(process,
      &CpusetAssignerProcess::assign,
      containerId,
      pid,
      ncpus_req,
      ngpus_req);
  }

  ~CpusetAssigner() {
    terminate(process);
    wait(process);
  }


private:

  CpusetAssignerProcess process;

};


#endif

