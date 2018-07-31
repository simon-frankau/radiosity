////////////////////////////////////////////////////////////////////////
//
// weighting_test.cpp: Tests for weighting.cpp.
//
// Copyright (c) Simon Frankaus 2018
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
    CPPUNIT_TEST(projWeightsSumToOne);
    CPPUNIT_TEST(calcWeightsSumToOne);
    CPPUNIT_TEST(weightsMatch);
    CPPUNIT_TEST_SUITE_END();

    void projWeightsSumToOne();
    void calcWeightsSumToOne();
    void weightsMatch();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(WeightingTestCase, "WeightingTestCase");

void WeightingTestCase::projWeightsSumToOne()
{
    std::vector<double> weights;
    projWeights(RESOLUTION, weights);
    double total = 0.0;
    for (int i = 0, n = weights.size(); i < n; ++i) {
        total += weights[i];
    }
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, total, 1.0e-5);
}

void WeightingTestCase::calcWeightsSumToOne()
{
    std::vector<double> weights;
    calcWeights(RESOLUTION, weights);
    double total = 0.0;
    for (int i = 0, n = weights.size(); i < n; ++i) {
        total += weights[i];
    }
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, total, 1.0e-5);
}

void WeightingTestCase::weightsMatch()
{
    std::vector<double> ws, ws2;
    projWeights(RESOLUTION, ws);
    calcWeights(RESOLUTION, ws2);
    for (int i = 0, n = ws.size(); i < n; ++i) {
        double error = fabs(1 - ws[i] / ws2[i]);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, error, 1.0 / RESOLUTION);
    }
}
