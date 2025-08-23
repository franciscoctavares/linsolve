SRC := $(shell find src -name "*.cpp")
OBJ := $(patsubst %.cpp,build/%.o,$(notdir $(SRC)))

BIN_DIR := bin

CXX := g++
CXXFLAGS := -Wall
TARGET := $(BIN_DIR)/main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@

# Pattern rule: build/foo.o from any foo.cpp
build/%.o: 
	@mkdir -p build bin
	$(CXX) $(CXXFLAGS) -c $(filter %/$*.cpp,$(SRC)) -o $@

build: clean $(TARGET)

clean:
	rm -rf build bin