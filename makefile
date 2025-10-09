SRC := $(shell find src -name "*.cpp")
OBJ := $(patsubst %.cpp,build/%.o,$(notdir $(SRC)))

INCLUDE_DIR := src
BUILD_DIR := build
BIN_DIR := bin

CXX := g++
OPTIM_LEVEL := 0
CPP_STANDARD := 20
CXXFLAGS := -Wall -Wextra -Werror -I$(INCLUDE_DIR) -O$(OPTIM_LEVEL) -std=c++$(CPP_STANDARD) $(EXTRA_FLAGS)
EXTRA_FLAGS := -pthread
TARGET := $(BIN_DIR)/main

INSTALL_PATH := /usr/local/bin

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(EXTRA_FLAGS) -o $@

# Pattern rule: build/foo.o from any foo.cpp
$(BUILD_DIR)/%.o: 
	@mkdir -p $(BUILD_DIR) bin
	$(CXX) $(CXXFLAGS) -c $(filter %/$*.cpp,$(SRC)) -o $@

build: clean $(TARGET)

test: test_clean
	g++ test.cpp -o test

test_clean:
	rm -f test test.o flag.o

bench:
	clear
	@./bin/main --benchmark

install:
	@sudo cp ./bin/main $(INSTALL_PATH)/linsolve

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
