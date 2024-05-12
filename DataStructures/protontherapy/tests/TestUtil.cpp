#include "TestUtil.h"

#include <iostream>
#include <QDateTime>

#include "Calc.h"
#include "Util.h"
#include "Modulateur.h"
#include "Degradeur.h"

void TestUtil::initTestCase() {
    maxRelDiff_ = 0.001;
}

void TestUtil::cleanupTestCase() {}

void TestUtil::init() {}

void TestUtil::cleanup() {}


void TestUtil::TestInterpolateMonitorUnits() {
    std::map<int, double> map1;
    map1[10] = 1.0;
    map1[20] = 2.0;
    map1[30] = 3.0;
    QVERIFY(calc::AlmostEqual(util::InterpolateMonitorUnits(10, map1), 1.0, 0.0001));
    QVERIFY(calc::AlmostEqual(util::InterpolateMonitorUnits(15, map1), 1.5, 0.0001));
    QVERIFY(calc::AlmostEqual(util::InterpolateMonitorUnits(20, map1), 2.0, 0.0001));
    QVERIFY(calc::AlmostEqual(util::InterpolateMonitorUnits(25, map1), 2.5, 0.0001));
    QVERIFY(calc::AlmostEqual(util::InterpolateMonitorUnits(30, map1), 3.0, 0.0001));
    try {
        util::InterpolateMonitorUnits(9, map1);
        QFAIL("TesUtil::TestInterpolateMonitorUnits Lower bounds exception should have been thrown");
    }
    catch (...) {}
    try {
        util::InterpolateMonitorUnits(31, map1);
        QFAIL("TesUtil::TestInterpolateMonitorUnits Upper bounds exception should have been thrown");
    }
    catch (...) {}

    std::map<int, double> map2;
    map2[10] = 1.0;
    QVERIFY(calc::AlmostEqual(util::InterpolateMonitorUnits(10, map2), 1.0, 0.0001));

    std::map<int, double> map3;
    try {
        util::InterpolateMonitorUnits(9, map3);
        QFAIL("TesUtil::TestInterpolateMonitorUnits An emtpy map should throw exception");
    }
    catch (...) {}
}

void TestUtil::TestExtrapolateMonitorUnits() {
    std::map<int, double> map1;
    map1[10] = 1.0;
    map1[100] = 17.0;
    map1[40] = 5.0;
    QVERIFY(calc::AlmostEqual(util::ExtrapolateMonitorUnits(101, map1), 17.2, 0.0001));
    QVERIFY(calc::AlmostEqual(util::ExtrapolateMonitorUnits(105, map1), 18.0, 0.0001));
    try {
        util::ExtrapolateMonitorUnits(111, map1);
        QFAIL("TestUtil::TestExtrapolateMonitorUnits Should have thrown an upper range exception");
    }
    catch (...) {}

    try {
        util::ExtrapolateMonitorUnits(90, map1);
        QFAIL("TestUtil::TestExtrapolateMonitorUnits Should have thrown an lower range exception");
    }
    catch (...) {}

     std::map<int, double> map2;
     map2[100] = 17.0;
     map2[40] = 5.0;
     QVERIFY(calc::AlmostEqual(util::ExtrapolateMonitorUnits(101, map2), 17.2, 0.0001));

     std::map<int, double> map3;
     map3[100] = 17.0;
     try {
         util::ExtrapolateMonitorUnits(104, map3);
         QFAIL("TestUtil::TestExtrapolateMonitorUnits Should have thrown an not enough data exception");
     }
     catch (...) {}
}

void TestUtil::TestInterpolateDepthDoseCurve() {
    std::map<int, DepthDoseCurve> map1;
    std::vector<DepthDose> depth_dose1 = { DepthDose(2.0, 10.0), DepthDose(3.0, 8.0), DepthDose(5.0, -2.0) };
    DepthDoseCurve curve1(depth_dose1);
    std::vector<DepthDose> depth_dose2 = { DepthDose(2.0, 20.0), DepthDose(3.0, 4.0), DepthDose(5.0, 2.0) };
    DepthDoseCurve curve2(depth_dose2);
    std::vector<DepthDose> depth_dose3 = { DepthDose(2.0, 30.0), DepthDose(3.0, 40.0), DepthDose(5.0, 20.0) };
    DepthDoseCurve curve3(depth_dose3);
    map1[2000] = curve1;
    map1[4000] = curve2;
    map1[6000] = curve3;

    QCOMPARE(util::InterpolateDepthDoseCurve(2000, map1), curve1);
    QCOMPARE(util::InterpolateDepthDoseCurve(4000, map1), curve2);
    QCOMPARE(util::InterpolateDepthDoseCurve(6000, map1), curve3);
    std::vector<DepthDose> depth_dose4 = { DepthDose(2.0, 15.0), DepthDose(3.0, 6.0), DepthDose(5.0, 0.0) };
    DepthDoseCurve exp_curve(depth_dose4);
    QCOMPARE(util::InterpolateDepthDoseCurve(3000, map1), exp_curve);
    try {
        util::InterpolateDepthDoseCurve(1000, map1);
        QFAIL("TestUtil::TestInterpolateDepthDoseCurve Lower bound exception should have been thrown");
    }
    catch (...) {}

    try {
        util::InterpolateDepthDoseCurve(7000, map1);
        QFAIL("TestUtil::TestInterpolateDepthDoseCurve Upper bound exception should have been thrown");
    }
    catch (...) {}

    std::map<int, DepthDoseCurve> map2;
    map2[2000] = curve2;
    QCOMPARE(util::InterpolateDepthDoseCurve(2000, map2), curve2);

    std::map<int, DepthDoseCurve> map3;
    try {
        util::InterpolateDepthDoseCurve(5000, map3);
        QFAIL("TestUtil::TestInterpolateDepthDoseCurve Empty map should throw exception");
    }
    catch (...) {}
}

