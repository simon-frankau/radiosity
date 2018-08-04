////////////////////////////////////////////////////////////////////////
//
// transfers_test.cpp: Tests for transfers.cpp.
//
// (c) Copyright Simon Frankau 2018
//

#include <cmath>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "geom.h"
#include "glut_wrap.h"
#include "transfers.h"
#include "weighting.h"

class TransfersTestCase : public CppUnit::TestCase
{
private:
    const int RESOLUTION = 512;

    CPPUNIT_TEST_SUITE(TransfersTestCase);
    CPPUNIT_TEST(renderEachFaceIsAreaOne);
    CPPUNIT_TEST(renderEachFaceIsAreaOneWithDifferentResolution);
    CPPUNIT_TEST(renderEachFaceIsAreaOneWithDifferentDirection);
    CPPUNIT_TEST(analyticTotalAreaIsSix);
    CPPUNIT_TEST(analyticVsRenderSubtended);
    CPPUNIT_TEST_SUITE_END();

    void renderEachFaceIsAreaOne();
    void renderEachFaceIsAreaOneWithDifferentResolution();
    void renderEachFaceIsAreaOneWithDifferentDirection();
    void analyticTotalAreaIsSix();
    void analyticVsRenderSubtended();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TransfersTestCase, "TransfersTestCase");

extern void calcCube(std::vector<Vertex> const &vertices,
                     std::vector<Quad> const &faces,
                     std::vector<double> const &weights);

void TransfersTestCase::renderEachFaceIsAreaOne()
{
    RenderTransferCalculator tc(cubeVertices, cubeFaces, RESOLUTION);
    std::vector<double> sums = tc.calcSubtended(Camera::baseCamera);
    for (int i = 0; i < sums.size(); ++i) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, sums[i], 1.0e-6);
    }
}

void TransfersTestCase::renderEachFaceIsAreaOneWithDifferentResolution()
{
    RenderTransferCalculator tc(cubeVertices, cubeFaces, RESOLUTION * 4);
    std::vector<double> sums = tc.calcSubtended(Camera::baseCamera);
    for (int i = 0; i < sums.size(); ++i) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, sums[i], 1.0e-6);
    }
}

void TransfersTestCase::renderEachFaceIsAreaOneWithDifferentDirection()
{
    Camera cam(Vertex(0.0, 0.0, 0.0),  // Still at origin
               Vertex(1.0, 3.0, 7.0),  // Look at arbitrary direction.
               Vertex(1.0, 2.0, 0.0)); // Arbitrary 'up'.

    RenderTransferCalculator tc(cubeVertices, cubeFaces, RESOLUTION);
    std::vector<double> sums = tc.calcSubtended(cam);
    for (int i = 0; i < sums.size(); ++i) {
        // Slightly larger error, but not much.
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, sums[i], 5.0e-5);
    }
}

void TransfersTestCase::analyticTotalAreaIsSix()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, 32, 32);
    }
    AnalyticTransferCalculator tc(vertices, quads);
    std::vector<double> sums = tc.calcSubtended(Camera::baseCamera);
    double total = 0.0;
    for (int i = 0; i < sums.size(); ++i) {
        total += sums[i];
    }
    // Tolerance is roughly of order 1/(32*32).
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, total, 2.0e-3);
}

void TransfersTestCase::analyticVsRenderSubtended()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, 32, 32);
    }
    AnalyticTransferCalculator atc(vertices, quads);
    std::vector<double> analyticAreas = atc.calcSubtended(Camera::baseCamera);

    RenderTransferCalculator rtc(vertices, quads, 512);
    std::vector<double> renderAreas = rtc.calcSubtended(Camera::baseCamera);

    CPPUNIT_ASSERT_EQUAL(analyticAreas.size(), renderAreas.size());
    for (int i = 0; i < analyticAreas.size(); ++i) {
        double relError = std::fabs(renderAreas[i] / analyticAreas[i] - 1);
        CPPUNIT_ASSERT(relError < 0.001);
    }
}
