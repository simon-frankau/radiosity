all: bin/cube bin/weights bin/transfers

clean:
	rm -rf bin/

bin/cube: cube.cpp geom.cpp
	mkdir -p bin
	g++ -std=c++11 -framework GLUT -framework OpenGL $^ -O2 -o $@

bin/weights: weighting.cpp geom.cpp
	mkdir -p bin
	g++ -std=c++11 -framework GLUT -framework OpenGL $^ -O2 -o $@

bin/transfers: transfers.cpp geom.cpp
	mkdir -p bin
	g++ -std=c++11 -framework GLUT -framework OpenGL $^ -O2 -o $@
