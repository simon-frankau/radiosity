////////////////////////////////////////////////////////////////////////
//
// geom.h: Points, primitives, etc.
//
// (C) Copyright 2018 Simon Frankau
//

#ifndef RADIOSITY_GEOM_H
#define RADIOSITY_GEOM_H

// TODO: Must be a better way, but those OpenGL deprecation warnings
// are really annoying.
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>

////////////////////////////////////////////////////////////////////////
// Constants etc.

// Break up each base quad into subdivision^2 subquads for radiosity
// calculations.
GLint const SUBDIVISION = 32;

////////////////////////////////////////////////////////////////////////
// Yet another 3d point class

class Vertex
{
public:
    Vertex(GLfloat ix, GLfloat iy, GLfloat iz);
    Vertex(Vertex const &v);

    GLfloat len() const;
    Vertex norm() const;

    Vertex operator-(Vertex const &rhs) const;

    GLfloat x() const;
    GLfloat y() const;
    GLfloat z() const;

    GLfloat p[3];
};

std::ostream &operator<<(std::ostream &os, Vertex const &v);

// Cross product.
Vertex cross(Vertex const &v1, Vertex const &v2);

// Dot product.
GLfloat dot(Vertex const &v1, Vertex const &v2);

// Linear interpolation. 0 returns v1, 1 returns v2.
Vertex lerp(Vertex const &v1, Vertex const &v2, GLfloat i);

////////////////////////////////////////////////////////////////////////
// And a quadrilateral

class Quad
{
public:
    Quad(GLint v1, GLint v2,
         GLint v3, GLint v4,
         GLfloat l);

    void render(std::vector<Vertex> const &v) const;
    // For transfer calculations.
    void renderIndex(int index, std::vector<Vertex> const &v) const;

    GLint indices[4];
    // Does this quad emit light, or just reflect?
    bool isEmitter;
    // How much light it emits, or the fraction reflected.
    GLfloat light;
    // On-screen brightness.
    GLfloat brightness;
};

// Break apart the given quad into a bunch of quads, add them to "qs",
// and add the new vertices to "vs".
void subdivide(Quad const &quad,
               std::vector<Vertex> &vs,
               std::vector<Quad> &qs,
               GLint uCount, GLint vCount);

#endif // RADIOSITY_GEOM_H
