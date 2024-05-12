#include "TestBraggPeak.h"

#include "BraggPeak.h"
#include "QCPCurveUtils.h"
#include "Calc.h"

void TestBraggPeak::initTestCase() {}

void TestBraggPeak::cleanupTestCase() {}

void TestBraggPeak::init() {}

void TestBraggPeak::cleanup() {}

BraggPeak TestBraggPeak::GetStandardBraggPeak() {
    std::vector<MeasurementPoint> p;
    p.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 2), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 3), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 4), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 5), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 7), BeamSignal(5, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 8), BeamSignal(6, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 9), BeamSignal(7, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 10), BeamSignal(8, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 11), BeamSignal(9, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 12), BeamSignal(10, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 13), BeamSignal(8, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 14), BeamSignal(6, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 15), BeamSignal(4, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 16), BeamSignal(2, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 17), BeamSignal(0, 1)));
    DepthDoseMeasurement d(QA_HARDWARE::UNK, p, 0.0);
    return BraggPeak(d, 50.0, 1.2, 30.7);
}

void TestBraggPeak::Getters() {
    auto bp = GetStandardBraggPeak();
    QVERIFY(calc::AlmostEqual(bp.parcours(), 30.7, 0.0001));
    QVERIFY(calc::AlmostEqual(bp.penumbra(), 1.2, 0.0001));
    QVERIFY(calc::AlmostEqual(bp.width50(), 50.0, 0.0001));
}

void TestBraggPeak::Comparison() {
    auto bp1 = GetStandardBraggPeak();
    QCOMPARE(bp1, bp1);

    std::vector<MeasurementPoint> p;
    p.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(0, 1)));
    DepthDoseMeasurement d(QA_HARDWARE::UNK, p, 0.0);
    BraggPeak bp2(d, 50.0, 1.2, 30.7);
    QVERIFY(bp1 != bp2);

    BraggPeak bp3(d, 50.1, 1.2, 30.7);
    QVERIFY(bp2 != bp3);

    BraggPeak bp4(d, 50.0, 1.3, 30.7);
    QVERIFY(bp2 != bp4);

    BraggPeak bp5(d, 50.0, 1.2, 30.8);
    QVERIFY(bp2 != bp5);
}
