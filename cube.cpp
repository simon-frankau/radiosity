
/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

/* This program was requested by Patrick Earl; hopefully someone else
   will write the equivalent Direct3D immediate mode program. */

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
    /* Enable a single OpenGL light. */
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    /* Use depth buffering for hidden surface elimination. */
    glEnable(GL_DEPTH_TEST);

    /* Setup the view of the cube. */
    glMatrixMode(GL_PROJECTION);
    gluPerspective( /* field of view in degree */ 40.0,
                    /* aspect ratio */ 1.0,
                    /* Z near */ 1.0, /* Z far */ 10.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
              0.0, 0.0, 0.0,      /* center is at (0,0,0) */
              0.0, 1.0, 0.);      /* up is in positive Y direction */

    /* Adjust cube position to be asthetic angle. */
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
