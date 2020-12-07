

.PHONY: all clean

all: spherical_harmonics.cpp spherical_harmonics.hpp wigner3j.hpp wigner3j.cpp
	g++ -O3 -Wall -shared -Wno-unused-value --std=c++11 -fPIC  -o libdmath.so spherical_harmonics.cpp spherical_harmonics.hpp wigner3j.hpp wigner3j.cpp

clean:
	rm libdmath.so
