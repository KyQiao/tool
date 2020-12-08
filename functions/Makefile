CC = g++

# build a single file
BUILDFLAGS = -O3 -fPIC -Wall -pedantic -c --std=c++11
# compile them into a shared lib
TOTALFALGS = -O3 -shared -fPIC -Wall -pedantic -lz -lvoro++ --std=c++11

#build directory
LibPath = ~/usr/
installPath = ~/usr/bin/


# grab all the cpp source file
SRC=$(wildcard *.cpp)

# The path of all hpp file
INCLUDES = $(LibPath)include/colloid/

#grab all the headers
HEADERS = $(wildcard $(INCLUDES)/*.hpp)

# All the .o file
OBJ= $(patsubst %.cpp,$(installPath)%,$(SRC))


.PHONY: all checkdirs clean 

all: checkdirs $(OBJ) build 

# @echo $(SRC)
# @echo $(installPath)
# @echo $(OBJ)
build: $(OBJ)
	@echo compile finished

$(installPath)%: %.cpp
	$(CC) -O3  -Wall -pedantic --std=c++11 -I $(INCLUDES)  -lcolloid -lz -fopenmp -lvoro++ -lm $< -o $@
	@echo $@ compile finished
# $(CC) -O3 -Wall -pedantic --std=c++11 -I$(INCLUDES) -L$(LibPath) -lcolloid -lz -lvoro++ $< -o $@

checkdirs: $(installPath)

$(installPath):
	@mkdir -p $@

clean:
	@rm -rf $(OBJ)