void TestUtil::TestExtrapolateDepthDoseCurve() {
    std::map<int, DepthDoseCurve> map1;
    std::vector<DepthDose> depth_dose1 = { DepthDose(2.0, 10.0), DepthDose(3.0, 8.0), DepthDose(5.0, -2.0) };
    DepthDoseCurve curve1(depth_dose1);
    std::vector<DepthDose> depth_dose2 = { DepthDose(2.0, 30.0), DepthDose(3.0, 4.0), DepthDose(5.0, 20.0) };
    DepthDoseCurve curve2(depth_dose2);
    std::vector<DepthDose> depth_dose3 = { DepthDose(2.0, 30.0), DepthDose(3.0, 40.0), DepthDose(5.0, 18.0) };
    DepthDoseCurve curve3(depth_dose3);
    map1[2000] = curve1;
    map1[3000] = curve2;
    map1[7000] = curve3;

    std::vector<DepthDose> exp_doses7100 = { DepthDose(2.0, 30.0), DepthDose(3.0, 40.9), DepthDose(5.0, 17.95) };
    DepthDoseCurve exp_curve7100(exp_doses7100);
    auto curve7100(util::ExtrapolateDepthDoseCurve(7100, map1));
    QCOMPARE(curve7100, exp_curve7100);

    try {
        util::ExtrapolateDepthDoseCurve(7800, map1);
        QFAIL("TestUtil::TestExtrapolateDepthDoseCurve Should have thrown an upper range exception");
    }
    catch (...) {}

    try {
        util::ExtrapolateDepthDoseCurve(6999, map1);
        QFAIL("TestUtil::TestExtrapolateDepthDoseCurve Should have thrown an lower range exception");
    }
    catch (...) {}

}


void TestUtil::TestDebitDebroca() {
    const double ref_debit(1.37);

    std::map<int, double> um_plexi_weight1;
    for (int i = 0; i < 19; ++i) {
        um_plexi_weight1[800*i] = (double)i*10.0;
    }
    Modulateur mod1(QString("1"), 0.0, 0.0, 0.0, 0, um_plexi_weight1, true);
    DegradeurSet deg_set1(Degradeur(QString("1"), 7800, true));
    const double debit1(util::DebitDebroca(ref_debit, mod1, deg_set1));
    QVERIFY(calc::AlmostEqual(debit1, ref_debit * 0.6216, 0.001));

    std::map<int, double> um_plexi_weight2;
    for (int i = 0; i < 11; ++i) {
        um_plexi_weight2[800*i] = (double)i*10.0;
    }
    Modulateur mod2(QString("1"), 0.0, 0.0, 0.0, 0, um_plexi_weight2, true);
    DegradeurSet deg_set2(Degradeur(QString("1"), 18700, true));
    const double debit2(util::DebitDebroca(ref_debit, mod2, deg_set2));
    QVERIFY(calc::AlmostEqual(debit2, ref_debit * 0.4188, 0.01));

    DegradeurSet deg_set3(Degradeur(QString("1"), 0, true));
    const double debit3(util::DebitDebroca(ref_debit, mod2, deg_set3));
    QVERIFY(calc::AlmostEqual(debit3, ref_debit * 1.52247, 0.001));

    std::map<int, double> um_plexi_weight4;
    for (int i = 0; i < 19; ++i) {
        um_plexi_weight4[800*i] = (double)i*10.0;
    }
    Modulateur mod4(QString("1"), 0.0, 0.0, 0.0, 500, um_plexi_weight4, true);
    DegradeurSet deg_set4(Degradeur(QString("1"), 2300, true), Degradeur(QString("2"), 5000, true));
    const double debit4(util::DebitDebroca(ref_debit, mod4, deg_set4));
    QVERIFY(calc::AlmostEqual(debit4, ref_debit * 0.6216, 0.001));
}

void TestUtil::TestStripperCurrent() {
    QVERIFY(std::abs(util::GetStripperCurrent(0.0, 1.0, 1.0)) < 0.0001);
    QVERIFY(std::abs(util::GetStripperCurrent(1.0, 0.0, 1.0)) < 0.0001);
    QVERIFY(std::abs(util::GetStripperCurrent(1.0, 1.0, 0.0)) < 0.0001);

}

void TestUtil::TestSeanceDuration() {
    QVERIFY(std::abs(util::GetSeanceDuration(0.0, 1.0, 5.0)) < 0.0001);
    QVERIFY(std::abs(util::GetSeanceDuration(1.0, 0.0, 5.0)) < 0.0001);
    QVERIFY(std::abs(util::GetSeanceDuration(1.0, 1.0, 0.0)) < 0.0001);
}

void TestUtil::TestStripperCurrentAndSeanceDuration() {
    double i_stripper(0.0);
    double duration(0.0);

    bool current_possible = util::GetStripperCurrentAndSeanceDuration(1.0, 10.0, 1, &i_stripper, &duration);
    QVERIFY(!current_possible);
    QVERIFY(std::abs(i_stripper - 400.0) < 0.01);
    QVERIFY(duration > 1.01);

    current_possible = util::GetStripperCurrentAndSeanceDuration(1.0, 10.0, 10, &i_stripper, &duration);
    QVERIFY(current_possible);
    QVERIFY(i_stripper < 399.0);
    QVERIFY(std::abs(duration - 10.0) < 0.01);

    current_possible = util::GetStripperCurrentAndSeanceDuration(1.0, 10.0, 1000, &i_stripper, &duration);
    QVERIFY(!current_possible);
    QVERIFY(i_stripper > 19.0);
}


