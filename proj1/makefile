# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall -I/usr/include/eigen3

# Source file
SRC = raytrace.cpp

# Output binary
OUT = raytrace

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)