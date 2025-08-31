SRC := $(shell find src -name "*.cpp")
OBJ := $(patsubst %.cpp,build/%.o,$(notdir $(SRC)))

INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR := bin

CXX := g++
OPTIM_LEVEL := 0
CXXFLAGS := -Wall -Wextra -I$(INCLUDE_DIR) -O$(OPTIM_LEVEL)
TARGET := $(BIN_DIR)/main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@

# Pattern rule: build/foo.o from any foo.cpp
$(BUILD_DIR)/%.o: 
	@mkdir -p $(BUILD_DIR) bin
	$(CXX) $(CXXFLAGS) -c $(filter %/$*.cpp,$(SRC)) -o $@

build: clean $(TARGET)

test:
	clear
	@./bin/main bench BEST_VALUE BEST_COEFFICIENT --show

bench:
	clear
	@./bin/main --benchmark

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
