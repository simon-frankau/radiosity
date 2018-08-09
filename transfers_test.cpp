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
    const int SUBDIVISION = 32;

    CPPUNIT_TEST_SUITE(TransfersTestCase);
    CPPUNIT_TEST(renderEachFaceIsAreaOne);
    CPPUNIT_TEST(renderEachFaceIsAreaOneWithDifferentResolution);
    CPPUNIT_TEST(renderEachFaceIsAreaOneWithDifferentDirection);
    CPPUNIT_TEST(analyticSubtendedTotalAreaIsSix);
    CPPUNIT_TEST(analyticVsRenderSubtended);
    CPPUNIT_TEST(analyticVsRenderSubtendedOffCentre);
    CPPUNIT_TEST(analyticVsRenderSubtendedOutside);
    CPPUNIT_TEST(analyticTotalLightIsOne);
    CPPUNIT_TEST(analyticVsRenderLight);
    CPPUNIT_TEST(analyticVsRenderLight2);
    CPPUNIT_TEST(baseCameraFacesRightWay);
    CPPUNIT_TEST(backCameraFacesRightWay);
    CPPUNIT_TEST(calcAllLightsWorks);
    CPPUNIT_TEST_SUITE_END();

    void renderEachFaceIsAreaOne();
    void renderEachFaceIsAreaOneWithDifferentResolution();
    void renderEachFaceIsAreaOneWithDifferentDirection();
    void analyticSubtendedTotalAreaIsSix();
    void analyticVsRenderSubtended();
    void analyticVsRenderSubtendedOffCentre();
    void analyticVsRenderSubtendedOutside();
    void analyticTotalLightIsOne();
    void analyticVsRenderLight();
    void analyticVsRenderLight2();
    void baseCameraFacesRightWay();
    void backCameraFacesRightWay();
    void calcAllLightsWorks();
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

void TransfersTestCase::analyticSubtendedTotalAreaIsSix()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, SUBDIVISION, SUBDIVISION);
    }
    AnalyticTransferCalculator tc(vertices, quads);
    std::vector<double> sums = tc.calcSubtended(Camera::baseCamera);
    double total = 0.0;
    for (int i = 0; i < sums.size(); ++i) {
        total += sums[i];
    }
    // Tolerance is roughly of order 1/(SUBDIVISION*SUBDIVISION).
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, total, 2.0e-3);
}

void TransfersTestCase::analyticVsRenderSubtended()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, SUBDIVISION, SUBDIVISION);
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

void TransfersTestCase::analyticVsRenderSubtendedOffCentre()
{
    Camera cam(Vertex(0.1, -0.1, 0.1),  // Slightly off the origin.
               Vertex(0.1, -0.1, 1.0),  // Look in same direction.
               Vertex(0.0,  1.0, 0.0)); // Usual 'up'.

    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, SUBDIVISION, SUBDIVISION);
    }
    AnalyticTransferCalculator atc(vertices, quads);
    std::vector<double> analyticAreas = atc.calcSubtended(cam);

    RenderTransferCalculator rtc(vertices, quads, 512);
    std::vector<double> renderAreas = rtc.calcSubtended(cam);

    CPPUNIT_ASSERT_EQUAL(analyticAreas.size(), renderAreas.size());
    for (int i = 0; i < analyticAreas.size(); ++i) {
        double relError = std::fabs(renderAreas[i] / analyticAreas[i] - 1);
        // Relative errors are pretty big, but smaller than comparing
        // against when the positions don't line up.
        CPPUNIT_ASSERT(relError < 0.1);
    }
}

void TransfersTestCase::analyticVsRenderSubtendedOutside()
{
    Camera cam(Vertex(0.0, 0.0, -1.1),  // Just outside cube.
               Vertex(0.0, 0.0,  1.0),  // The rest, as usual
               Vertex(0.0, 1.0,  0.0));

    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, 16, 16);
    }
    AnalyticTransferCalculator atc(vertices, quads);
    std::vector<double> analyticAreas = atc.calcSubtended(cam);

    RenderTransferCalculator rtc(vertices, quads, 512);
    std::vector<double> renderAreas = rtc.calcSubtended(cam);

    CPPUNIT_ASSERT_EQUAL(analyticAreas.size(), renderAreas.size());
    for (int i = 0; i < analyticAreas.size(); ++i) {
        double relError = std::fabs(renderAreas[i] / analyticAreas[i] - 1);
        // Some faces are back-culled, we don't want NaN.
        if (renderAreas[i] == 0.0 || analyticAreas[i] == 0.0) {
            relError = std::max(std::fabs(renderAreas[i]),
                                std::fabs(analyticAreas[i]));
        }
        // Relative errors are pretty big, but smaller than comparing
        // against when the positions don't line up.
        CPPUNIT_ASSERT(relError < 0.12);
    }
}

void TransfersTestCase::analyticTotalLightIsOne()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, SUBDIVISION, SUBDIVISION);
    }
    AnalyticTransferCalculator tc(vertices, quads);
    std::vector<double> sums = tc.calcLight(Camera::baseCamera);
    double total = 0.0;
    for (int i = 0; i < sums.size(); ++i) {
        total += sums[i];
    }
    // Tolerance is roughly of order 1/(SUBDIVISION*SUBDIVISION).
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, total, 1.0e-3);
}

