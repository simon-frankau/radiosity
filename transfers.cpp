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

// TODO: Copied code that should be factored out.

// Brightness of the walls, etc.
static const GLfloat b = 0.7;

static const int WIDTH = 512;
static const int HEIGHT = 512;

static const int NUM_CHANS = 4;

// Vertex indices for the 6 faces of a cube.
std::vector<Quad> origFaces = {
    Quad(1, 0, 2, 3, b), Quad(3, 2, 6, 7, b), Quad(7, 6, 4, 5, b),
    Quad(5, 4, 0, 1, b), Quad(4, 6, 2, 0, b), Quad(7, 5, 1, 3, b)
};

// Will be initialised with the subdivided faces.
std::vector<Quad> faces;

// Vector of vertices will grow with new vertices, this is just the
// initial set defined for the initial cube.
std::vector<Vertex> vertices = {
    Vertex(-1, -1, -1),
    Vertex(-1, -1, +1),
    Vertex(-1, +1, -1),
    Vertex(-1, +1, +1),
    Vertex(+1, -1, -1),
    Vertex(+1, -1, +1),
    Vertex(+1, +1, -1),
    Vertex(+1, +1, +1),
};

// Subdivide the faces
void initGeometry(void)
{
    for (std::vector<Quad>::const_iterator iter = origFaces.begin(),
             end = origFaces.end(); iter != end; ++iter) {
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
    // Flat shading.
    glEnable(GL_COLOR_MATERIAL);
    // Use depth buffering for hidden surface elimination.
    glEnable(GL_DEPTH_TEST);
    // Back-face culling.
    glEnable(GL_CULL_FACE);
    // To read from the scene...
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Setup the view of the cube. Will become a view from inside the
    // cube.
    glMatrixMode(GL_PROJECTION);
    gluPerspective(90.0,  // Field of view in degrees
                   1.0,   // Aspect ratio
                   0.1,   // Z near
                   10.0); // Z far
    glMatrixMode(GL_MODELVIEW);
    viewBack();
    gluLookAt(0.0, 0.0, 0.0, // Eye position
              0.0, 0.0, -1.0, // Looking at
              0.0, 1.0, 0.); // Up is in positive Y direction
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // RGB -> RGBA
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Radiosity demo thing");
    glutDisplayFunc(display);
    initGL();
    initGeometry();
    glutMainLoop();
    return 0;
}
