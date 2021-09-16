export
OPTIMIZATION_ON ?= 1
ASAN ?= 0
DEPRECATION_ON ?= 1
DEBUG ?= 0
COPYCHECK_ARGS ?= 

CXX := g++
INC := -I ZAPD -I lib/assimp/include -I lib/elfio -I lib/json/include -I lib/stb -I lib/tinygltf -I lib/libgfxd -I lib/tinyxml2 -I ZAPDUtils -I usr/include
CXXFLAGS := -fpic -std=c++17 -Wall -Wextra -fno-omit-frame-pointer
OPTFLAGS :=

ifneq ($(DEBUG),0)
  OPTIMIZATION_ON = 0
  DEPRECATION_OFF = 1
  COPYCHECK_ARGS += --devel
  DEPRECATION_ON = 0
else
  CXXFLAGS += -Werror
endif

ifeq ($(OPTIMIZATION_ON),0)
  OPTFLAGS := -O0
else
  OPTFLAGS := -O2 -march=native -mtune=native
endif

ifneq ($(ASAN),0)
  CXXFLAGS += -fsanitize=address
endif
ifneq ($(DEPRECATION_ON),0)
  CXXFLAGS += -DDEPRECATION_ON
endif
# CXXFLAGS += -DTEXTURE_DEBUG

LDFLAGS := -lstdc++ -lm -ldl -lpng -fuse-ld=lld

UNAME := $(shell uname)
ifneq ($(UNAME), Darwin)
    LDFLAGS += -Wl,-export-dynamic -lstdc++fs
endif

ZAPD_SRC_DIRS := $(shell find ZAPD -type d)
SRC_DIRS = $(ZAPD_SRC_DIRS) lib/tinyxml2

ZAPD_CPP_FILES := $(foreach dir,$(ZAPD_SRC_DIRS),$(wildcard $(dir)/*.cpp))
ZAPD_H_FILES   := $(foreach dir,$(ZAPD_SRC_DIRS),$(wildcard $(dir)/*.h))

CPP_FILES += $(ZAPD_CPP_FILES) lib/tinyxml2/tinyxml2.cpp
O_FILES   := $(foreach f,$(CPP_FILES:.cpp=.o),build/$f)
O_FILES   += build/ZAPD/BuildInfo.o

# create build directories
$(shell mkdir -p $(foreach dir,$(SRC_DIRS),build/$(dir)))


all: ZAPD.out copycheck

build/ZAPD/BuildInfo.o:
	python3 ZAPD/genbuildinfo.py $(COPYCHECK_ARGS)
	$(CXX) $(CXXFLAGS) $(INC) -c build/ZAPD/BuildInfo.cpp -o build/ZAPD/BuildInfo.o

copycheck: ZAPD.out
	python3 copycheck.py

clean:
	rm -rf build ZAPD.out
	$(MAKE) -C lib/libgfxd clean
	$(MAKE) -C ZAPDUtils clean
	$(MAKE) -C ExporterTest clean

rebuild: clean all

format:
	clang-format-11 -i $(ZAPD_CPP_FILES) $(ZAPD_H_FILES)
	$(MAKE) -C ZAPDUtils format
	$(MAKE) -C ExporterTest format

.PHONY: all build/ZAPD/BuildInfo.o copycheck clean rebuild format

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INC) -c $< -o $@

build/ZAPD/Main.o: ZAPD/Main.cpp build/ZAPD/BuildInfo.o
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INC) -c $< -o $@

lib/libgfxd/libgfxd.a:
	$(MAKE) -C lib/libgfxd

.PHONY: ExporterTest
ExporterTest:
	$(MAKE) -C ExporterTest

.PHONY: ZAPDUtils
ZAPDUtils:
	$(MAKE) -C ZAPDUtils

ZAPD.out: $(O_FILES) lib/libgfxd/libgfxd.a ExporterTest ZAPDUtils
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INC) $(O_FILES) lib/libgfxd/libgfxd.a ZAPDUtils/ZAPDUtils.a -Wl,--whole-archive ExporterTest/ExporterTest.a -Wl,--no-whole-archive -o $@ $(FS_INC) $(LDFLAGS)
