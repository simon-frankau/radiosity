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
    CPPUNIT_TEST(testVertexCopy);
    CPPUNIT_TEST(testVertexComponents);
    CPPUNIT_TEST(testVertexLength);
    CPPUNIT_TEST(testVertexNorm);
    CPPUNIT_TEST(testVertexPerp);
    CPPUNIT_TEST(testVertexScaling);
    CPPUNIT_TEST(testVertexAddition);
    CPPUNIT_TEST(testVertexSubtraction);
    CPPUNIT_TEST(testVertexPrint);
    CPPUNIT_TEST(testVertexSelfDot);
    CPPUNIT_TEST(testVertexOrthogonalDot);
    CPPUNIT_TEST(testVertexOtherDot);
    CPPUNIT_TEST(testVertexOrthog);
    CPPUNIT_TEST(testVertexCross);
    CPPUNIT_TEST(testVertexLerp);
    // Quad cases
    CPPUNIT_TEST(testQuadConstruct);
    CPPUNIT_TEST(testParaCentre);
    CPPUNIT_TEST(testParaCross);
    CPPUNIT_TEST(testParaArea);
    CPPUNIT_TEST(testQuadTrivialSubdivision);
    CPPUNIT_TEST(testQuadSubdivision);
    CPPUNIT_TEST(testScale);
    CPPUNIT_TEST(testRotation);
    CPPUNIT_TEST(testTranslation);
    CPPUNIT_TEST(testFlip);
    // Cube case
    CPPUNIT_TEST(testCubeProperties);
    CPPUNIT_TEST_SUITE_END();

    // Vertex cases
    void testVertexCopy();
    void testVertexComponents();
    void testVertexLength();
    void testVertexNorm();
    void testVertexPerp();
    void testVertexScaling();
    void testVertexAddition();
    void testVertexSubtraction();
    void testVertexPrint();
    void testVertexSelfDot();
    void testVertexOrthogonalDot();
    void testVertexOtherDot();
    void testVertexOrthog();
    void testVertexCross();
    void testVertexLerp();
    // Quad cases
    void testQuadConstruct();
    void testParaCentre();
    void testParaCross();
    void testParaArea();
    void testQuadTrivialSubdivision();
    void testQuadSubdivision();
    void testScale();
    void testRotation();
    void testTranslation();
    void testFlip();
    // Cube case
    void testCubeProperties();
    // Helpers
    void assertVectorsEqual(Vertex const &v1, Vertex const &v2);
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(GeomTestCase, "GeomTestCase");

////////////////////////////////////////////////////////////////////////
// Vertex test cases

void GeomTestCase::testVertexCopy()
{
    Vertex v1(-1.0, 2.0, 3.0);
    Vertex v2(v1);

    CPPUNIT_ASSERT(v1.x() == v2.x());
    CPPUNIT_ASSERT(v1.y() == v2.y());
    CPPUNIT_ASSERT(v1.z() == v2.z());
}

void GeomTestCase::testVertexComponents()
{
    Vertex v(-1.0, 2.0, 3.0);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, v.x(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+2.0, v.y(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+3.0, v.z(), 1e-9);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, v.p[0], 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+2.0, v.p[1], 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+3.0, v.p[2], 1e-9);
}

void GeomTestCase::testVertexLength()
{
    Vertex v(-1.0, 2.0, 3.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(14.0), v.len(), 1e-9);
}

void GeomTestCase::testVertexNorm()
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

void GeomTestCase::testVertexPerp()
{
    Vertex v1(-1.0, 2.0, 3.0);
    Vertex v2(v1.perp());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dot(v1, v2), 1e-9);
    CPPUNIT_ASSERT(v2.len() > 1.0e-3);
}

void GeomTestCase::testVertexScaling()
{
    Vertex v1(1.0, -2.0, 3.0);
    Vertex v2(v1.scale(-0.5));
    Vertex v3(-0.5, 1.0, -1.5);

    assertVectorsEqual(v2, v3);
}

