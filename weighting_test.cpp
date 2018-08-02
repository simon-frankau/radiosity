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
    CPPUNIT_TEST(projSubtendWeightsSumToOne);
    CPPUNIT_TEST(calcSubtendWeightsSumToOne);
    CPPUNIT_TEST(weightsMatch);
    CPPUNIT_TEST_SUITE_END();

    void projSubtendWeightsSumToOne();
    void calcSubtendWeightsSumToOne();
    void weightsMatch();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(WeightingTestCase, "WeightingTestCase");

void WeightingTestCase::projSubtendWeightsSumToOne()
{
    std::vector<double> weights;
    projSubtendWeights(RESOLUTION, weights);
    double total = 0.0;
    for (int i = 0, n = weights.size(); i < n; ++i) {
        total += weights[i];
    }
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, total, 1.0e-5);
}

void WeightingTestCase::calcSubtendWeightsSumToOne()
{
    std::vector<double> weights;
    calcSubtendWeights(RESOLUTION, weights);
    double total = 0.0;
    for (int i = 0, n = weights.size(); i < n; ++i) {
        total += weights[i];
    }
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, total, 1.0e-5);
}

void WeightingTestCase::weightsMatch()
{
    std::vector<double> ws, ws2;
    projSubtendWeights(RESOLUTION, ws);
    calcSubtendWeights(RESOLUTION, ws2);
    for (int i = 0, n = ws.size(); i < n; ++i) {
        double error = fabs(1 - ws[i] / ws2[i]);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, error, 1.0 / RESOLUTION);
    }
}
