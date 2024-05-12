#include "TestBeamProfile.h"

#include "BeamProfile.h"
#include "QCPCurveUtils.h"
#include "Calc.h"

void TestBeamProfile::initTestCase() {}

void TestBeamProfile::cleanupTestCase() {}

void TestBeamProfile::init() {}

void TestBeamProfile::cleanup() {}

void TestBeamProfile::Comparisons() {
    std::vector<MeasurementPoint> no_points;
    MeasurementCurrents c(QDateTime::currentDateTime(), 1.1, 2.2, 3.3, 4.4, 5.5);
    QVERIFY(BeamProfile(QA_HARDWARE::UNK,Axis::X, no_points, QDateTime(QDate(2017, 1, 1)), c, 3, 0.0) ==
            BeamProfile(QA_HARDWARE::UNK, Axis::X, no_points, QDateTime(QDate(2017, 1, 1)), c, 3, 0.0));
    QVERIFY(BeamProfile(QA_HARDWARE::UNK, Axis::X, no_points, QDateTime(QDate(2017, 1, 1)), c, 3, 0.0) !=
            BeamProfile(QA_HARDWARE::UNK, Axis::Y, no_points, QDateTime(QDate(2017, 1, 1)), c, 3, 0.0));
    QVERIFY(BeamProfile(QA_HARDWARE::UNK, Axis::X, no_points, QDateTime(QDate(2017, 1, 1)), c, 3, 0.0) !=
            BeamProfile(QA_HARDWARE::UNK, Axis::X, no_points, QDateTime(QDate(2017, 1, 1)), c, 4, 0.0));
}

void TestBeamProfile::Centre() {
    BeamProfile profile1(QA_HARDWARE::UNK, Axis::X);
    profile1.SetSmoothingElements(0);
    profile1 << MeasurementPoint(Point(Axis::X, -3.0), BeamSignal(1.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -2.0), BeamSignal(2.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -1.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 0.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 1.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 2.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 3.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 4.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 5.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 6.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 7.0), BeamSignal(2.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 8.0), BeamSignal(1.0, 1.0));
    double centre1 = profile1.Centre();
    QVERIFY(calc::AlmostEqual(centre1, 2.0, 0.0001));

    BeamProfile profile2(QA_HARDWARE::UNK, Axis::X);
    profile2.SetSmoothingElements(0);
    profile2 << MeasurementPoint(Point(Axis::X, 0.0), BeamSignal(0.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 1.0), BeamSignal(1.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 2.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 3.0), BeamSignal(3.1, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 4.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 5.0), BeamSignal(3.3, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 6.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 7.0), BeamSignal(0.0, 1.0));
    double centre2 = profile2.Centre();
    QVERIFY(calc::AlmostEqual(centre2, 4.0, 0.0001));

    BeamProfile profile3(QA_HARDWARE::UNK, Axis::X);
    profile3.SetSmoothingElements(0);
    profile3 << MeasurementPoint(Point(Axis::X, 0.0), BeamSignal(2.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 1.0), BeamSignal(0.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 2.0), BeamSignal(2.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 3.0), BeamSignal(2.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 4.0), BeamSignal(3.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 5.0), BeamSignal(3.0, 1.0));
    try {
        profile3.Centre();
        QFAIL("TestBeamProfile::Centre Should have thrown1");
    }
    catch (std::exception& exc) {}

    BeamProfile profile4(QA_HARDWARE::UNK, Axis::X);
    try {
        profile4.Centre();
        QFAIL("TestBeamProfile::Centre Should have thrown2");
    }
    catch (std::exception& exc) {}
}

void TestBeamProfile::Width() {
    BeamProfile profile1(QA_HARDWARE::UNK, Axis::X);
    profile1.SetSmoothingElements(0);
    profile1 << MeasurementPoint(Point(Axis::X, -4.0), BeamSignal(0.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -3.0), BeamSignal(1.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -2.0), BeamSignal(2.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -1.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 0.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 1.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 2.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 3.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 4.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 5.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 6.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 7.0), BeamSignal(2.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 8.0), BeamSignal(1.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 9.0), BeamSignal(0.0, 1.0));

    double width1_50 = profile1.Width(0.5);
    QVERIFY(calc::AlmostEqual(width1_50, 10.0, 0.0001));
    double width1_90 = profile1.Width(0.90);
    QVERIFY(calc::AlmostEqual(width1_90, 7.6, 0.0001));
    double width1_95 = profile1.Width(0.95);
    QVERIFY(calc::AlmostEqual(width1_95, 7.3, 0.0001));

    // width should be the same if using the same profile as
    // previous test but displaced on the x-axis
    BeamProfile profile2(QA_HARDWARE::UNK, Axis::X);
    profile2.SetSmoothingElements(0);
    profile2 << MeasurementPoint(Point(Axis::X, 8.0), BeamSignal(0.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 9.0), BeamSignal(1.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 10.0), BeamSignal(2.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 11.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 12.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 13.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 14.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 15.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 16.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 17.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 18.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 19.0), BeamSignal(2.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 20.0), BeamSignal(1.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 21.0), BeamSignal(0.0, 1.0));

    double width2 = profile2.Width(0.5);
    QVERIFY(calc::AlmostEqual(width2, 10.0, 0.0001));

    // a bump not in the center of the profile should
    // not change the calculated width
    BeamProfile profile3(QA_HARDWARE::UNK, Axis::X);
    profile3.SetSmoothingElements(0);
    profile3 << MeasurementPoint(Point(Axis::X, 8.0), BeamSignal(0.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 9.0), BeamSignal(1.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 10.0), BeamSignal(2.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 11.0), BeamSignal(3.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 12.0), BeamSignal(3.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 13.0), BeamSignal(3.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 14.0), BeamSignal(3.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 15.0), BeamSignal(3.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 16.0), BeamSignal(4.0, 1.0)); // not the center point
    profile3 << MeasurementPoint(Point(Axis::X, 17.0), BeamSignal(3.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 18.0), BeamSignal(3.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 19.0), BeamSignal(2.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 20.0), BeamSignal(1.0, 1.0));
    profile3 << MeasurementPoint(Point(Axis::X, 21.0), BeamSignal(0.0, 1.0));

    double width3 = profile3.Width(0.5);
    QVERIFY(calc::AlmostEqual(width3, 10.0, 0.0001));
}

