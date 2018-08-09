C_FLAGS=-Wno-deprecated-declarations

$(shell mkdir -p bin/ obj/ >/dev/null)

.PHONY: all clean test

all: bin/cube bin/test

clean:
	rm -rf bin/ obj/

test: bin/test
	bin/test

-include obj/*.d

obj/%.o: %.cpp
	g++ -c ${C_FLAGS} -O2 -std=c++11 -o $@ $<
	g++ -MM ${C_FLAGS} $< | sed "s|^|obj/|" > $(@:.o=.d)

bin/cube: obj/cube.o obj/geom.o obj/glut_wrap.o obj/geom.o obj/transfers.o obj/weighting.o
	g++ -framework GLUT -framework OpenGL $^ -o $@

bin/test: obj/weighting.o obj/weighting_test.o obj/geom.o obj/geom_test.o obj/test.o obj/transfers.o obj/transfers_test.o obj/glut_wrap.o
	g++ -l cppunit -framework GLUT -framework OpenGL $^ -o $@