void TransfersTestCase::analyticVsRenderLight()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, SUBDIVISION, SUBDIVISION);
    }

    AnalyticTransferCalculator atc(vertices, quads);
    std::vector<double> analyticLight = atc.calcLight(Camera::baseCamera);

    RenderTransferCalculator rtc(vertices, quads, 512);
    std::vector<double> renderLight = rtc.calcLight(Camera::baseCamera);

    CPPUNIT_ASSERT_EQUAL(quads.size(), renderLight.size());
    CPPUNIT_ASSERT_EQUAL(analyticLight.size(), renderLight.size());
    for (int i = 0; i < analyticLight.size(); ++i) {
        if (renderLight[i] != 0.0 || analyticLight[i] != 0.0) {
            double relError = std::fabs(renderLight[i] / analyticLight[i] - 1);
            CPPUNIT_ASSERT(relError < 0.003);
        }
    }
}

// Check scores match up with some camera adjustment applied.
void TransfersTestCase::analyticVsRenderLight2()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        subdivide(cubeFaces[i], vertices, quads, SUBDIVISION, SUBDIVISION);
    }

    Camera cam(Vertex(0.1, -0.1, 0.05),
               Vertex(1.0, 1.0, 1.0),
               Vertex(1.0, 0.0, 0.0));

    AnalyticTransferCalculator atc(vertices, quads);
    std::vector<double> analyticLight = atc.calcLight(cam);

    RenderTransferCalculator rtc(vertices, quads, 512);
    std::vector<double> renderLight = rtc.calcLight(cam);

    CPPUNIT_ASSERT_EQUAL(quads.size(), renderLight.size());
    CPPUNIT_ASSERT_EQUAL(analyticLight.size(), renderLight.size());
    for (int i = 0; i < analyticLight.size(); ++i) {
        // This is messy. Exclude small cases which are error-prone
        // and then be a little careful about how we calculate
        // relative error...
        if (renderLight[i] > 1.0e-5 || analyticLight[i] > 1.0e-5) {
            double relError = std::fabs(fmin(renderLight[i], analyticLight[i]) /
                                        fmax(renderLight[i], analyticLight[i]) - 1);
            // And even then, for a few polys, the relative error is pretty large.
            CPPUNIT_ASSERT(relError < 0.15);
        }
    }
}

void TransfersTestCase::baseCameraFacesRightWay()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    // Add the face at z = 1.
    quads.push_back(cubeFaces[5]);
    // And z = -1.
    quads.push_back(cubeFaces[4]);

    // Should be able to see the face in front of camera, not behind.
    AnalyticTransferCalculator atc(vertices, quads);
    std::vector<double> analyticLight = atc.calcLight(Camera::baseCamera);
    CPPUNIT_ASSERT(analyticLight[0] >  0.0);
    CPPUNIT_ASSERT(analyticLight[1] == 0.0);

    RenderTransferCalculator rtc(vertices, quads, 512);
    std::vector<double> renderLight = rtc.calcLight(Camera::baseCamera);
    CPPUNIT_ASSERT(renderLight[0] >  0.0);
    CPPUNIT_ASSERT(renderLight[1] == 0.0);

}

void TransfersTestCase::backCameraFacesRightWay()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    // Add the face at z = 1.
    quads.push_back(cubeFaces[5]);
    // And z = -1.
    quads.push_back(cubeFaces[4]);

    Camera cam(Vertex(0.1, -0.1,  0.05),
               Vertex(0.0,  0.0, -1.0),
               Vertex(1.0,  0.0,  0.0));

    // Should be able to see the face in front of camera, not behind.
    AnalyticTransferCalculator atc(vertices, quads);
    std::vector<double> analyticLight = atc.calcLight(cam);
    CPPUNIT_ASSERT(analyticLight[0] == 0.0);
    CPPUNIT_ASSERT(analyticLight[1] >  0.0);

    RenderTransferCalculator rtc(vertices, quads, 512);
    std::vector<double> renderLight = rtc.calcLight(cam);
    CPPUNIT_ASSERT(renderLight[0] == 0.0);
    CPPUNIT_ASSERT(renderLight[1] >  0.0);
}

void TransfersTestCase::calcAllLightsWorks()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> quads;
    // Add the face at z = 1.
    quads.push_back(cubeFaces[5]);
    // And z = -1.
    quads.push_back(cubeFaces[4]);

    AnalyticTransferCalculator atc(vertices, quads);
    std::vector<double> analyticLight;
    atc.calcAllLights(analyticLight);
    CPPUNIT_ASSERT(isnan(analyticLight[0]));
    CPPUNIT_ASSERT(analyticLight[1] > 0.0);
    CPPUNIT_ASSERT(analyticLight[2] > 0.0);
    CPPUNIT_ASSERT(isnan(analyticLight[3]));

    RenderTransferCalculator rtc(vertices, quads, 512);
    std::vector<double> renderLight;
    rtc.calcAllLights(renderLight);
    CPPUNIT_ASSERT(renderLight[0] == 0.0);
    CPPUNIT_ASSERT(renderLight[1] >  0.0);
    CPPUNIT_ASSERT(renderLight[2] >  0.0);
    CPPUNIT_ASSERT(renderLight[3] == 0.0);
}
