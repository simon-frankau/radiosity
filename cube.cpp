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

////////////////////////////////////////////////////////////////////////
// Constants etc.

// Break up each base quad into subdivision^2 subquads for radiosity
// calculations.
GLint const SUBDIVISION = 16;

////////////////////////////////////////////////////////////////////////
// Yet another 3d point class

class Vertex
{
public:
    Vertex(GLfloat ix, GLfloat iy, GLfloat iz)
        : p {ix, iy, iz}
    {}

    Vertex(Vertex const &v)
        : p {v.x(), v.y(), v.z()}
    {}

    GLfloat len() const
    {
        return std::sqrt(x() * x() + y() * y() + z() * z());
    }

    Vertex norm() const
    {
        GLfloat l = len();
        return Vertex(x() / l, y() / l, z() / l);
    }

    Vertex operator-(Vertex const &rhs) const
    {
        return Vertex(x() - rhs.x(), y() - rhs.y(), z() - rhs.z());
    }

    GLfloat x() const { return p[0]; }
    GLfloat y() const { return p[1]; }
    GLfloat z() const { return p[2]; }

    GLfloat p[3];
};

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
// And a quadrilateral

class Quad
{
public:
    Quad(GLint v1, GLint v2,
         GLint v3, GLint v4,
         GLfloat b)
        : indices { v1, v2, v3, v4 }, brightness(b)
    {}

    Quad(GLint v1, GLint v2,
         GLint v3, GLint v4)
        : Quad(v1, v2, v3, v4, 1.0)
    {}


    void render(std::vector<Vertex> const &v) const
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

    GLint indices[4];
    GLfloat brightness;
};

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

// Ignoring visibility, etc., calculate transfer between two quads.
GLfloat basicTransfer(Quad const &q1, Quad const &q2,
                      std::vector<Vertex> const &vs)
{
    // Find centres of the quads
    Vertex c1 = centre(q1, vs);
    Vertex c2 = centre(q2, vs);
    // Vector from one quad to the other.
    Vertex path = c2 - c1;

    // Inverse square component.
    GLfloat l = path.len();
    GLfloat r2 = 1.0 / (l * l);

    // Find area and angle from the path.
    // TODO: Facing direction.
    path = path.norm();
    GLfloat f1 = fabs(dot(quadCross(q1, vs), path)) * SUBDIVISION * SUBDIVISION / 4.0;
    GLfloat f2 = fabs(dot(quadCross(q2, vs), path)) * SUBDIVISION * SUBDIVISION / 4.0;

    return fmin(1.0, 4.0 * r2 * f1 * f2);
}

void calculateLighting(std::vector<Quad> &qs, std::vector<Vertex> const &vs)
{
    // Choose the quad to act as light source. This is a hacky way to
    // find the one that's the centre of the first face.
    Quad const &centre = qs[SUBDIVISION * (SUBDIVISION + 1) / 2];

    for (std::vector<Quad>::iterator iter = qs.begin(), end = qs.end();
         iter != end; ++iter) {
        iter->brightness = basicTransfer(centre, *iter, vs);
    }
}

////////////////////////////////////////////////////////////////////////
// And the main rendering bit...

// Vertex indices for the 6 faces of a cube.
std::vector<Quad> origFaces = {
    Quad(1, 0, 2, 3), Quad(3, 2, 6, 7), Quad(7, 6, 4, 5),
    Quad(5, 4, 0, 1), Quad(4, 0, 2, 6), Quad(7, 3, 1, 5)
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
    glRotatef(5, 1.0, 0.0, 0.0);
    glRotatef(-5, 0.0, 0.0, 1.0);
    drawBox();
    glutSwapBuffers();
}

void initGL(void)
{
    // Flat shading.
    glEnable(GL_COLOR_MATERIAL);
    // Use depth buffering for hidden surface elimination.
    glEnable(GL_DEPTH_TEST);
    // TODO: Could enable back-face culling

    // Setup the view of the cube. Will become a view from inside the
    // cube.
    glMatrixMode(GL_PROJECTION);
    gluPerspective(40.0,  // Field of view in degrees
                   1.0,   // Aspect ratio
                   1.0,   // Z near
                   10.0); // Z far
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 5.0, // Eye position
              0.0, 0.0, 0.0, // Looking at
              0.0, 1.0, 0.); // Up is in positive Y direction

    // Adjust cube position to be asthetic angle. Will go away when
    // we're just rendering the inside of the space.
    glTranslatef(0.0, 0.0, -1.0);
    glRotatef(60, 1.0, 0.0, 0.0);
    glRotatef(-20, 0.0, 0.0, 1.0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Radiosity demo thing");
    glutDisplayFunc(display);
    initGL();
    initGeometry();
    calculateLighting(faces, vertices);
    glutMainLoop();
    return 0;
}
