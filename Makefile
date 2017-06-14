CC=g++
CFLAGS=-std=c++11 -O3
PROTOC=protoc

all:
	$(PROTOC) --cpp_out=include numa.pb
	mv include/numa.pb.pb.h include/numa.pb.h
	mv include/numa.pb.pb.cc include/numa.pb.cc
	$(CC) $(CFLAGS) -c osfile.cpp
	$(CC) $(CFLAGS) -c cpuinfo.cpp
	$(CC) $(CFLAGS) osfile.o cpuinfo.o unit1.cpp -o unit1

clean:
	rm cpuinfo.o osfile.o unit1
