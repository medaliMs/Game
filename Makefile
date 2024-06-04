# Makefile

# Compiler
CXX = gcc

# Compiler flags
CXXFLAGS = -std=c11 -Wall -pedantic

# SDL libraries
SDL_LIBS = `pkg-config --libs SDL2` -lSDL2_image -lSDL2_ttf -lxml2

# Source files
SRC = testSdl.c

# Output binary
BIN = testSdl

# Default target
all: $(BIN)

# Build target
$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -g -o $(BIN) $(SDL_LIBS)

# Clean target
clean:
	rm -f $(BIN)
