////////////////////////////////////////////////////////////////////////
//
// weighting.h: Weightings to apply to a rendered image to get
// how much light comes from which point.
//
// Copyright (c) Simon Frankaus 2018
//

#ifndef RADIOSITY_WEIGHTING_H
#define RADIOSITY_WEIGHTING_H

#include <vector>

// Generate a weightings array based on projecting rectangles onto a
// sphere. Assumes 90 degree field of view.
//
// TODO: This doesn't include cos(angle from normal) factor.
void projSubtendWeights(int resolution, std::vector<double> &weights);

// Like calcWeights, but generated analytically rather than through
// finite differences.
void calcSubtendWeights(int resolution, std::vector<double> &weights);

#endif // RADIOSITY_WEIGHTING_H
