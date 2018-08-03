////////////////////////////////////////////////////////////////////////
//
// geom_test.cpp: Tests for geom.cpp.
//
// Copyright (c) Simon Frankau 2018
//

#include <cmath>
#include <sstream>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "geom.h"

class GeomTestCase : public CppUnit::TestCase
{
private:
    CPPUNIT_TEST_SUITE(GeomTestCase);
    // Vertex cases
    CPPUNIT_TEST(copyVertex);
    CPPUNIT_TEST(getVertexComponents);
    CPPUNIT_TEST(getVertexLength);
    CPPUNIT_TEST(getVertexNorm);
    CPPUNIT_TEST(vertexSubtraction);
    CPPUNIT_TEST(printVertex);
    CPPUNIT_TEST(selfDotVertex);
    CPPUNIT_TEST(orthDotVertex);
    CPPUNIT_TEST(otherDotVertex);
    CPPUNIT_TEST(crossVertex);
    CPPUNIT_TEST(lerpVertex);
    CPPUNIT_TEST_SUITE_END();

    // Vertex cases
    void copyVertex();
    void getVertexComponents();
    void getVertexLength();
    void getVertexNorm();
    void vertexSubtraction();
    void printVertex();
    void selfDotVertex();
    void orthDotVertex();
    void otherDotVertex();
    void crossVertex();
    void lerpVertex();

    // Helpers
    void vecEquals(Vertex const &v1, Vertex const &v2);
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(GeomTestCase, "GeomTestCase");

void GeomTestCase::copyVertex()
{
    Vertex v1(-1.0, 2.0, 3.0);
    Vertex v2(v1);

    CPPUNIT_ASSERT(v1.x() == v2.x());
    CPPUNIT_ASSERT(v1.y() == v2.y());
    CPPUNIT_ASSERT(v1.z() == v2.z());
}


void GeomTestCase::getVertexComponents()
{
    Vertex v(-1.0, 2.0, 3.0);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, v.x(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+2.0, v.y(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+3.0, v.z(), 1e-9);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, v.p[0], 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+2.0, v.p[1], 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+3.0, v.p[2], 1e-9);
}


void GeomTestCase::getVertexLength()
{
    Vertex v(-1.0, 2.0, 3.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(14.0), v.len(), 1e-9);
}

void GeomTestCase::getVertexNorm()
{
    Vertex v1(-1.0, 2.0, 3.0);
    Vertex v2(v1.norm());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, v2.len(), 1e-9);

    double rx = v1.x() / v2.x();
    double ry = v1.x() / v2.x();
    double rz = v1.x() / v2.x();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(rx, ry, 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(rx, rz, 1e-9);
}

void GeomTestCase::vertexSubtraction()
{
    Vertex v1(-1.0, 2.0,  3.0);
    Vertex v2(-4.0, 5.0, -6.0);
    Vertex v3(v1 - v2);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(+3.0, v3.x(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.0, v3.y(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+9.0, v3.z(), 1e-9);
}

void GeomTestCase::printVertex()
{
    Vertex v(-1.0, 2.0, 3.0);
    std::ostringstream oss;
    oss << v;
    CPPUNIT_ASSERT_EQUAL(std::string("(-1, 2, 3)"), oss.str());
}

void GeomTestCase::selfDotVertex()
{
    Vertex vs[] = {
        Vertex(-1.0, 2.0,  3.0),
        Vertex(-4.0, 5.0, -6.0)
    };
    for (int i = 0; i < sizeof(vs) / sizeof(vs[0]); ++i) {
        double d = dot(vs[i], vs[i]);
        double l = vs[i].len();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(d, l * l, 1e-9);
    }
}

void GeomTestCase::orthDotVertex()
{
    Vertex v1(-1.0, 1.0, 1.0);
    Vertex v2( 1.0, 1.0, 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dot(v1, v2), 1e-9);
}

void GeomTestCase::otherDotVertex()
{
    Vertex v1(-1.0, 1.0, 1.0);
    Vertex v2(-1.0, 0.0, 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, dot(v1, v2), 1e-9);
}

void GeomTestCase::crossVertex()
{
    Vertex vs[] = {
        Vertex(2.0,  0.0, 0.0),
        Vertex(0.0, -2.0, 0.0),
        Vertex(0.0,  0.0, 2.0)
    };

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Vertex v = cross(vs[i], vs[j]);
            if (i == j) {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, v.len(), 1e-9);
            } else {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, v.len(), 1e-9);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dot(v, vs[i]), 1e-9);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dot(v, vs[j]), 1e-9);
            }
        }
    }
}

void GeomTestCase::lerpVertex()
{
    Vertex v000(1.0, 1.0,  1.0);
    Vertex v100(5.0, 9.0, 13.0);
    Vertex v025(2.0, 3.0,  4.0);

    vecEquals(v000, lerp(v000, v100, 0.00));
    vecEquals(v100, lerp(v000, v100, 1.00));
    vecEquals(v025, lerp(v000, v100, 0.25));
}

void GeomTestCase::vecEquals(Vertex const &v1, Vertex const &v2)
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, (v1 - v2).len(), 1e-9);
}

// TODO: test cases for quad and basic shapes.
