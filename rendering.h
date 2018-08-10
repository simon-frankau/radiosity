////////////////////////////////////////////////////////////////////////
//
// rendering.h: Put the calculated polys on the screen
//
// Copyright (c) Simon Frankau 2018
//

#ifndef RADIOSITY_RENDERING_H
#define RADIOSITY_RENDERING_H

#include "geom.h"

// Render the scene in flat-shaded quads
void renderFlat(std::vector<Quad> f, std::vector<Vertex> v);

// Render the scene with Gouraud shading
void renderGouraud(std::vector<GouraudQuad> f, std::vector<Vertex> v);

// Normalise the brightness of non-emitting components
void normaliseBrightness(std::vector<Quad> &qs, std::vector<Vertex> const &vs);

#endif // RADIOSITY_RENDERING_H
