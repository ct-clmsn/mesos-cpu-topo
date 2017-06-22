CC=g++
CFLAGS=-std=c++11 -O3 -g 
PROTOC=protoc
PBCFLAGS=$(shell pkg-config --cflags protobuf)
PBLDFLAGS=$(shell pkg-config --libs protobuf)

all: numa.pb.pb.o osfile.o cpuinfo.o
	$(CC) $(CFLAGS) $(PBCFLAGS) numa.pb.pb.o osfile.o cpuinfo.o test_numatopo.cpp -o test_numatopo $(PBLDFLAGS)

numa.pb.pb.o: numa.pb
	mkdir include
	$(PROTOC) --cpp_out=include numa.pb
	$(CC) $(CFLAGS) $(PBCFLAGS) -c include/numa.pb.pb.cc

osfile.o: osfile.cpp osfile.hpp
	$(CC) $(CFLAGS) -c osfile.cpp

cpuinfo.o: cpuinfo.cpp cpuinfo.hpp
	$(CC) $(CFLAGS) -c cpuinfo.cpp

unit1:
	$(CC) $(CFLAGS) osfile.o cpuinfo.o unit1.cpp -o unit1

clean:
	rm -rf numa.pb.pb.o cpuinfo.o osfile.o test_numatopo ./include
#unit1
