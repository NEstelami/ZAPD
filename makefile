CC := g++
CFLAGS := -std=c++17

SRC_DIRS := ZAP2

CPP_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
O_FILES := $(foreach file,$(CPP_FILES),$(BUILD_DIR)/$(file:.cpp=.o))

all: ZAP2.out

clean: 
	rm -f ZAP2.out

rebuild: clean all

ZAP2.out:
	$(CC) $(CFLAGS) $(CPP_FILES) -o ZAP2.out -lstdc++fs