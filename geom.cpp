////////////////////////////////////////////////////////////////////////
//
// geom.cpp: Points, primitives, etc.
//
// (C) Copyright 2018 Simon Frankau
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
// Vertex

Vertex::Vertex(GLfloat ix, GLfloat iy, GLfloat iz)
    : p {ix, iy, iz}
{
}

Vertex::Vertex(Vertex const &v)
    : p {v.x(), v.y(), v.z()}
{
}

GLfloat Vertex::len() const
{
    return std::sqrt(x() * x() + y() * y() + z() * z());
}

Vertex Vertex::norm() const
{
    GLfloat l = len();
    return Vertex(x() / l, y() / l, z() / l);
}

Vertex Vertex::operator-(Vertex const &rhs) const
{
    return Vertex(x() - rhs.x(), y() - rhs.y(), z() - rhs.z());
}

GLfloat Vertex::x() const { return p[0]; }
GLfloat Vertex::y() const { return p[1]; }
GLfloat Vertex::z() const { return p[2]; }

std::ostream &operator<<(std::ostream &os, Vertex const &v)
{
    return os << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
}

// Cross product.
Vertex cross(Vertex const &v1, Vertex const &v2)
{
    return Vertex(v1.y() * v2.z() - v1.z() * v2.y(),
                  v1.z() * v2.x() - v1.x() * v2.z(),
                  v1.x() * v2.y() - v1.y() * v2.x());
}

// Dot product.
GLfloat dot(Vertex const &v1, Vertex const &v2)
{
    return v1.x() * v2.x()
         + v1.y() * v2.y()
         + v1.z() * v2.z();
}

// Linear interpolation. 0 returns v1, 1 returns v2.
Vertex lerp(Vertex const &v1, Vertex const &v2, GLfloat i)
{
    GLfloat j = 1.0 - i;
    return Vertex(v1.x() * j + v2.x() * i,
                  v1.y() * j + v2.y() * i,
                  v1.z() * j + v2.z() * i);
}

////////////////////////////////////////////////////////////////////////
// Quad

Quad::Quad(GLint v1, GLint v2,
     GLint v3, GLint v4,
     GLfloat b)
    : indices { v1, v2, v3, v4 }, brightness(b)
{
}

Quad::Quad(GLint v1, GLint v2,
     GLint v3, GLint v4)
    : Quad(v1, v2, v3, v4, 1.0)
{
}

void Quad::render(std::vector<Vertex> const &v) const
{
    Vertex const &v0 = v[indices[0]];
    Vertex const &v1 = v[indices[1]];
    Vertex const &v2 = v[indices[2]];
    Vertex const &v3 = v[indices[3]];
    Vertex n = cross(v3 - v0, v1 - v0).norm();
    glBegin(GL_QUADS);
    glColor3f(brightness, brightness, brightness);
    glNormal3fv(n.p);
    glVertex3fv(v0.p);
    glVertex3fv(v1.p);
    glVertex3fv(v2.p);
    glVertex3fv(v3.p);
    glEnd();
}

// Break apart the given quad into a bunch of quads, add them to "qs",
// and add the new vertices to "vs".
void subdivide(Quad const &quad,
               std::vector<Vertex> &vs,
               std::vector<Quad> &qs,
               GLint uCount, GLint vCount)
{
    GLint offset = vs.size();
    Vertex v0 = vs[quad.indices[0]];
    Vertex v1 = vs[quad.indices[1]];
    Vertex v2 = vs[quad.indices[2]];
    Vertex v3 = vs[quad.indices[3]];

    // Generate the grid of points we will build the quads from.
    for (GLint v = 0; v < vCount + 1; ++v) {
        for (GLint u = 0; u < uCount + 1; ++u) {
            Vertex u0 = lerp(v0, v1, static_cast<GLfloat>(u) / uCount);
            Vertex u1 = lerp(v3, v2, static_cast<GLfloat>(u) / uCount);
            Vertex pt = lerp(u0, u1, static_cast<GLfloat>(v) / vCount);
            vs.push_back(pt);
        }
    }

    // Build the corners of the quads.
    for (GLint v = 0; v < vCount; ++v) {
        for (GLint u = 0; u < uCount; ++u) {
            GLint base = offset + v * (uCount + 1) + u;
            // Slightly arbitrary colour with which to see the
            // tesselation pattern.
            qs.push_back(Quad(base, base + 1,
                              base + uCount + 2, base + uCount + 1));
        }
    }
}
