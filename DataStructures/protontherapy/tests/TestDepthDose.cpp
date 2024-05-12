#include "TestDepthDose.h"

#include "DepthDose.h"
#include "Calc.h"

void TestDepthDose::initTestCase() {}

void TestDepthDose::cleanupTestCase() {}

void TestDepthDose::init() {}

void TestDepthDose::cleanup() {}

void TestDepthDose::Constructor() {
    const double dose(1.0);
    const double depth(3.0);
    DepthDose d(depth, dose);
    QVERIFY(calc::AlmostEqual(d.dose(), dose, 0.0001));
    QVERIFY(calc::AlmostEqual(d.depth(), depth, 0.0001));

    try {
        DepthDose(-1.0, 2.2);
        QFAIL("TestDepthDose::Constructor Negative depth should have thrown an exception");
    }
    catch (...) {}
}

void TestDepthDose::Comparison() {
    const DepthDose d1(1.1, 3.3);
    const DepthDose d2(1.2, 2.2);
    QCOMPARE(d1, d1);
    QVERIFY(!(d1 == d2));
}

void TestDepthDose::Addition() {
    DepthDose d1(3.0, 1.0);
    DepthDose d2(3.0, 10.0);
    DepthDose sum1(d1 + d2);
    QVERIFY(calc::AlmostEqual(sum1.dose(), 11.0, 0.0001));
    QVERIFY(calc::AlmostEqual(sum1.depth(), 3.0, 0.0001));

    try {
        DepthDose d3(d1 + DepthDose(4.0, 10.0));
        QFAIL("TestDepthDose::Addition Summing DepthDoses with different depth should throw an exception");
    }
    catch (...) {}
}

void TestDepthDose::Negation() {
    DepthDose d1(3.0, 1.0);
    DepthDose d2(3.0, -1.0);
    QCOMPARE(d1, -d2);
}

void TestDepthDose::Subtraction() {
    DepthDose d1(3.0, 1.0);
    DepthDose d2(3.0, 10.0);
    DepthDose sum1(d2 - d1);
    QVERIFY(calc::AlmostEqual(sum1.dose(), 9.0, 0.0001));
    QVERIFY(calc::AlmostEqual(sum1.depth(), 3.0, 0.0001));
}

void TestDepthDose::Multiplication() {
    DepthDose d(3.0, 4.0);

    DepthDose factor1(2.0 * d);
    QVERIFY(calc::AlmostEqual(factor1.dose(), 8.0, 0.0001));
    QVERIFY(calc::AlmostEqual(factor1.depth(), 3.0, 0.0001));

    DepthDose factor2(d * 2.0);
    QVERIFY(calc::AlmostEqual(factor2.dose(), 8.0, 0.0001));
    QVERIFY(calc::AlmostEqual(factor2.depth(), 3.0, 0.0001));
}

