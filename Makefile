CC = g++
#build directory
BUILD_DIR=build/

INSTALL_PATH = ~/usr
# INSTALL_PATH = /usr/local

# build a single file
BUILDFLAGS = -O3 -fPIC -Wall -pedantic  -fopenmp -c --std=c++11
# compile them into a shared lib
TOTALFALGS = -O3 -shared -fPIC -Wall -pedantic -lz -lvoro++ -fopenmp --std=c++11


# grab all the folders
MODULES=base \
		math\
		structure 

# all source folder
SRC_DIR=$(addprefix src/,$(MODULES))

# put target file into build directory, categoried by module name
BUILD_MODULE_DIR=$(addprefix $(BUILD_DIR),$(MODULES))

# grab all the cpp source file
SRC=$(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))

#grab all the headers
HEADERS = $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.hpp))

# All the .o file
OBJ= $(patsubst src/%.cpp,$(BUILD_DIR)%.o,$(SRC))

# The path of all hpp file
INCLUDES = $(addprefix -I,$(SRC_DIR))
# INCLUDES=-I./include #$(addprefix -I,$(SRC_DIR))

# make all cpp file detectable
vpath %.cpp $(SRC_DIR)

# build single file
define make-build
$1/%.o: %.cpp
	$(CC) $(BUILDFLAGS) $(INCLUDES) $$< -o $$@
endef

# move all the headers to build's include
# the empty line is needed
define install-header

@echo moving $(1) to folder;
@cp $(1) $(BUILD_DIR)include;
endef

$(foreach bdir,$(BUILD_MODULE_DIR),$(eval $(call make-build,$(bdir))))


.PHONY: all checkdirs clean functions copy-headers build


# all: checkdirs $(objects) build $(BUILD_DIR)libcolloid.so copy-headers
all: checkdirs $(objects) build  copy-headers

# $(BUILD_DIR)libcolloid.so: $(HEADERS) $(SRC)
# 	$(CC) $(TOTALFALGS) $(INCLUDES) $^ -o $@

build: $(OBJ)
	$(CC) -O3 -Wall -pedantic --std=c++11 -shared -fPIC -Wl,-soname,libcolloid.so -lz -lvoro++ -fopenmp $^ -o $(BUILD_DIR)libcolloid.so
	# -llapack -lblas  -lfftw3

checkdirs: $(BUILD_DIR) $(BUILD_MODULE_DIR)

$(BUILD_DIR):
	@mkdir -p $@
	@mkdir -p $(BUILD_DIR)include

$(BUILD_MODULE_DIR):
	@mkdir -p $@

copy-headers:
	$(foreach h,$(HEADERS),$(call install-header, $(h)))

clean:
	@rm -rf $(BUILD_DIR)
	@cd ./functions && make clean && cd ..;

functions:
	@cd ./functions && make && cd ..;


install:
	@mkdir -p $(INSTALL_PATH)/include/colloid;
	@cp -f $(BUILD_DIR)/include/* $(INSTALL_PATH)/include/colloid;
	@cp -f $(BUILD_DIR)libcolloid.so $(INSTALL_PATH)/lib