void GeomTestCase::testVertexAddition()
{
    Vertex v1(-1.0, 2.0,  3.0);
    Vertex v2(-4.0, 5.0, -6.0);
    Vertex v3(v1 + v2);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-5.0, v3.x(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+7.0, v3.y(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.0, v3.z(), 1e-9);
}

void GeomTestCase::testVertexSubtraction()
{
    Vertex v1(-1.0, 2.0,  3.0);
    Vertex v2(-4.0, 5.0, -6.0);
    Vertex v3(v1 - v2);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(+3.0, v3.x(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.0, v3.y(), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(+9.0, v3.z(), 1e-9);
}

void GeomTestCase::testVertexPrint()
{
    Vertex v(-1.0, 2.0, 3.0);
    std::ostringstream oss;
    oss << v;
    CPPUNIT_ASSERT_EQUAL(std::string("(-1, 2, 3)"), oss.str());
}

void GeomTestCase::testVertexSelfDot()
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

void GeomTestCase::testVertexOrthogonalDot()
{
    Vertex v1(-1.0, 1.0, 1.0);
    Vertex v2( 1.0, 1.0, 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dot(v1, v2), 1e-9);
}

void GeomTestCase::testVertexOtherDot()
{
    Vertex v1(-1.0, 1.0, 1.0);
    Vertex v2(-1.0, 0.0, 0.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, dot(v1, v2), 1e-9);
}

void GeomTestCase::testVertexOrthog()
{
    Vertex v1( 5.0, -3.0, 42.0);
    Vertex v2(-5.0, -3.0,  7.0);
    Vertex v3(orthog(v1, v2));

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, dot(v2, v3), 1e-9);
    assertVectorsEqual(cross(v1, v2), cross(v3, v2));
}

void GeomTestCase::testVertexCross()
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

void GeomTestCase::testVertexLerp()
{
    Vertex v000(1.0, 1.0,  1.0);
    Vertex v100(5.0, 9.0, 13.0);
    Vertex v025(2.0, 3.0,  4.0);

    assertVectorsEqual(v000, lerp(v000, v100, 0.00));
    assertVectorsEqual(v100, lerp(v000, v100, 1.00));
    assertVectorsEqual(v025, lerp(v000, v100, 0.25));
}

////////////////////////////////////////////////////////////////////////
// Quad test cases.

void GeomTestCase::testQuadConstruct()
{
    Quad q(1, 2, 3, 4, 0.5);
    CPPUNIT_ASSERT_EQUAL(1, q.indices[0]);
    CPPUNIT_ASSERT_EQUAL(2, q.indices[1]);
    CPPUNIT_ASSERT_EQUAL(3, q.indices[2]);
    CPPUNIT_ASSERT_EQUAL(4, q.indices[3]);
    CPPUNIT_ASSERT_EQUAL(false, q.isEmitter);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, q.light,      1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, q.brightness, 1e-9);
}

void GeomTestCase::testParaCentre()
{
    std::vector<Vertex> vs;
    vs.push_back(Vertex(1.0, 0.0, 0.0));
    vs.push_back(Vertex(1.0, 1.0, 0.0));
    vs.push_back(Vertex(2.0, 2.0, 0.0));
    vs.push_back(Vertex(2.0, 1.0, 0.0));
    Quad q(0, 1, 2, 3, 0.5);
    Vertex vc(1.5, 1.0, 0.0);
    assertVectorsEqual(vc, paraCentre(q, vs));
}

void GeomTestCase::testParaCross()
{
    std::vector<Vertex> vs;
    vs.push_back(Vertex(1.0, 0.0, 0.0));
    vs.push_back(Vertex(1.0, 1.0, 0.0));
    vs.push_back(Vertex(2.0, 2.0, 0.0));
    vs.push_back(Vertex(2.0, 1.0, 0.0));
    Quad q(0, 1, 2, 3, 0.5);
    Vertex vc(0.0, 0.0, 1.0);
    assertVectorsEqual(vc, paraCross(q, vs));
}

void GeomTestCase::testParaArea()
{
    std::vector<Vertex> vs;
    vs.push_back(Vertex(1.0, 0.0, 0.0));
    vs.push_back(Vertex(1.0, 1.0, 0.0));
    vs.push_back(Vertex(2.0, 2.0, 0.0));
    vs.push_back(Vertex(2.0, 1.0, 0.0));
    Quad q(0, 1, 2, 3, 0.5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, paraArea(q, vs), 1e-9);
}

void GeomTestCase::testQuadTrivialSubdivision()
{
    std::vector<Vertex> vs;
    std::vector<Quad> qs;

    vs.push_back(Vertex(1.0, 1.0, 1.0));
    vs.push_back(Vertex(1.0, 5.0, 1.0));
    vs.push_back(Vertex(1.0, 5.0, 5.0));
    vs.push_back(Vertex(1.0, 1.0, 5.0));

    Quad q(0, 1, 2, 3, 0.5);
    subdivide(q, vs, qs, 1, 1);

    CPPUNIT_ASSERT_EQUAL(1ul, qs.size());
    Quad &q2 = qs[0];
    for (int i = 0; i < 4; ++i) {
        assertVectorsEqual(vs[q.indices[i]], vs[q2.indices[i]]);
    }
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q.light, q2.light, 1e-9);
}

