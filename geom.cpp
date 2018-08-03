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

Vertex::Vertex(double ix, double iy, double iz)
    : p {ix, iy, iz}
{
}

Vertex::Vertex(Vertex const &v)
    : p {v.x(), v.y(), v.z()}
{
}

double Vertex::len() const
{
    return std::sqrt(x() * x() + y() * y() + z() * z());
}

Vertex Vertex::norm() const
{
    double l = len();
    return Vertex(x() / l, y() / l, z() / l);
}

Vertex Vertex::operator-(Vertex const &rhs) const
{
    return Vertex(x() - rhs.x(), y() - rhs.y(), z() - rhs.z());
}

double Vertex::x() const { return p[0]; }
double Vertex::y() const { return p[1]; }
double Vertex::z() const { return p[2]; }

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
double dot(Vertex const &v1, Vertex const &v2)
{
    return v1.x() * v2.x()
         + v1.y() * v2.y()
         + v1.z() * v2.z();
}

// Linear interpolation. 0 returns v1, 1 returns v2.
Vertex lerp(Vertex const &v1, Vertex const &v2, double i)
{
    double j = 1.0 - i;
    return Vertex(v1.x() * j + v2.x() * i,
                  v1.y() * j + v2.y() * i,
                  v1.z() * j + v2.z() * i);
}

////////////////////////////////////////////////////////////////////////
// Quad

Quad::Quad(GLint v1, GLint v2, GLint v3, GLint v4, double l)
    : indices { v1, v2, v3, v4 }, isEmitter(false), light(l), brightness(0)
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
    glColor3d(brightness, brightness, brightness);
    glNormal3dv(n.p);
    glVertex3dv(v0.p);
    glVertex3dv(v1.p);
    glVertex3dv(v2.p);
    glVertex3dv(v3.p);
    glEnd();
}

void Quad::renderIndex(int index, std::vector<Vertex> const &v) const
{
    Vertex const &v0 = v[indices[0]];
    Vertex const &v1 = v[indices[1]];
    Vertex const &v2 = v[indices[2]];
    Vertex const &v3 = v[indices[3]];
    Vertex n = cross(v3 - v0, v1 - v0).norm();
    glBegin(GL_QUADS);
    // We're not using that many polys, so skip the low bits.
    glColor3ub((index << 2) & 0xFC, (index >> 4) & 0xFC, (index >> 10) & 0xFC);
    glNormal3dv(n.p);
    glVertex3dv(v0.p);
    glVertex3dv(v1.p);
    glVertex3dv(v2.p);
    glVertex3dv(v3.p);
    glEnd();
}

// Return the centre of the quad. Assumes paralellogram.
Vertex paraCentre(Quad const &q, std::vector<Vertex> const &vs)
{
    return lerp(vs[q.indices[0]], vs[q.indices[2]], 0.5);
}

// Return the vector for the cross product of the edges - this will
// have length proportional to area, and be normal to the quad.
// Also assumes parallelogram.
Vertex paraCross(Quad const &q, std::vector<Vertex> const &vs)
{
    Vertex const &v0 = vs[q.indices[0]];
    Vertex const &v1 = vs[q.indices[1]];
    Vertex const &v3 = vs[q.indices[3]];
    return cross(v3 - v0, v1 - v0);
}

// Find area of given parallelogram.
double paraArea(Quad const &q, std::vector<Vertex> const &vs)
{
    return paraCross(q, vs).len();
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
            Vertex u0 = lerp(v0, v1, static_cast<double>(u) / uCount);
            Vertex u1 = lerp(v3, v2, static_cast<double>(u) / uCount);
            Vertex pt = lerp(u0, u1, static_cast<double>(v) / vCount);
            vs.push_back(pt);
        }
    }

    // Build the corners of the quads.
    for (GLint v = 0; v < vCount; ++v) {
        for (GLint u = 0; u < uCount; ++u) {
            GLint base = offset + v * (uCount + 1) + u;
            qs.push_back(Quad(base, base + 1,
                              base + uCount + 2, base + uCount + 1,
                              quad.light));
            qs.back().isEmitter = quad.isEmitter;
        }
    }
}

////////////////////////////////////////////////////////////////////////
// Basic shapes.

// Brightness of the walls, etc.
static const double B = 0.7;

std::vector<Quad> const cubeFaces = {
    Quad(1, 0, 2, 3, B), Quad(3, 2, 6, 7, B), Quad(7, 6, 4, 5, B),
    Quad(5, 4, 0, 1, B), Quad(4, 6, 2, 0, B), Quad(7, 5, 1, 3, B)
};

std::vector<Vertex> const cubeVertices = {
    Vertex(-1, -1, -1),
    Vertex(-1, -1, +1),
    Vertex(-1, +1, -1),
    Vertex(-1, +1, +1),
    Vertex(+1, -1, -1),
    Vertex(+1, -1, +1),
    Vertex(+1, +1, -1),
    Vertex(+1, +1, +1),
};
