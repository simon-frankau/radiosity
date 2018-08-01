////////////////////////////////////////////////////////////////////////
//
// glut_wrap.cpp: Wrap up GLUT-calling code to increase reuse.
//
// Copyright (c) Simon Frankaus 2018
//

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <exception>

#include "glut_wrap.h"

void gwInit(int *argc, char **argv)
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Radiosity demo thing");
}

void gwTransferSetup()
{
    // Flat shading.
    glEnable(GL_COLOR_MATERIAL);
    // Use depth buffering for hidden surface elimination.
    glEnable(GL_DEPTH_TEST);
    // Back-face culling.
    glEnable(GL_CULL_FACE);
    // To read from the scene...
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Set up the view to be one face of the cube-map.
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(90.0,  // Field of view in degrees
                   1.0,   // Aspect ratio
                   0.1,   // Z near
                   10.0); // Z far
}

// Not re-entrant but I don't think GLUT is.
static void (*dispFn)() = NULL;

class BreakException : public std::exception {
};

static void idleFn()
{
    if (dispFn == NULL) {
        throw BreakException();
    }
}

static void dispFnWrapper()
{
    if (dispFn) {
        dispFn();
        dispFn = NULL;
    }
}

void gwRenderOnce(void (*f)())
{
    // Set up helper functions to display once and then throw int the
    // idle function...
    dispFn = f;
    glutDisplayFunc(dispFnWrapper);
    glutIdleFunc(idleFn);
    // Reading the definition of 'glutMainLoop', unwinding the stack
    // from the idle function shouldn't break anything. Horrible way
    // to escape, but... meh.
    try {
        glutMainLoop();
    } catch (BreakException &e) {
    }
}
