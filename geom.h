////////////////////////////////////////////////////////////////////////
//
// geom.h: Points, primitives, etc.
//
// Copyright (C) 2018 Simon Frankau
//

#ifndef RADIOSITY_GEOM_H
#define RADIOSITY_GEOM_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>

////////////////////////////////////////////////////////////////////////
// Yet another 3d point class

class Vertex
{
public:
    Vertex(double ix, double iy, double iz);
    Vertex(Vertex const &v);

    double len() const;
    Vertex norm() const;
    Vertex perp() const;
    Vertex scale(double s) const;

    Vertex operator+(Vertex const &rhs) const;
    Vertex operator-(Vertex const &rhs) const;

    double x() const;
    double y() const;
    double z() const;

    double p[3];
};

std::ostream &operator<<(std::ostream &os, Vertex const &v);

// Cross product.
Vertex cross(Vertex const &v1, Vertex const &v2);

// Dot product.
double dot(Vertex const &v1, Vertex const &v2);

// Orthogonalise v1, taking away the v2 component.
Vertex orthog(Vertex const &v1, Vertex const &v2);

// Linear interpolation. 0 returns v1, 1 returns v2.
Vertex lerp(Vertex const &v1, Vertex const &v2, double i);

////////////////////////////////////////////////////////////////////////
// Colours

// Simple red, green and blue components aren't particularly
// realistic, physically, but they'll do for us.
class Colour
{
public:
    Colour(double red, double green, double blue);
    Colour(Colour const &c);
    Colour();

    Colour operator*(double x) const;
    Colour operator*(Colour const &c) const;
    Colour &operator+=(Colour const &c);

    double asGrey() const;

    double r, g, b;
};

////////////////////////////////////////////////////////////////////////
// And a quadrilateral

class Quad
{
public:
    Quad(int v1, int v2,
         int v3, int v4,
         Colour const &c);

    void render(std::vector<Vertex> const &v) const;
    // For transfer calculations.
    void renderIndex(int index, std::vector<Vertex> const &v) const;

    int indices[4];
    // Does this quad emit light, or just reflect?
    bool isEmitter;
    // How much light it emits, or the fraction reflected.
    Colour materialColour;
    // On-screen colour.
    Colour screenColour;
};

// Return the centre of the quad. Assumes paralellogram.
Vertex paraCentre(Quad const &q, std::vector<Vertex> const &vs);

// Return the vector for the cross product of the edges - this will
// have length proportional to area, and be normal to the quad.
// Also assumes parallelogram.
Vertex paraCross(Quad const &q, std::vector<Vertex> const &vs);

// Find area of given parallelogram.
double paraArea(Quad const &q, std::vector<Vertex> const &vs);

// Translate the given quads, in-place
void translate(Vertex const &t,
           std::vector<Quad> &qs,
           std::vector<Vertex> &vs);

// Scale the given quads, in-place.
void scale(double s,
           std::vector<Quad> &qs,
           std::vector<Vertex> &vs);

// Rotate the given quads, in-place.
void rotate(Vertex const &axis,
            double angle,
            std::vector<Quad> &qs,
            std::vector<Vertex> &vs);

// Flip the facing direction of the given quads, in-place.
void flip(std::vector<Quad> &qs,
          std::vector<Vertex> &vs);

////////////////////////////////////////////////////////////////////////
// Gouraud-shaded quad, used only for final rendering.

class GouraudQuad
{
public:
    GouraudQuad(int v1, int v2, int v3, int v4,
                Colour c1, Colour c2, Colour c3, Colour c4);

    void render(std::vector<Vertex> const &v) const;

private:
    int m_indices[4];
    Colour m_colours[4];
};

////////////////////////////////////////////////////////////////////////
// Subdivision

// Structure to hold the information tying together an
// initial quad and a subdivided quad
class SubdivInfo
{
public:
    SubdivInfo(int uCount, int vCount, int vertexStart, int faceStart);

    void generateGouraudQuads(std::vector<Vertex> const &vs,
                              std::vector<Quad> const &qsIn,
                              std::vector<GouraudQuad> &qsOut);

private:
    friend class GeomTestCase;

    int m_uCount;
    int m_vCount;
    int m_vertexStart;
    int m_faceStart;
};

// Break apart the given quad into a bunch of quads, add them to "qs",
// and add the new vertices to "vs".
SubdivInfo subdivide(Quad const &quad,
                     std::vector<Vertex> &vs,
                     std::vector<Quad> &qs,
                     int uCount, int vCount);

////////////////////////////////////////////////////////////////////////
// Basic shapes.

extern std::vector<Vertex> const cubeVertices;
extern std::vector<Quad>   const cubeFaces;

#endif // RADIOSITY_GEOM_H
