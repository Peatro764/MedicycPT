#include "TestMeasurementPoint.h"

#include "MeasurementPoint.h"
#include "Calc.h"
#include "QCPCurveUtils.h"

void TestMeasurementPoint::initTestCase() {}

void TestMeasurementPoint::cleanupTestCase() {}

void TestMeasurementPoint::init() {}

void TestMeasurementPoint::cleanup() {}

void TestMeasurementPoint::Constructor() {
    MeasurementPoint p(Point(1, 2, 3), BeamSignal(4, 2));
    QCOMPARE(p.point(), Point(1, 2, 3));
    QCOMPARE(p.signal(), BeamSignal(4, 2));
}

void TestMeasurementPoint::ScaleAxis() {
    MeasurementPoint p1(Point(3, 1, 2), BeamSignal(10, 2));
    MeasurementPoint p1ExpScaled(Point(9, 1, 2), BeamSignal(10, 2));
    MeasurementPoint p1Scaled = p1.ScaleAxis(Axis::X, 3);
    QCOMPARE(p1Scaled, p1ExpScaled);
    QVERIFY(p1 != p1Scaled);
}

void TestMeasurementPoint::ScaleIntensity() {
    MeasurementPoint p1(Point(3, 1, 2), BeamSignal(10, 2));
    MeasurementPoint p1ExpScaled(Point(3, 1, 2), BeamSignal(82, 2));
    MeasurementPoint p1Scaled = p1.ScaleIntensity(2, 10);
    QCOMPARE(p1Scaled, p1ExpScaled);
    QCOMPARE(p1Scaled.intensity(2), 10.0*p1.intensity(2));
    QVERIFY(p1 != p1Scaled);

    MeasurementPoint p2ExpScaled(Point(3, 1, 2), BeamSignal(118, 2));
    MeasurementPoint p2Scaled = p1.ScaleIntensity(-2, 10);
    QCOMPARE(p2Scaled, p2ExpScaled);
    QCOMPARE(p2Scaled.intensity(-2), 10.0*p1.intensity(-2));
}

void TestMeasurementPoint::Translate() {
    MeasurementPoint p1(Point(3, 1, 2), BeamSignal(10, 2));
    MeasurementPoint p1ExpTranslated(Point(3, 10, 2), BeamSignal(10, 2));
    MeasurementPoint p1Translated = p1.TranslateAxis(Axis::Y, 9);
    QCOMPARE(p1Translated, p1ExpTranslated);
    QVERIFY(p1 != p1Translated);
}
