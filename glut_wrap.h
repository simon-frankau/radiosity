////////////////////////////////////////////////////////////////////////
//
// glut_wrap.h: Wrap up GLUT-calling code to increase reuse.
//
// Copyright (c) Simon Frankaus 2018
//

#ifndef RADIOSITY_GLUT_WRAP_H
#define RADIOSITY_GLUT_WRAP_H

// TODO: Must be a better way, but those OpenGL deprecation warnings
// are really annoying.
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Set the flags etc. up for rendering a cube map. Returns window id.
int gwTransferSetup(int size);

// Run the display function a single time, entering and leaving the
// event loop. It might be possible to draw this outside the event
// loop, but I haven't poked around the event loop code enough to be
// sure.
void gwRenderOnce(void (*f)());

#endif // RADIOSITY_GLUT_WRAP_H
