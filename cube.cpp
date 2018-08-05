////////////////////////////////////////////////////////////////////////
//
// cube.cpp: Calculate radiosity inside a cube.
//
// Copyright (c) Simon Frankaus 2018
//
// Portions Copyright (c) Mark J. Kilgard, 1997, from
// https://www.opengl.org/archives/resources/code/samples/glut_examples/examples/cube.c
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

const double CONVERGENCE_TARGET = 0.001;

////////////////////////////////////////////////////////////////////////
// Radiosity calculations

// Ignoring visibility, etc., calculate transfer between two quads.
double basicTransfer(Quad const &src, Quad const &dst,
                      std::vector<Vertex> const &vs)
{
    // Find centres of the quads
    Vertex c1 = paraCentre(src, vs);
    Vertex c2 = paraCentre(dst, vs);
    // Vector from one quad to the other.
    Vertex path = c2 - c1;

    // Inverse square component.
    double l = path.len();
    double r2 = 1.0 / (l * l);

    // Find area and angle from the path.
    // TODO: Facing direction.
    path = path.norm();
    Vertex srcNorm = paraCross(src, vs);
    Vertex dstNorm = paraCross(dst, vs).norm();
    double f1 = fmax(0, -dot(srcNorm, path));
    double f2 = fmax(0,  dot(dstNorm, path));

    return fmin(1.0, r2 * f1 * f2 / M_PI);
}

void initLighting(std::vector<Quad> &qs, std::vector<Vertex> const &vs)
{
    for (std::vector<Quad>::iterator iter = qs.begin(), end = qs.end();
         iter != end; ++iter) {
        Vertex c = paraCentre(*iter, vs);
        // Put a big light in the top centre of the box.
        if (fabs(c.x()) < 0.5 && fabs(c.z()) < 0.5 & c.y() > 0) {
            iter->light = iter->brightness = 2.0;
            iter->isEmitter = true;
        }
    }
}

void initTransfers(std::vector<Quad> &qs, std::vector<Vertex> const &vs,
                   std::vector<double> &transfers)
{
    int n = qs.size();
    // Iterate over targets
    for (int i = 0; i < n; ++i) {
        // Iterate over sources
        for (int j = 0; j < n; ++j) {
            transfers.push_back(basicTransfer(qs[j], qs[i], vs));
        }
    }

    // Compare with analytic calc
    AnalyticTransferCalculator tc(vs, qs);
    std::vector<double> analyticLight;
    tc.calcAllLights(analyticLight);

    for (int i = 0; i < transfers.size(); ++i) {
        double diff = fabs(transfers[i] / analyticLight[i] - 1);
        if (diff > 1e-15) {
            std::cout << diff << std::endl;
        }
    }
}

void iterateLighting(std::vector<Quad> &qs, std::vector<double> const &transfers)
{
    int n = qs.size();
    std::vector<double> updatedBrightness(n);

    // Iterate over targets
    for (int i = 0; i < n; ++i) {
        double newBrightness = 0;
        if (qs[i].isEmitter) {
            // Emission is just like having 1.0 light arrive.
            newBrightness = 1.0;
        } else {
            // Iterate over sources
            for (int j = 0; j < n; ++j) {
                if (i == j) {
                    continue;
                }
                newBrightness += transfers[i * n + j] * qs[j].brightness;
            }
        }
        updatedBrightness[i] = (newBrightness * qs[i].light);
    }

    for (int i = 0; i < n; ++i) {
        qs[i].brightness = updatedBrightness[i];
    }
}

// Calculate the total light in the scene, as area-weight sum of
// brightness.
double calcLight(std::vector<Quad> &qs, std::vector<Vertex> const &vs)
{
    double totalLight = 0.0;
    for (std::vector<Quad>::iterator iter = qs.begin(), end = qs.end();
         iter != end; ++iter) {
        totalLight += iter->brightness * paraArea(*iter, vs);
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
    for (std::vector<Quad>::const_iterator iter = cubeFaces.begin(),
             end = cubeFaces.end(); iter != end; ++iter) {
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
    initTransfers(faces, vertices, transfers);
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
