# mesos-cpu-topo
mesos topology support for cpus

modified the containerizer source from the mesos source code distribution.

the modified version of the containerizer in this repository adds a 
preprocessor flag ' __NUMATOPOLOGY_RESOURCE__' which is used to mask or 
unmask the numa topology support from the containerizer at compile time.

includes demo system including a mesos framework that provides a scheduler
and a cpuisolator. the scheduler selects cpunodes and schedules a 
container's execution plan dynamically. the isolator accepts the task 
description and the execution plan and runs the container according to 
those constraints.

