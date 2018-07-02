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

Vertex cross(Vertex const &v1, Vertex const &v2)
{
    return Vertex(v1.y() * v2.z() - v1.z() * v2.y(),
                  v1.z() * v2.x() - v1.x() * v2.z(),
                  v1.x() * v2.y() - v1.y() * v2.x());
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

    void render(std::vector<Vertex> const &v)
    {
        Vertex const &v0 = v[indices[0]];
        Vertex const &v1 = v[indices[1]];
        Vertex const &v2 = v[indices[2]];
        Vertex const &v3 = v[indices[3]];
        Vertex n = cross(v3 - v0, v1 - v0).norm();
        glBegin(GL_QUADS);
        glColor3f(brightness,
                  fmod(brightness * 3.7, 1),
                  fmod(brightness * 16.1, 1));
        glNormal3fv(n.p);
        glVertex3fv(v0.p);
        glVertex3fv(v1.p);
        glVertex3fv(v2.p);
        glVertex3fv(v3.p);
        glEnd();
    }

private:
    GLint indices[4];
    GLfloat brightness;
};

////////////////////////////////////////////////////////////////////////
// And the main rendering bit...

// White diffuse light.
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
// Infinite light location.
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

// Vertex indices for the 6 faces of a cube.
std::vector<Quad> faces = {
    Quad(1, 0, 2, 3, 1.0), Quad(3, 2, 6, 7, 0.8), Quad(7, 6, 4, 5, 0.6),
    Quad(5, 4, 0, 1, 0.5), Quad(4, 0, 2, 6, 0.4), Quad(7, 3, 1, 5, 0.3)
};

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

void drawBox(void)
{
    for (int i = 0; i < 6; i++) {
        faces[i].render(vertices);
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

void init(void)
{
    // Flat shading.
    glEnable(GL_COLOR_MATERIAL);
    // Use depth buffering for hidden surface elimination.
    glEnable(GL_DEPTH_TEST);

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
    init();
    glutMainLoop();
    return 0;
}
