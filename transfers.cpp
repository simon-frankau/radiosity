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
#include "weighting.h"

////////////////////////////////////////////////////////////////////////
// Functions to face in cube map directions
//

static void viewFront(void)
{
    // Don't need to do anything to look forward
}

static void viewBack(void)
{
    glRotated(180.0, 0.0, 1.0, 0.0);
}

static void viewRight(void)
{
    glRotated(-90.0, 0.0, 0.0, 1.0);
    glRotated(+90.0, 0.0, 1.0, 0.0);
}

static void viewLeft(void)
{
    glRotated(+90.0, 0.0, 0.0, 1.0);
    glRotated(-90.0, 0.0, 1.0, 0.0);
}

static void viewUp(void)
{
    glRotated(180.0, 0.0, 0.0, 1.0);
    glRotated(-90.0, 1.0, 0.0, 0.0);
}

static void viewDown(void)
{
    glRotated(+90.0, 1.0, 0.0, 0.0);
}

typedef void (*viewFn_t)();

static viewFn_t viewFns[] = {
    viewFront, viewBack, viewRight, viewLeft, viewUp, viewDown
};

////////////////////////////////////////////////////////////////////////
// Class that holds all the bits to do transfer calculations.
//

class TransferCalculator
{
public:
    TransferCalculator(std::vector<Vertex> const &vertices,
                       std::vector<Quad> const &faces,
                       std::vector<double> const &weights)
        : m_vertices(vertices), m_faces(faces), m_weights(weights)
    {
    }

private:
    // Geometry.
    std::vector<Vertex> const &m_vertices;
    std::vector<Quad> const &m_faces;
    // Weighting tables to use.
    std::vector<double> const &m_weights;
    // Sums being calculated.
    std::vector<double> m_sums;

static const int NUM_CHANS = 4;

void render(void)
{
    for (int i = 0, n = m_faces.size(); i < n; ++i) {
        m_faces[i].renderIndex(i, m_vertices);
    }
}

void getWeights()
{
    m_sums.clear();
    m_sums.resize(m_faces.size());
    std::vector<GLubyte> pixels(NUM_CHANS * WIDTH * HEIGHT);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    for (int i = 0, n = pixels.size(); i < n; i += 4) {
        // We're not using that many polys, so skip the low bits.
        int index = (pixels[i] + (pixels[i+1] << 6) + (pixels[i+2] << 12)) >> 2;
        m_sums[index] += m_weights[i/4];
    }
}

public:
void calcFace(viewFn_t view)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    view();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render();
    getWeights();
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        std::cout << i << ": " << m_sums[i] << std::endl;
        cubeFaces[i].renderIndex(i, cubeVertices);
    }
    glutSwapBuffers();
}

void calcCube()
{
    // Horrible...
    for (int i=0; i < sizeof(viewFns)/sizeof(viewFns[0]); ++i) {
        calcFace(viewFns[i]);
    }
}
};

// TODO: Temporarily simplify header
void calcCube(std::vector<Vertex> const &vertices,
              std::vector<Quad> const &faces,
              std::vector<double> const &weights)
{
    TransferCalculator tc(vertices, faces, weights);
    tc.calcCube();
}

// TODO:
//  * Pull methods out of class definition
//  * Do weighting over the scene
//  * Put weight calculation (and full-scene weight calculation) into
//    the class
//  * Build unit tests checking the sanity of the results.
//  * Extend unit tests to try different scene rotations and see
//    how it goes.
