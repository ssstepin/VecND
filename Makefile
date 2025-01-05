CXXFLAGS=--std=c++20 -g 

test: main
	./main

main: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp

clean:
	rm -rf main
