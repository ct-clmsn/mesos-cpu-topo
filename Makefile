CC=g++
CFLAGS=-std=c++11 -O3
all:
	$(CC) $(CFLAGS) -c osfile.cpp
	$(CC) $(CFLAGS) -c cpuinfo.cpp
	$(CC) $(CFLAGS) osfile.o cpuinfo.o unit1.cpp -o unit1

clean:
	rm cpuinfo.o osfile.o unit1
