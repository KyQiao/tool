
FLAG =  -O3 -fPIC -Wall -pedantic -c --std=c++11 -lz 

testFLAG = -O3  -Wall -pedantic  --std=c++11 -lz

# SRC= $(wildcard ./*.cpp)

HEADERS =  $(wildcard ./*.hpp)

vpath %.cpp

.PHONY: all clean test

all: libbase.so

libbase.so: Frame.cpp $(HEADERS)
	g++ $(FLAG) Frame.cpp -I/. -o libbase.so

clean:
	rm libbase.so

test:
	g++ $(testFLAG) testBase.cpp libbase.so -lz -o test

test2:
	g++ -O3 -Wall -pedantic --std=c++11  testTable.cpp -o test2