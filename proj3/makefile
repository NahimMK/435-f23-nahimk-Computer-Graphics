# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall -I/usr/include/eigen3

# Source file
SRC = renderer.cpp

# Output binary
OUT = renderer

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)