////////////////////////////////////////////////////////////////////////
//
// glut_wrap.h: Wrap up GLUT-calling code to increase reuse.
//
// Copyright (c) Simon Frankau 2018
//

#ifndef RADIOSITY_GLUT_WRAP_H
#define RADIOSITY_GLUT_WRAP_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "geom.h"

// Set the flags etc. up for rendering a cube map. Returns window id.
int gwTransferSetup(int size);

// Run the display function a single time, entering and leaving the
// event loop. It might be possible to draw this outside the event
// loop, but I haven't poked around the event loop code enough to be
// sure.
void gwRenderOnce(void (*f)());

// Camera object, used to pass camera set-up to the
// TransferCalculators.
class Camera
{
public:
    Camera(Vertex const &eyePos, Vertex const &lookAt, Vertex const &upDir);

    static Camera baseCamera;

    Vertex getEyePos() const;
    Vertex getLookAt() const;
    Vertex getUpDir() const;

    void applyViewTransform() const;

private:
    Vertex const m_eyePos;
    Vertex const m_lookAt;
    Vertex const m_upDir;
};

#endif // RADIOSITY_GLUT_WRAP_H
