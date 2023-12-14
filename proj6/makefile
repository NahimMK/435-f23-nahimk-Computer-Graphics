CXXFLAGS = -g -O3 -I /usr/include/eigen3/
CXX = g++

carve: seamcarve.cpp
	$(CXX) $(CXXFLAGS) seamcarve.cpp -o carve

clean:
	rm -f *~
	rm -f *.o

run:
	./carve dog.jpg dog_2.jpg 500 420