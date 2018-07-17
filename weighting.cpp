////////////////////////////////////////////////////////////////////////
//
// weighting.cpp: Weightings to apply to a rendered image to get
// how much light comes from which point.
//
// Copyright (c) Simon Frankaus 2018
//

#include <math.h>

#include "geom.h"

const int WEIGHT_RESOLUTION = 512;

// Generate a weightings array based on projecting rectangles onto a
// sphere. Assumes 90 degree field of view.
//
// TODO: This doesn't include cos(angle from normal) factor.
void projWeights(std::vector<GLfloat> &weights)
{
    // From -1 to +1.
    GLfloat conv = 2.0 / WEIGHT_RESOLUTION;
    // Make total come to 1.
    GLfloat weight = 3.0 / (M_PI * 2.0);

    for (int y = 0; y < WEIGHT_RESOLUTION; ++y) {
        for (int x = 0; x < WEIGHT_RESOLUTION; ++x) {
            Vertex v1 = Vertex(x       * conv - 1, y       * conv - 1, 1);
            Vertex v2 = Vertex((x + 1) * conv - 1, y       * conv - 1, 1);
            Vertex v3 = Vertex(x       * conv - 1, (y + 1) * conv - 1, 1);
            v1 = v1.norm(); v2 = v2.norm(); v3 = v3.norm();
            weights.push_back(weight * cross(v3 - v1, v2 - v1).len());
        }
    }
}

// Quick test that the integral comes out right.
//
// TODO: I should really get some cpp_unit, shouldn't I?
int main(void)
{
    std::vector<GLfloat> ws;
    projWeights(ws);
    GLfloat total = 0.0;
    for (int i = 0, n = ws.size(); i < n; ++i) {
        total += ws[i];
    }
    std::cout << "Should be about 1.0: " << total << std::endl;
}