void GeomTestCase::testQuadSubdivision()
{
    // I'm sure there are funky things we could do. Let's just check
    // we get the right number of quads and area is preserved.
    std::vector<Vertex> vs;
    std::vector<Quad> qs;

    vs.push_back(Vertex(1.0, 1.0, 1.0));
    vs.push_back(Vertex(1.0, 5.0, 1.0));
    vs.push_back(Vertex(1.0, 5.0, 5.0));
    vs.push_back(Vertex(1.0, 1.0, 5.0));

    Quad q(0, 1, 2, 3, 0.7);

    subdivide(q, vs, qs, 10, 20);
    CPPUNIT_ASSERT_EQUAL(10ul * 20ul, qs.size());

    double subdivArea = 0.0;
    for (unsigned long i = 0; i < qs.size(); ++i) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(q.light, qs[i].light, 1e-9);
        subdivArea += paraArea(qs[i], vs);
    }
    CPPUNIT_ASSERT_DOUBLES_EQUAL(paraArea(q, vs), subdivArea, 1e-9);
}

void GeomTestCase::testFlip()
{
    std::vector<Vertex> vs;
    std::vector<Quad> qs;
    vs.push_back(Vertex(1.0, 0.0, 0.0));
    vs.push_back(Vertex(1.0, 1.0, 0.0));
    vs.push_back(Vertex(2.0, 2.0, 0.0));
    vs.push_back(Vertex(2.0, 1.0, 0.0));
    qs.push_back(Quad(0, 1, 2, 3, 0.5));

    std::vector<Quad> qs2(qs);

    flip(qs2, vs);
    assertVectorsEqual(paraCross(qs [0], vs),
              paraCross(qs2[0], vs).scale(-1.0));
}

void GeomTestCase::testScale()
{
    std::vector<Vertex> vertices(cubeVertices);
    std::vector<Quad> faces(cubeFaces);

    scale(0.5, faces, vertices);
    // Right now, only check the appropriate number of faces and
    // vertices are present.
    CPPUNIT_ASSERT_EQUAL(faces.size(), cubeFaces.size());
    CPPUNIT_ASSERT_EQUAL(vertices.size(), 2 * cubeVertices.size());
}

void GeomTestCase::testRotation()
{
    // Three vertices away from axis, plus one on the axis.
    std::vector<Vertex> vs = {
        Vertex(1.0, 0.0, 0.0),
        Vertex(0.0, 1.0, 0.0),
        Vertex(0.0, 0.0, 1.0),
        Vertex(1.0, 1.0, 0.0)
    };
    std::vector<Quad> qs = {
        Quad(0, 1, 2, 3, 1.0)
    };

    rotate(Vertex(1.0, 1.0, 0.0), M_PI / 2.0, qs, vs);
    CPPUNIT_ASSERT_EQUAL(1ul, qs.size());
    Quad const &q = qs[0];
    double rt = sqrt(0.5);
    assertVectorsEqual(Vertex( 0.5, 0.5,  rt), vs[q.indices[0]]);
    assertVectorsEqual(Vertex( 0.5, 0.5, -rt), vs[q.indices[1]]);
    assertVectorsEqual(Vertex( -rt, rt,  0.0), vs[q.indices[2]]);
    assertVectorsEqual(Vertex( 1.0, 1.0, 0.0), vs[q.indices[3]]);
}

void GeomTestCase::testTranslation()
{
    std::vector<Vertex> vs = {
        Vertex(1.0, 0.0, 0.0),
        Vertex(0.0, 1.0, 0.0),
        Vertex(0.0, 0.0, 1.0),
        Vertex(1.0, 1.0, 0.0)
    };
    std::vector<Quad> qs = {
        Quad(0, 1, 2, 3, 1.0)
    };

    translate(Vertex(1.0, 1.0, 0.0), qs, vs);
    CPPUNIT_ASSERT_EQUAL(1ul, qs.size());
    Quad const &q = qs[0];
    assertVectorsEqual(Vertex(2.0, 1.0, 0.0), vs[q.indices[0]]);
    assertVectorsEqual(Vertex(1.0, 2.0, 0.0), vs[q.indices[1]]);
    assertVectorsEqual(Vertex(1.0, 1.0, 1.0), vs[q.indices[2]]);
    assertVectorsEqual(Vertex(2.0, 2.0, 0.0), vs[q.indices[3]]);
}

////////////////////////////////////////////////////////////////////////
// Miscellaneous.

void GeomTestCase::testCubeProperties()
{
    // Check that all faces of the cube face the same way around the
    // origin and have same area.
    for (int i = 0, n = cubeFaces.size(); i < n; ++i) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0,
                                     paraArea(cubeFaces[i], cubeVertices),
                                     1e-9);

        Vertex normal(paraCross(cubeFaces[i], cubeVertices));
        Vertex corner(cubeVertices[cubeFaces[i].indices[0]]);
        CPPUNIT_ASSERT(dot(normal, corner) > 0);
    }
}

void GeomTestCase::assertVectorsEqual(Vertex const &v1, Vertex const &v2)
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, (v1 - v2).len(), 1e-9);
}
