#include "TestEye.h"

#include "Calc.h"

void TestEye::Constructor() {
    try {
        Eye(0.0, 1.0);
        QFAIL("TestEye::Constructor Exception should have been thrown, d <= 0");
    }
    catch (...) {}

    try {
        Eye(3.0, 2.0);
        QFAIL("TestEye::Constructor Exception should have been thrown, d <= 2 * sclere");
    }
    catch (...) {}
}

void TestEye::SclereCurve() {
    bool found;
    const double diam_eye(4.0);
    const double thick_sclere(1.0);
    // -> diam sclere = 2.0
    Eye eye(diam_eye, thick_sclere);

    // COUNTER CLOCKWISE

    // full circle
    QCPCurveDataContainer curve1(eye.SclereCurve(0.0, 2.0 * M_PI));
    QCOMPARE(curve1.keyRange(found) , QCPRange(-1.0, 1.0));
    QCOMPARE(curve1.valueRange(found) , QCPRange(-1.0, 1.0));

    // half circle
    QCPCurveDataContainer curve2(eye.SclereCurve(0.0, M_PI));
    QVERIFY(calc::AlmostEqual(curve2.keyRange(found).upper , 1.0, 0.001));
    QVERIFY(calc::AlmostEqual(curve2.keyRange(found).lower , -1.0, 0.001));
    QVERIFY(calc::AlmostEqual(curve2.valueRange(found).upper , 1.0, 0.001));
    QVERIFY(std::fabs(curve2.valueRange(found).lower) < 0.01);

    // quarter circle
    QCPCurveDataContainer curve3(eye.SclereCurve(0.0, M_PI / 2.0));
    QVERIFY(std::abs(curve3.keyRange(found).lower) < 0.01);
    QVERIFY(calc::AlmostEqual(curve3.keyRange(found).upper , 1.0, 0.0001));
    QVERIFY(std::abs(curve3.valueRange(found).lower) < 0.01);
    QVERIFY(calc::AlmostEqual(curve3.valueRange(found).upper , 1.0, 0.0001));

    // CLOCKWISE

    // full circle
    QCPCurveDataContainer curve4(eye.SclereCurve(2.0 * M_PI, 0.0));
    QVERIFY(calc::AlmostEqual(curve4.keyRange(found).upper , 1.0, 0.001));
    QVERIFY(calc::AlmostEqual(curve4.keyRange(found).lower , -1.0, 0.001));
    QVERIFY(calc::AlmostEqual(curve4.valueRange(found).upper , 1.0, 0.001));
    QVERIFY(calc::AlmostEqual(curve4.valueRange(found).lower , -1.0, 0.001));

    // half circle
    QCPCurveDataContainer curve5(eye.SclereCurve(M_PI, 0.0));
    QVERIFY(calc::AlmostEqual(curve5.keyRange(found).upper , 1.0, 0.001));
    QVERIFY(calc::AlmostEqual(curve5.keyRange(found).lower , -1.0, 0.001));
    QVERIFY(calc::AlmostEqual(curve5.valueRange(found).upper, 1.0, 0.001));
    QVERIFY(std::abs(curve5.valueRange(found).lower) < 0.01);

    // quarter circle
    QCPCurveDataContainer curve6(eye.SclereCurve(M_PI / 2.0, 0.0));
    QVERIFY(std::abs(curve6.keyRange(found).lower) < 0.01);
    QVERIFY(calc::AlmostEqual(curve6.keyRange(found).upper , 1.0, 0.0001));
    QVERIFY(std::abs(curve6.valueRange(found).lower) < 0.01);
    QVERIFY(calc::AlmostEqual(curve6.valueRange(found).upper , 1.0, 0.0001));
}

void TestEye::SclereRadius() {
    QCOMPARE(Eye(4.0, 1.0).sclere_radius(), 1.0);
    QCOMPARE(Eye(6.0, 1.0).sclere_radius(), 2.0);
}

