CC = g++
CFLAGS = -std=c++11 -I/usr/include/eigen3/
LDFLAGS =
SOURCES = smoothing.cpp io.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = smoothing

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)