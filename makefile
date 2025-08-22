# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++23
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TARGET := $(BIN_DIR)/main

# Default target
all: build

# Build target
build: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile rule for objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: build
	clear
	./$(TARGET) model BEST_VALUE BEST_COEFFICIENT --show

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)