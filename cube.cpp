////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////
// Radiosity calculations

// Return the centre of the quad.
Vertex centre(Quad const &q, std::vector<Vertex> const &vs)
{
    return lerp(vs[q.indices[0]], vs[q.indices[2]], 0.5);
}

// Return the vector for the cross product of the edges - this will
// have length proportional to area, and be normal to the quad.
Vertex quadCross(Quad const &q, std::vector<Vertex> const &vs)
{
    Vertex const &v0 = vs[q.indices[0]];
    Vertex const &v1 = vs[q.indices[1]];
    Vertex const &v3 = vs[q.indices[3]];
    return cross(v3 - v0, v1 - v0);
}

GLfloat quadArea(Quad const &q, std::vector<Vertex> const &vs)
{
    return quadCross(q, vs).len();
}

// Ignoring visibility, etc., calculate transfer between two quads.
GLfloat basicTransfer(Quad const &src, Quad const &dst,
                      std::vector<Vertex> const &vs)
{
    // Find centres of the quads
    Vertex c1 = centre(src, vs);
    Vertex c2 = centre(dst, vs);
    // Vector from one quad to the other.
    Vertex path = c2 - c1;

    // Inverse square component.
    GLfloat l = path.len();
    GLfloat r2 = 1.0 / (l * l);

    // Find area and angle from the path.
    // TODO: Facing direction.
    path = path.norm();
    Vertex srcNorm = quadCross(src, vs);
    Vertex dstNorm = quadCross(dst, vs).norm();
    GLfloat f1 = fmax(0, -dot(srcNorm, path));
    GLfloat f2 = fmax(0,  dot(dstNorm, path));

    return fmin(1.0, r2 * f1 * f2 / M_PI);
}

void initLighting(std::vector<Quad> &qs, std::vector<Vertex> const &vs)
{
    for (std::vector<Quad>::iterator iter = qs.begin(), end = qs.end();
         iter != end; ++iter) {
        Vertex c = centre(*iter, vs);
        // Put a big light in the top centre of the box.
        if (fabs(c.x()) < 0.5 && fabs(c.z()) < 0.5 & c.y() > 0) {
            iter->light = iter->brightness = 2.0;
            iter->isEmitter = true;
        }
    }
}

void iterateLighting(std::vector<Quad> &qs, std::vector<Vertex> const &vs)
{
    std::vector<GLfloat> updatedBrightness;

    for (std::vector<Quad>::iterator dstIter = qs.begin(), end = qs.end();
         dstIter != end; ++dstIter) {
        GLfloat newBrightness = 0;
        if (dstIter->isEmitter) {
            // Emission is just like having 1.0 light arrive.
            newBrightness = 1.0;
        } else {
            for (std::vector<Quad>::iterator srcIter = qs.begin(), srcEnd = qs.end();
                 srcIter != srcEnd; ++srcIter) {
                if (&*srcIter == &*dstIter) {
                    continue;
                }
                newBrightness += basicTransfer(*srcIter, *dstIter, vs) * srcIter->brightness;
            }
        }
        updatedBrightness.push_back(newBrightness * dstIter->light);
    }

    for (int i = 0, n = qs.size(); i < n; ++i) {
        qs[i].brightness = updatedBrightness[i];
    }
}

// Calculate the total light in the scene, as area-weight sum of
// brightness.
void calcLight(std::vector<Quad> &qs, std::vector<Vertex> const &vs)
{
    GLfloat totalLight = 0.0;
    for (std::vector<Quad>::iterator iter = qs.begin(), end = qs.end();
         iter != end; ++iter) {
        totalLight += iter->brightness * quadArea(*iter, vs);
    }
    std::cout << "Total light: " << totalLight << std::endl;
}


////////////////////////////////////////////////////////////////////////
// And the main rendering bit...

// Vertex indices for the 6 faces of a cube.
std::vector<Quad> origFaces = {
    Quad(1, 0, 2, 3), Quad(3, 2, 6, 7), Quad(7, 6, 4, 5),
    Quad(5, 4, 0, 1), Quad(4, 6, 2, 0), Quad(7, 5, 1, 3)
};

// Will be initialised with the subdivided faces.
std::vector<Quad> faces;

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
    gluLookAt(0.0, 0.0, 3.0, // Eye position
              0.0, 0.0, 0.0, // Looking at
              0.0, 1.0, 0.); // Up is in positive Y direction
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Radiosity demo thing");
    glutDisplayFunc(display);
    initGL();
    initGeometry();
    initLighting(faces, vertices);
    iterateLighting(faces, vertices);
    iterateLighting(faces, vertices);
    calcLight(faces, vertices);
    glutMainLoop();
    return 0;
}
