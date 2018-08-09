////////////////////////////////////////////////////////////////////////
//
// glut_wrap.cpp: Wrap up GLUT-calling code to increase reuse.
//
// Copyright (c) Simon Frankau 2018
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <exception>

#include "glut_wrap.h"

static void doNothing()
{
}

int gwTransferSetup(int size)
{
    // Configure window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(size, size);
    int win = glutCreateWindow("Transfer calculator");
    // This is needed as otherwise any glutMainLoop called gets
    // unhappy, even if we've already destroyed the window.
    glutDisplayFunc(doNothing);
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0,  // Field of view in degrees
                   1.0,   // Aspect ratio
                   0.001, // Z near
                   10.0); // Z far

    return win;
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
#ifdef VANILLA_GLUT
    // Reading the definition of 'glutMainLoop', unwinding the stack
    // from the idle function shouldn't break anything. Horrible way
    // to escape, but... meh.
    //
    // While vanilla GLUT allows this, the MacOS GLUT calls into
    // [GLUTApplication run] and gets unhappy if you try to run it
    // twice as it sets up menus etc.
    //
    // Set up helper functions to display once and then throw int the
    // idle function...
    dispFn = f;
    glutDisplayFunc(dispFnWrapper);
    glutIdleFunc(idleFn);
    try {
        glutMainLoop();
    } catch (BreakException &e) {
    }
#else
    // On MacOS glutCheckLoop looks like an alternative. However, this
    // just manages the event loop. It doesn't render unless
    // necessary, and rendering for the purposes of reading the buffer
    // back out works fine by just making OpenGL calls directly,
    // outside the event loop. So, let's just do that.
    f();
#endif
}

Camera::Camera(Vertex const &eyePos, Vertex const &lookAt, Vertex const &upDir)
    : m_eyePos(eyePos), m_lookAt(lookAt), m_upDir(upDir)
{
}

Camera Camera::baseCamera = Camera(Vertex(0.0, 0.0, 0.0),
                                   Vertex(0.0, 0.0, 1.0),
                                   Vertex(0.0, 1.0, 0.0));

Vertex Camera::getEyePos() const
{
    return m_eyePos;
}

Vertex Camera::getLookAt() const
{
    return m_lookAt;
}

Vertex Camera::getUpDir() const
{
    return m_upDir;
}

void Camera::applyViewTransform() const
{
    gluLookAt(m_eyePos.x(), m_eyePos.y(), m_eyePos.z(),
              m_lookAt.x(), m_lookAt.y(), m_lookAt.z(),
              m_upDir.x(), m_upDir.y(), m_upDir.z());
}
