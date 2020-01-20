CC := g++
CFLAGS := -std=c++17 -I ZAP2 -O2

SRC_DIRS := ZAP2 ZAP2/ZRoom ZAP2/ZRoom/Commands ZAP2/Overlays

CPP_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
O_FILES := $(foreach file,$(CPP_FILES),$(BUILD_DIR)/$(file:.cpp=.o))

all: ZAP2.out

clean: 
	rm -f ZAP2.out

rebuild: clean all

/ZAP2/%.cpp:
	@:

ZAP2.out: $(CPP_FILES)
	$(CC) $(CFLAGS) $(CPP_FILES) -o ZAP2.out -lstdc++fs