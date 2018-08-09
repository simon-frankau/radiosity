////////////////////////////////////////////////////////////////////////
//
// weighting_test.cpp: Tests for weighting.cpp.
//
// Copyright (c) Simon Frankau 2018
//

#include <cmath>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "weighting.h"

class WeightingTestCase : public CppUnit::TestCase
{
private:
    const int RESOLUTION = 512;

    CPPUNIT_TEST_SUITE(WeightingTestCase);
    CPPUNIT_TEST(testProjSubtendWeightsSumToOne);
    CPPUNIT_TEST(testCalcSubtendWeightsSumToOne);
    CPPUNIT_TEST(testWeightsMatch);
    CPPUNIT_TEST(testCalcLightWeightsSumToOne);
    CPPUNIT_TEST_SUITE_END();

    void testProjSubtendWeightsSumToOne();
    void testCalcSubtendWeightsSumToOne();
    void testWeightsMatch();
    void testCalcLightWeightsSumToOne();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(WeightingTestCase, "WeightingTestCase");

void WeightingTestCase::testProjSubtendWeightsSumToOne()
{
    std::vector<double> weights;
    projSubtendWeights(RESOLUTION, weights);
    double total = 0.0;
    for (int i = 0, n = weights.size(); i < n; ++i) {
        total += weights[i];
    }
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, total, 1.0e-5);
}

void WeightingTestCase::testCalcSubtendWeightsSumToOne()
{
    std::vector<double> weights;
    calcSubtendWeights(RESOLUTION, weights);
    double total = 0.0;
    for (int i = 0, n = weights.size(); i < n; ++i) {
        total += weights[i];
    }
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, total, 1.0e-5);
}

void WeightingTestCase::testWeightsMatch()
{
    std::vector<double> ws, ws2;
    projSubtendWeights(RESOLUTION, ws);
    calcSubtendWeights(RESOLUTION, ws2);
    for (int i = 0, n = ws.size(); i < n; ++i) {
        double error = fabs(1 - ws[i] / ws2[i]);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, error, 1.0 / RESOLUTION);
    }
}

void WeightingTestCase::testCalcLightWeightsSumToOne()
{
    std::vector<double> frontWeights;
    calcForwardLightWeights(RESOLUTION, frontWeights);
    double totalFrontWeight = 0.0;
    for (int i = 0, n = frontWeights.size(); i < n; ++i) {
        totalFrontWeight += frontWeights[i];
    }

    std::vector<double> sideWeights;
    calcSideLightWeights(RESOLUTION, sideWeights);
    double totalSideWeight = 0.0;
    for (int i = 0, n = sideWeights.size(); i < n; ++i) {
        totalSideWeight += sideWeights[i];
    }

    double totalWeight = totalFrontWeight + 4 * totalSideWeight;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, totalWeight, 1e-5);
}
