CXXFLAGS = -g -O3 -I /usr/include/eigen3/
CXX = g++

carve: seamcarve.cpp
	$(CXX) $(CXXFLAGS) seamcarve.cpp -o seamcarve

clean:
	rm -f *~
	rm -f *.o

run:
	./seamcarve dog.jpg dog_2.jpg 400 420