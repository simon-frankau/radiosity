all: bin/cube bin/transfers bin/test

clean:
	rm -rf bin/

bin/cube: cube.cpp geom.cpp glut_wrap.cpp
	mkdir -p bin
	g++ -std=c++11 -framework GLUT -framework OpenGL $^ -O2 -o $@

bin/transfers: transfers.cpp geom.cpp glut_wrap.cpp weighting.cpp
	mkdir -p bin
	g++ -std=c++11 -framework GLUT -framework OpenGL $^ -O2 -o $@

bin/test: weighting.cpp weighting_test.cpp geom.cpp test.cpp
	mkdir -p bin
	g++ -std=c++11 -l cppunit -framework GLUT -framework OpenGL $^ -O2 -o $@
