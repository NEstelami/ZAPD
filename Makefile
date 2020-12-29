CC := g++

ifneq (, $(shell which ccache))
CC := ccache $(CC)
endif

CFLAGS := -g -std=c++17 -I ZAP2 -I ZAP2/sqlite -O2 -rdynamic

SRC_DIRS := ZAP2 ZAP2/ZRoom ZAP2/ZRoom/Commands ZAP2/Overlays ZAP2/HighLevel ZAP2/OpenFBX

CPP_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
O_FILES   := $(CPP_FILES:.cpp=.o)

all: ZAP2.out

clean: 
	rm -f $(O_FILES) ZAP2.out

rebuild: clean all

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

ZAP2.out: $(O_FILES)
	$(CC) $(CFLAGS) $(O_FILES) -o $@ -lstdc++fs
	cp ZAP2.out /mnt/c/projects/oot/tools/ZAP2/ZAP2.out
