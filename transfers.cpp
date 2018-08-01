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

std::vector<Quad> faces;
std::vector<Vertex> vertices;

// Subdivide the faces
void initGeometry(void)
{
    vertices = cubeVertices;
    for (std::vector<Quad>::const_iterator iter = cubeFaces.begin(),
             end = cubeFaces.end(); iter != end; ++iter) {
        subdivide(*iter, vertices, faces, SUBDIVISION, SUBDIVISION);
    }
}

void drawBox(void)
{
    for (int i = 0, n = faces.size(); i < n; ++i) {
        faces[i].renderIndex(i, vertices);
    }
}

static void getWeights()
{
    std::vector<GLubyte> pixels(NUM_CHANS * WIDTH * HEIGHT);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    for (int i = 0, n = pixels.size(); i < n; i += 4) {
        // We're not using that many polys, so skip the low bits.
        int index = pixels[i] + (pixels[i+1] << 6) + (pixels[i+2] << 12);
        std::cout << index/4 << std::endl;
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBox();
    getWeights();
    glutSwapBuffers();
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


void initGL(void)
{
    gwTransferSetup();
    glMatrixMode(GL_MODELVIEW);
    viewBack();
    gluLookAt(0.0, 0.0, 0.0, // Eye position
              0.0, 0.0, -1.0, // Looking at
              0.0, 1.0, 0.0); // Up is in positive Y direction
}

int main(int argc, char **argv)
{
    gwInit(&argc, argv);

    // glutDisplayFunc(display);
    initGL();
    initGeometry();

    gwRenderOnce(display);
    return 0;
}
