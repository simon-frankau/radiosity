////////////////////////////////////////////////////////////////////////
//
// weighting.h: Weightings to apply to a rendered image to get
// how much light comes from which point.
//
// Copyright (c) Simon Frankau 2018
//

#ifndef RADIOSITY_WEIGHTING_H
#define RADIOSITY_WEIGHTING_H

#include <vector>

// Generate a weightings array based on projecting rectangles onto a
// sphere. Assumes 90 degree field of view.
void projSubtendWeights(int resolution, std::vector<double> &weights);

// Like calcWeights, but generated analytically rather than through
// finite differences.
void calcSubtendWeights(int resolution, std::vector<double> &weights);

// Calculate forward-facing weights. Like calcSubtendWeights, but with an extra
// cos(theta) factor for the angle from facing direction.
void calcForwardLightWeights(int resolution, std::vector<double> &weights);

// Calculate sideways-facing weights. Like calcForwardLightWeights,
// but for the sideways-facing cube maps.
void calcSideLightWeights(int resolution, std::vector<double> &weights);

#endif // RADIOSITY_WEIGHTING_H
