all: bin/cube bin/test

clean:
	rm -rf bin/

bin/cube: cube.cpp geom.cpp glut_wrap.cpp
	mkdir -p bin
	g++ -std=c++11 -framework GLUT -framework OpenGL $^ -O2 -o $@

bin/test: weighting.cpp weighting_test.cpp geom.cpp test.cpp transfers.cpp transfers_test.cpp glut_wrap.cpp
	mkdir -p bin
	g++ -std=c++11 -l cppunit -framework GLUT -framework OpenGL $^ -O2 -o $@
