# mesos-cpu-topo
mesos topology support for cpus

modified the containerizer source from the mesos source code distribution.

the modified version of the containerizer in this repository adds a 
preprocessor flag ' __NUMATOPOLOGY_RESOURCE__' which is used to mask or 
unmask the numa topology support from the containerizer at compile time.

