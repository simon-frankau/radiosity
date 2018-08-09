////////////////////////////////////////////////////////////////////////
//
// transfers.h: Calculate the weights used for working out how the
// light is transferred between elements.
//
// Copyright (c) Simon Frankau 2018
//

#ifndef RADIOSITY_TRANSFERS_H
#define RADIOSITY_TRANSFERS_H

// This class holds all the state that stays the same as we repeatedly
// render the scene from different views to calculate the light
// transfers.
class RenderTransferCalculator
{
public:
    RenderTransferCalculator(std::vector<Vertex> const &vertices,
                             std::vector<Quad> const &faces,
                             int resolution);

    virtual ~RenderTransferCalculator();

    // Calculate the subtended-angle and incoming light from a
    // particular camera view.
    std::vector<double> calcSubtended(Camera const &cam);
    std::vector<double> calcLight(Camera const &cam);
    // Calculate the light for all polys, as if we have a camera at
    // each poly.
    void calcAllLights(std::vector<double> &weights);

private:
    typedef void (*viewFn_t)();

    void render(void);
    void sumWeights(std::vector<double> const &weights);
    void calcFace(Camera const &cam,
                  viewFn_t view,
                  std::vector<double> const &weights);

    // Caches of weights.
    std::vector<double> const &getSubtendWeights();
    std::vector<double> const &getForwardLightWeights();
    std::vector<double> const &getSideLightWeights();

    // Geometry.
    std::vector<Vertex> const &m_vertices;
    std::vector<Quad> const &m_faces;
    // Rendering resolution.
    int const m_resolution;
    // Window id.
    int const m_win;

    // Weighting tables.
    std::vector<double> m_subtendWeights;
    std::vector<double> m_forwardLightWeights;
    std::vector<double> m_sideLightWeights;

    // Sums being calculated.
    std::vector<double> m_sums;
};

// Calculate an analytic approximation. Assume nothing obscuring the
// view, and the polys are small.
class AnalyticTransferCalculator
{
public:
    AnalyticTransferCalculator(std::vector<Vertex> const &vertices,
                               std::vector<Quad> const &faces);

    std::vector<double> calcSubtended(Camera const &cam);
    std::vector<double> calcLight(Camera const &cam);
    void calcAllLights(std::vector<double> &weights);

private:
    double calcSingleQuadSubtended(Camera const &cam, Quad const &q) const;
    double calcSingleQuadLight(Camera const &cam, Quad const &quad) const;

    // Geometry.
    std::vector<Vertex> const &m_vertices;
    std::vector<Quad> const &m_faces;
};

#endif // RADIOSITY_TRANSFERS_H
