CXXFLAGS ?= -Wall -Wextra -O2 -g -std=c++17
OBJ = Main.o TextureExporter.o RoomExporter.o CollisionExporter.o
LIB = ExporterTest.a


all: $(LIB)

clean:
	rm -f $(OBJ) $(LIB)

format:
	clang-format-11 -i $(CPP_FILES) $(H_FILES)

.PHONY: all clean format

$(OBJ): Main.cpp TextureExporter.cpp RoomExporter.cpp CollisionExporter.cpp

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

%.o: %.cpp
	$(CXX) $(OUTPUT_OPTION) $(CXXFLAGS) -c $< -I ./ -I ../ZAPD -I ../ZAPDUtils -I ../lib/tinyxml2
