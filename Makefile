all: bin/cube

clean:
	rm -rf bin/

bin/cube: cube.cpp geom.cpp
	mkdir -p bin
	g++ -std=c++11 -framework GLUT -framework OpenGL $^ -o $@
