////////////////////////////////////////////////////////////////////////
//
// cube.cpp: Calculate radiosity inside a cube.
//
// Copyright (c) Simon Frankau 2018
//

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

// Relative change in total light in the scene by the point we stop
// iterating.
const double CONVERGENCE_TARGET = 0.001;

// Break up each base quad into subdivision^2 subquads for radiosity
// calculations.
GLint const SUBDIVISION = 32;

////////////////////////////////////////////////////////////////////////
// Radiosity calculations

void initLighting(std::vector<Quad> &qs, std::vector<Vertex> const &vs)
{
    for (std::vector<Quad>::iterator iter = qs.begin(), end = qs.end();
         iter != end; ++iter) {
        Vertex c = paraCentre(*iter, vs);
        // Put a big light in the top centre of the box.
        if (fabs(c.x()) < 0.5 && fabs(c.z()) < 0.5 & c.y() > 0.9) {
            iter->materialColour = iter->screenColour = Colour(2.0, 2.0, 2.0);
            iter->isEmitter = true;
        }
    }
}

void iterateLighting(std::vector<Quad> &qs, std::vector<double> const &transfers)
{
    int const n = qs.size();
    std::vector<Colour> updatedColours(n);

    // Iterate over targets
    for (int i = 0; i < n; ++i) {
        Colour incoming;
        if (qs[i].isEmitter) {
            // Emission is just like having 1.0 light arrive.
            incoming = Colour(1.0, 1.0, 1.0);
        } else {
            // Iterate over sources
            for (int j = 0; j < n; ++j) {
                if (i == j) {
                    continue;
                }
                incoming += qs[j].screenColour * transfers[i * n + j];
            }
        }
        updatedColours[i] = incoming * qs[i].materialColour;
    }

    for (int i = 0; i < n; ++i) {
        qs[i].screenColour = updatedColours[i];
    }
}

// Calculate the total light in the scene, as area-weight sum of
// screenColour.
double calcLight(std::vector<Quad> &qs, std::vector<Vertex> const &vs)
{
    double totalLight = 0.0;
    for (std::vector<Quad>::iterator iter = qs.begin(), end = qs.end();
         iter != end; ++iter) {
        totalLight += iter->screenColour.asGrey() * paraArea(*iter, vs);
    }
    return totalLight;
}

////////////////////////////////////////////////////////////////////////
// And the main rendering bit...

// Geometry.
static std::vector<Quad> faces;
static std::vector<Vertex> vertices;
// Array of quad-to-quad light transfers.
static std::vector<double> transfers;

// Subdivide the faces
void initGeometry(void)
{
    vertices = cubeVertices;
    // Start with the inner cube: Take the basic scene cube, scale it
    // down, rotate and move it...
    std::vector<Quad> sceneFaces(cubeFaces); // Enclosed cube
    scale(0.4, sceneFaces, vertices);
    flip(sceneFaces, vertices);
    rotate(Vertex(1.0, 0.0, 0.0), M_PI / 3.0, sceneFaces, vertices);
    rotate(Vertex(0.0, 0.0, 1.0), M_PI / 6.0, sceneFaces, vertices);
    translate(Vertex(0.0, -0.25, 0.0), sceneFaces, vertices);
    // Then add in the scene-holding cube.
    sceneFaces.insert(sceneFaces.end(), cubeFaces.begin(), cubeFaces.end());
    // Finally, copy the subdivided version into 'faces'.
    for (std::vector<Quad>::const_iterator iter = sceneFaces.begin(),
             end = sceneFaces.end(); iter != end; ++iter) {
        subdivide(*iter, vertices, faces, SUBDIVISION, SUBDIVISION);
    }
}

void drawBox(void)
{
    for (std::vector<Quad>::const_iterator iter = faces.begin(),
             end = faces.end(); iter != end; ++iter) {
        iter->render(vertices);
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBox();
    glutSwapBuffers();
}

void initGL(void)
{
    // Flat shading.
    glEnable(GL_COLOR_MATERIAL);
    // Use depth buffering for hidden surface elimination.
    glEnable(GL_DEPTH_TEST);
    // Back-face culling.
    glEnable(GL_CULL_FACE);

    // Setup the view of the cube. Will become a view from inside the
    // cube.
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0,  // Field of view in degrees
                   1.0,   // Aspect ratio
                   1.0,   // Z near
                   10.0); // Z far
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, -3.0, // Eye position
              0.0, 0.0,  0.0, // Looking at
              0.0, 1.0,  0.); // Up is in positive Y direction
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Radiosity demo");

    glutDisplayFunc(display);
    initGL();
    initGeometry();
    initLighting(faces, vertices);
    RenderTransferCalculator(vertices, faces, 256).calcAllLights(transfers);
    double light = 0.0;
    double relChange;
    do {
        iterateLighting(faces, transfers);
        double newLight = calcLight(faces, vertices);
        relChange = fabs(light / newLight - 1.0);
        light = newLight;
        std::cout << "Total light: " << light << std::endl;
    } while (relChange > CONVERGENCE_TARGET);

    glutMainLoop();
    return 0;
}
