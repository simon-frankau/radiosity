////////////////////////////////////////////////////////////////////////
//
// weighting.cpp: Weightings to apply to a rendered image to get
// how much light comes from which point.
//
// Copyright (c) Simon Frankau 2018
//

#include <cmath>

#include "geom.h"
#include "weighting.h"

// Generate a weightings array based on projecting rectangles onto a
// sphere. Assumes 90 degree field of view.
void projSubtendWeights(int resolution, std::vector<double> &weights)
{
    // From -1 to +1.
    double conv = 2.0 / resolution;
    // Make total come to 1.
    double weight = 3.0 / (M_PI * 2.0);

    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution; ++x) {
            Vertex v1 = Vertex(x       * conv - 1, y       * conv - 1, 1);
            Vertex v2 = Vertex((x + 1) * conv - 1, y       * conv - 1, 1);
            Vertex v3 = Vertex(x       * conv - 1, (y + 1) * conv - 1, 1);
            v1 = v1.norm(); v2 = v2.norm(); v3 = v3.norm();
            weights.push_back(weight * cross(v3 - v1, v2 - v1).len());
        }
    }
}

// Like projSubtendWeights, but generated analytically rather than through
// finite differences.
void calcSubtendWeights(int resolution, std::vector<double> &weights)
{
    // From -1 to +1.
    double conv = 2.0 / resolution;
    // Make total come to 1.
    double weight = 3.0 / (M_PI * 2.0);

    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution; ++x) {
            // We're calculating how much a pixel at (px, py) occludes
            // of the unit sphere. We'll axis align it, and calculate
            // it for (sqrt(distSq), 0) instead:
            double px = (x + 0.5) * conv - 1.0;
            double py = (y + 0.5) * conv - 1.0;
            // Distance from centre of projection plane.
            double distSq = px * px + py * py;

            // When we project down from the pixel to the sphere, the
            // x-axis scaling factor is is derivative of arctan, 1/(1
            // + dist^2)...
            double xFactor = 1.0 / (1.0 + distSq);
            // and the y-axis factor is just scaled by 1/(distance
            // from centre of sphere we're projecting onto)
            // (this is just perspective).
            double yFactor = sqrt(xFactor);
            // And the final calculation, throwing in a couple of
            // "conv"s to get the scaling factor right.
            weights.push_back(weight * conv * conv * xFactor * yFactor);
        }
    }
}

// Calculate forward-facing weights. Like calcSubtendWeights, but with an extra
// cos(theta) factor for the angle from facing direction.
void calcForwardLightWeights(int resolution, std::vector<double> &weights)
{
    double conv = 2.0 / resolution;
    double weight = 1.0 / M_PI;

    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution; ++x) {
            // Mostly calculation is same as before...
            double px = (x + 0.5) * conv - 1.0;
            double py = (y + 0.5) * conv - 1.0;
            double distSq = px * px + py * py;
            double xFactor = 1.0 / (1.0 + distSq);
            // We multiply in an extra sqrt(xFactor) to get what we need,
            // which turns the whole thing into an extra xFactor.
            weights.push_back(weight * conv * conv * xFactor * xFactor);
        }
    }
}

// Calculate sideways-facing weights. Like calcForwardLightWeights,
// but for the sideways-facing cube maps.
void calcSideLightWeights(int resolution, std::vector<double> &weights)
{
    double conv = 2.0 / resolution;
    double weight = 1.0 / M_PI;

    for (int y = 0; y < resolution / 2; ++y) {
        for (int x = 0; x < resolution; ++x) {
            // Mostly calculation is same as before...
            double px = (x + 0.5) * conv - 1.0;
            double py = (y + 0.5) * conv - 1.0;
            double distSq = px * px + py * py;
            double xFactor = 1.0 / (1.0 + distSq);
            // The extra weighting needed for the extra cos is yFactor * -py.
            weights.push_back(weight * conv * conv * xFactor * xFactor * -py);
        }
    }
}
