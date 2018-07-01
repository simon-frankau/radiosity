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
// And the main rendering bit...

// White diffuse light.
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
// Infinite light location.
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

// Vertex indices for the 6 faces of a cube.
GLint faces[6][4] = {
    {1, 0, 2, 3}, {3, 2, 6, 7}, {7, 6, 4, 5},
    {5, 4, 0, 1}, {4, 0, 2, 6}, {7, 3, 1, 5} };

Vertex v[8] = {
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
    int i;

    for (i = 0; i < 6; i++) {
        Vertex &v0 = v[faces[i][0]];
        Vertex &v1 = v[faces[i][1]];
        Vertex &v2 = v[faces[i][2]];
        Vertex &v3 = v[faces[i][3]];
        Vertex n = cross(v3 - v0, v1 - v0).norm();
        glBegin(GL_QUADS);
        glNormal3fv(n.p);
        glVertex3fv(v0.p);
        glVertex3fv(v1.p);
        glVertex3fv(v2.p);
        glVertex3fv(v3.p);
        glEnd();
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
    // Enable a single OpenGL light. Should switch to flat lighting
    // once the light is calculated via radiosity.
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

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
