CXX=g++
CXXFLAGS = -Iinclude -std=c++17 # -fsanitize=address -O1 -fno-omit-frame-pointer -g #-Wall 

SRC_DIR = src
OBJ_DIR = obj

SOURCES = compile.cpp $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXECUTABLE = compile

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) compile

.PHONY: all clean
