#include "TestCuveSlice.h"

#include "Calc.h"

void TestCuveSlice::initTestCase() {}

void TestCuveSlice::cleanupTestCase() {}

void TestCuveSlice::init() {}

void TestCuveSlice::cleanup() {}

void TestCuveSlice::Comparisons() {
    CuveSlice s1;
    QCOMPARE(s1, s1);

    CuveSlice s2;
    s2.Add(1.0, 2.0, Signal(3.0, 1.0, 0.0));
    QVERIFY(s1 != s2);
    QVERIFY(s2 == s2);

    CuveSlice s3;
    s3.Add(1.0, 2.0, Signal(3.0, 1.0, 0.0));
    s3.Add(2.0, 3.0, Signal(4.0, 1.0, 0.0));
    QVERIFY(s3 != s2);
    QVERIFY(s3 == s3);
}

void TestCuveSlice::IntegratedSignal() {
    double ref(1.0);
    double noise(0.0);

    CuveSlice s3;
    s3.Add(1.0, 2.0, Signal(1.0, ref));
    s3.Add(2.0, 2.0, Signal(2.0, ref));
    s3.Add(3.0, 2.0, Signal(3.0, ref));
    s3.Add(4.0, 2.0, Signal(4.0, ref));

    s3.Add(1.0, 3.0, Signal(10.0, ref));
    s3.Add(2.0, 3.0, Signal(20.0, ref));
    s3.Add(3.0, 3.0, Signal(30.0, ref));
    s3.Add(4.0, 3.0, Signal(40.0, ref));

    s3.Add(1.0, 4.0, Signal(100.0, ref));
    s3.Add(2.0, 4.0, Signal(200.0, ref));
    s3.Add(3.0, 4.0, Signal(300.0, ref));
    s3.Add(4.0, 4.0, Signal(400.0, ref));

    double i1 = s3.IntegratedSignal(Range(2.0, 3.0), Range(3.0, 4.0), noise);
    QVERIFY(calc::AlmostEqual(i1, 550.0, 0.001));

    double i2 = s3.IntegratedSignal(Range(1.0, 4.0), Range(2.0, 4.0), noise);
    QVERIFY(calc::AlmostEqual(i2, 1110.0, 0.001));

    double i3 = s3.IntegratedSignal(Range(0.5, 1.5), Range(1.9, 2.1), noise);
    QVERIFY(calc::AlmostEqual(i3, 1.0, 0.001));

    double i4 = s3.IntegratedSignal(Range(0.1, 0.2), Range(1.9, 2.1), noise);
    QVERIFY(calc::AlmostEqual(i4, 0.0, 0.001));
}

void TestCuveSlice::AverageSignal() {
    double ref(1.0);
    double noise(0.0);

    CuveSlice s3;
    s3.Add(1.0, 2.0, Signal(1.0, ref));
    s3.Add(2.0, 2.0, Signal(2.0, ref));
    s3.Add(3.0, 2.0, Signal(3.0, ref));
    s3.Add(4.0, 2.0, Signal(4.0, ref));

    s3.Add(1.0, 3.0, Signal(10.0, ref));
    s3.Add(2.0, 3.0, Signal(20.0, ref));
    s3.Add(3.0, 3.0, Signal(30.0, ref));
    s3.Add(4.0, 3.0, Signal(40.0, ref));

    s3.Add(1.0, 4.0, Signal(100.0, ref));
    s3.Add(2.0, 4.0, Signal(200.0, ref));
    s3.Add(3.0, 4.0, Signal(300.0, ref));
    s3.Add(4.0, 4.0, Signal(400.0, ref));

    double i1 = s3.AverageSignal(Range(2.0, 3.0), Range(3.0, 4.0), noise);
    QVERIFY(calc::AlmostEqual(i1, 550.0 / 4.0, 0.001));

    double i2 = s3.AverageSignal(Range(1.0, 4.0), Range(2.0, 4.0), noise);
    QVERIFY(calc::AlmostEqual(i2, 1110.0 / 12.0, 0.001));

    double i3 = s3.AverageSignal(Range(0.5, 1.5), Range(1.9, 2.1), noise);
    QVERIFY(calc::AlmostEqual(i3, 1.0, 0.001));

    double i4 = s3.AverageSignal(Range(0.1, 0.2), Range(1.9, 2.1), noise);
    QVERIFY(calc::AlmostEqual(i4, 0.0, 0.001));
}

void TestCuveSlice::GetColorMapData() {

}


