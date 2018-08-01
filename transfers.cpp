////////////////////////////////////////////////////////////////////////
//
// transfers.cpp: Calculate the weights used for working out how the
// light is transferred between elements.
//
// We render the scene from the view of all elements to see where the
// light comes from.
//
// Copyright (c) Simon Frankaus 2018
//

// TODO: Must be a better way, but those OpenGL deprecation warnings
// are really annoying.
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <cmath>
#include <vector>

#include "geom.h"
#include "glut_wrap.h"

static const int NUM_CHANS = 4;

static std::vector<int> weights;

static void render(void)
{
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        cubeFaces[i].renderIndex(i, cubeVertices);
    }
}

static void getWeights()
{
    weights.clear();
    weights.resize(cubeFaces.size());
    std::vector<GLubyte> pixels(NUM_CHANS * WIDTH * HEIGHT);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    for (int i = 0, n = pixels.size(); i < n; i += 4) {
        // We're not using that many polys, so skip the low bits.
        int index = (pixels[i] + (pixels[i+1] << 6) + (pixels[i+2] << 12)) >> 2;
        weights[index]++;
    }
}

////////////////////////////////////////////////////////////////////////
// TODO: Render the view in multiple directions - make a cube map.

void viewFront(void)
{
    // Don't need to do anything to look forward
}

void viewBack(void)
{
    glRotated(180.0, 0.0, 1.0, 0.0);
    // Don't need to do anything to look forward
}

void viewRight(void)
{
    glRotated(-90.0, 0.0, 0.0, 1.0);
    glRotated(+90.0, 0.0, 1.0, 0.0);
}

void viewLeft(void)
{
    glRotated(+90.0, 0.0, 0.0, 1.0);
    glRotated(-90.0, 0.0, 1.0, 0.0);
}

void viewUp(void)
{
    glRotated(180.0, 0.0, 0.0, 1.0);
    glRotated(-90.0, 1.0, 0.0, 0.0);
}

void viewDown(void)
{
    glRotated(+90.0, 1.0, 0.0, 0.0);
}

typedef void (*viewFn_t)();

viewFn_t viewFn;

viewFn_t viewFns[] = {
    viewFront, viewBack, viewRight, viewLeft, viewUp, viewDown
};

void display(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    viewFn();
    gluLookAt(0.0, 0.0, 0.0, // Eye position
              0.0, 0.0, -1.0, // Looking at
              0.0, 1.0, 0.0); // Up is in positive Y direction

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render();
    getWeights();
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        std::cout << i << ": " << weights[i] << std::endl;
        cubeFaces[i].renderIndex(i, cubeVertices);
    }

    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    gwInit(&argc, argv);
    gwTransferSetup();
    // Horrible...
    for (int i=0; i < sizeof(viewFns)/sizeof(viewFns[0]); ++i) {
        viewFn = viewFns[i];
        // display();
        gwRenderOnce(display);
    }
    return 0;
}