void TestBeamProfile::Penumbra() {
    BeamProfile profile1(QA_HARDWARE::UNK, Axis::X);
    profile1.SetSmoothingElements(0);
    profile1 << MeasurementPoint(Point(Axis::X, -6.0), BeamSignal(0.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -5.0), BeamSignal(1.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -4.0), BeamSignal(2.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -3.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -2.0), BeamSignal(4.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, -1.0), BeamSignal(10.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 0.0), BeamSignal(10.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 1.0), BeamSignal(10.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 2.0), BeamSignal(10.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 3.0), BeamSignal(9.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 4.0), BeamSignal(8.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 5.0), BeamSignal(7.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 6.0), BeamSignal(6.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 7.0), BeamSignal(5.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 8.0), BeamSignal(4.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 9.0), BeamSignal(3.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 10.0), BeamSignal(2.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 11.0), BeamSignal(1.0, 1.0));
    profile1 << MeasurementPoint(Point(Axis::X, 12.0), BeamSignal(0.0, 1.0));


    double penumbra1 = profile1.Penumbra();
    QVERIFY(calc::AlmostEqual(penumbra1, 8.0, 0.0001));

    // steeper
    BeamProfile profile2(QA_HARDWARE::UNK, Axis::X);
    profile2.SetSmoothingElements(0);
    profile2 << MeasurementPoint(Point(Axis::X, -6.0), BeamSignal(0.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, -5.0), BeamSignal(1.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, -4.0), BeamSignal(2.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, -3.0), BeamSignal(3.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, -2.0), BeamSignal(4.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, -1.0), BeamSignal(10.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 0.0), BeamSignal(10.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 1.0), BeamSignal(10.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 2.0), BeamSignal(9.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 3.0), BeamSignal(1.0, 1.0));
    profile2 << MeasurementPoint(Point(Axis::X, 4.0), BeamSignal(0.0, 1.0));

    double penumbra2 = profile2.Penumbra();
    QVERIFY(calc::AlmostEqual(penumbra2, 1.0, 0.0001));
}

void TestBeamProfile::ResultsValid() {
    BeamProfile profile(QA_HARDWARE::UNK, Axis::X);
    QVERIFY(!profile.ResultsValid());

    profile.SetSmoothingElements(0);
    profile << MeasurementPoint(Point(Axis::X, -6.0), BeamSignal(0.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, -5.0), BeamSignal(1.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, -4.0), BeamSignal(2.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, -3.0), BeamSignal(3.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, -2.0), BeamSignal(4.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, -1.0), BeamSignal(5.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 0.0), BeamSignal(5.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 1.0), BeamSignal(5.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 2.0), BeamSignal(5.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 3.5), BeamSignal(5.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 4.0), BeamSignal(4.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 4.5), BeamSignal(3.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 5.0), BeamSignal(2.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 5.5), BeamSignal(1.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 6.0), BeamSignal(0.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 7.0), BeamSignal(0.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, 8.0), BeamSignal(0.0, 1.0));
    QVERIFY(profile.ResultsValid());

}

void TestBeamProfile::ScaleIntensity() {
    BeamProfile profile(QA_HARDWARE::UNK, Axis::X);
    profile.SetSmoothingElements(0);
    profile << MeasurementPoint(Point(Axis::X, -6.0), BeamSignal(0.0, 1.0));
    profile << MeasurementPoint(Point(Axis::X, -5.0), BeamSignal(10.0, 2.0));
    profile << MeasurementPoint(Point(Axis::X, -4.0), BeamSignal(20.0, 1.0));

    BeamProfile expected(QA_HARDWARE::UNK, Axis::X);
    expected.SetSmoothingElements(0);
    expected << MeasurementPoint(Point(Axis::X, -6.0), BeamSignal(0.0, 1.0));
    expected << MeasurementPoint(Point(Axis::X, -5.0), BeamSignal(50.0, 2.0));
    expected << MeasurementPoint(Point(Axis::X, -4.0), BeamSignal(100.0, 1.0));

    BeamProfile scaled = profile.ScaleIntensity(100);
    QCOMPARE(scaled, expected);
}

void TestBeamProfile::Translate() {
    BeamProfile profile(QA_HARDWARE::UNK, Axis::Z);
    profile.SetSmoothingElements(0);
    profile << MeasurementPoint(Point(Axis::Z, -6.0), BeamSignal(0.0, 1.0));
    profile << MeasurementPoint(Point(Axis::Z, -5.0), BeamSignal(10.0, 2.0));
    profile << MeasurementPoint(Point(Axis::Z, -4.0), BeamSignal(20.0, 1.0));

    BeamProfile expected(QA_HARDWARE::UNK, Axis::Z);
    expected.SetSmoothingElements(0);
    expected << MeasurementPoint(Point(Axis::Z, 4.0), BeamSignal(0.0, 1.0));
    expected << MeasurementPoint(Point(Axis::Z, 5.0), BeamSignal(10.0, 2.0));
    expected << MeasurementPoint(Point(Axis::Z, 6.0), BeamSignal(20.0, 1.0));

    QVERIFY(profile != expected);
    profile.Translate(10.0);
    QCOMPARE(profile, expected);
}

