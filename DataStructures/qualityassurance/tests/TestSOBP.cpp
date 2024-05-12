#include "TestSOBP.h"

#include "QCPCurveUtils.h"
#include "Calc.h"

void TestSOBP::initTestCase() {}

void TestSOBP::cleanupTestCase() {}

void TestSOBP::init() {}

void TestSOBP::cleanup() {}

SOBP TestSOBP::GetStandardSOBP() {
    std::vector<MeasurementPoint> p;
    p.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(0, 1)));

    DepthDoseMeasurement d(QA_HARDWARE::UNK, p, 0.0);
    return SOBP(d, 20000, 10000, 2.3, "comment", 1.2, 2.2, 10.0, 11.0);
}

void TestSOBP::Getters() {
    auto sobp = GetStandardSOBP();
    QCOMPARE(sobp.dossier(), 20000);
    QCOMPARE(sobp.modulateur_id(), 10000);
    QCOMPARE(sobp.comment(), QString("comment"));
    QVERIFY(calc::AlmostEqual(sobp.mm_degradeur(), 2.3, 0.0001));
    QVERIFY(calc::AlmostEqual(sobp.penumbra(), 1.2, 0.0001));
    QVERIFY(calc::AlmostEqual(sobp.parcours(), 2.2, 0.0001));
    QVERIFY(calc::AlmostEqual(sobp.mod98(), 10.0, 0.0001));
    QVERIFY(calc::AlmostEqual(sobp.mod100(), 11.0, 0.0001));
}

void TestSOBP::Comparison() {
    auto sobp = GetStandardSOBP();
    QCOMPARE(sobp, sobp);

    std::vector<MeasurementPoint> p;
    p.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(3, 1)));
    DepthDoseMeasurement d(QA_HARDWARE::UNK, p, 0.0);
    SOBP sobp1(d, 20000, 10000, 2.3, "comment", 1.2, 2.2, 10.0, 11.0);
    QVERIFY(sobp != sobp1);
}


