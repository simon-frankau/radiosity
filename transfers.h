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
// render the scene from different views...
class TransferCalculator
{
public:
    TransferCalculator(std::vector<Vertex> const &vertices,
                       std::vector<Quad> const &faces,
                       int resolution);

    virtual ~TransferCalculator();

    std::vector<double> calcSubtended();

private:
    typedef void (*viewFn_t)();

    void render(void);
    void sumWeights(std::vector<double> const &weights);
    void calcFace(viewFn_t view, std::vector<double> const &weights);

    std::vector<double> const &getSubtendWeights();

    // Geometry.
    std::vector<Vertex> const &m_vertices;
    std::vector<Quad> const &m_faces;
    // Rendering resolution.
    int const m_resolution;
    // Window id.
    int const m_win;

    // Weighting tables for area subtended calculations
    std::vector<double> m_subtendWeights;

    // Sums being calculated.
    std::vector<double> m_sums;
};

#endif // RADIOSITY_TRANSFERS_H
