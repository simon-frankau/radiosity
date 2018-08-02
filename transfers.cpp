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
#include "transfers.h"
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

////////////////////////////////////////////////////////////////////////
// Class that holds all the bits to do transfer calculations.
//

TransferCalculator::TransferCalculator(std::vector<Vertex> const &vertices,
                                       std::vector<Quad> const &faces,
                                       int resolution)
    : m_vertices(vertices),
      m_faces(faces),
      m_resolution(resolution),
      m_win(gwTransferSetup(resolution))
{
}

TransferCalculator::~TransferCalculator()
{
    glutDestroyWindow(m_win);
}

// Extremely simple rendering of the scene.
void TransferCalculator::render(void)
{
    for (int i = 0, n = m_faces.size(); i < n; ++i) {
        m_faces[i].renderIndex(i, m_vertices);
    }
}

static const int NUM_CHANS = 4;

// Sum up value of the pixels, with the given weights.
void TransferCalculator::sumWeights(std::vector<double> const &weights)
{
    std::vector<GLubyte> pixels(NUM_CHANS * m_resolution * m_resolution);
    glReadPixels(0, 0, m_resolution, m_resolution, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    for (int i = 0, n = pixels.size(); i < n; i += 4) {
        // We're not using that many polys, so skip the low bits.
        int index = (pixels[i] + (pixels[i+1] << 6) + (pixels[i+2] << 12)) >> 2;
        m_sums[index] += weights[i/4];
    }
}

// Work out contributions from the given face.
void TransferCalculator::calcFace(viewFn_t view, std::vector<double> const &weights)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    view();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render();
    sumWeights(weights);
    glutSwapBuffers();
}

// Calculate the the area subtended by the faces, using a cube map.
std::vector<double> TransferCalculator::calcSubtended()
{
    m_sums.clear();
    m_sums.resize(m_faces.size());

    std::vector<double> const &ws = getSubtendWeights();

    calcFace(viewFront, ws);
    calcFace(viewBack,  ws);
    calcFace(viewRight, ws);
    calcFace(viewLeft,  ws);
    calcFace(viewUp,    ws);
    calcFace(viewDown,  ws);

    return m_sums;
}

std::vector<double> const &TransferCalculator::getSubtendWeights()
{
    if (m_subtendWeights.empty()) {
        calcSubtendWeights(m_resolution, m_subtendWeights);
    }
    return m_subtendWeights;
}


// TODO:
//  * Extend unit tests to try different scene rotations and see
//    how it goes.
//  * Resizable rendering.
//  * Incoming light calculations.
