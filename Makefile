all: bin/cube bin/test

C_FLAGS=-Wno-deprecated-declarations

clean:
	rm -rf bin/

bin/cube: cube.cpp geom.cpp glut_wrap.cpp geom.cpp transfers.cpp weighting.cpp
	mkdir -p bin
	g++ ${C_FLAGS} -std=c++11 -framework GLUT -framework OpenGL $^ -O2 -o $@

bin/test: weighting.cpp weighting_test.cpp geom.cpp geom_test.cpp test.cpp transfers.cpp transfers_test.cpp glut_wrap.cpp
	mkdir -p bin
	g++ ${C_FLAGS} -std=c++11 -l cppunit -framework GLUT -framework OpenGL $^ -O2 -o $@
