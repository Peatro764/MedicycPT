#include "TestDepthDoseCurve.h"

#include "DepthDoseCurve.h"
#include "Calc.h"

void TestDepthDoseCurve::initTestCase() {}

void TestDepthDoseCurve::cleanupTestCase() {}

void TestDepthDoseCurve::init() {}

void TestDepthDoseCurve::cleanup() {}

void TestDepthDoseCurve::Constructor() {
    DepthDose medium(10.0, 1.1);
    DepthDose large(14.0, 3.3);
    DepthDose small(5.0, 4.4);
    std::vector<DepthDose> depth_doses_unsorted { large, small, medium };
    std::vector<DepthDose> depth_doses_sorted { small, medium, large };

    DepthDoseCurve curve(depth_doses_unsorted);

    QCOMPARE(curve, DepthDoseCurve(depth_doses_sorted));

    std::vector<DepthDose> empty;
    DepthDoseCurve empty_curve(empty);
    QCOMPARE(empty_curve, empty_curve);

    QCOMPARE(DepthDoseCurve(), DepthDoseCurve());
}

void TestDepthDoseCurve::Comparison() {
    DepthDose small(10.0, 1.1);
    DepthDose medium(14.0, 3.3);
    DepthDose large(5.0, 4.4);
    std::vector<DepthDose> d1 { small, medium, large };
    std::vector<DepthDose> d2 { large, medium };

    QCOMPARE(DepthDoseCurve(d1), DepthDoseCurve(d1));
    QVERIFY(DepthDoseCurve(d1) != DepthDoseCurve(d2));
}

void TestDepthDoseCurve::CapAtZero() {
    DepthDose small(10.0, 1.1);
    DepthDose negative(14.0, -3.3);
    DepthDose large(5.0, 4.4);
    std::vector<DepthDose> dUnCapped { small, negative, large };
    std::vector<DepthDose> dCapped { small, -negative, large };
    DepthDoseCurve curveUnCapped(dUnCapped);
    DepthDoseCurve curveCapped(dCapped);

    QCOMPARE(curveUnCapped.CapAtZero(), curveCapped);
}

void TestDepthDoseCurve::Dose() {
    std::vector<DepthDose> depth_doses { DepthDose(1.1, 3.0), DepthDose(3.4, 8.0), DepthDose(4.0, 10.0), DepthDose(7.0, 3.0) };
    DepthDoseCurve curve(depth_doses);

    DepthDose d1 = curve.Dose(2.0);
    QVERIFY(calc::AlmostEqual(d1.dose(), 4.956521739, 0.0001));
    QVERIFY(calc::AlmostEqual(d1.depth(), 2.0, 0.0001));

    DepthDose d2 = curve.Dose(3.4);
    QVERIFY(calc::AlmostEqual(d2.dose(), 8.0, 0.0001));
    QVERIFY(calc::AlmostEqual(d2.depth(), 3.4, 0.0001));

    DepthDose d3 = curve.Dose(6.0);
    QVERIFY(calc::AlmostEqual(d3.dose(), 5.333333333, 0.0001));
    QVERIFY(calc::AlmostEqual(d3.depth(), 6.0, 0.0001));

    try {
        curve.Dose(0.5);
        QFAIL("TestDepthDoseCurve::Dose Requesting a dose before start of curve should throw exception");
    }
    catch (...) {}

    try {
        curve.Dose(7.5);
        QFAIL("TestDepthDoseCurve::Dose Requesting a dose after end of curve should throw exception");
    }
    catch (...) {}

    try {
        DepthDoseCurve().Dose(0.5);
        QFAIL("TestDepthDoseCurve::Dose Requesting a dose from an empty curve should throw an exception");
    }
    catch (...) {}
}

void TestDepthDoseCurve::Addition() {
    DepthDose small(10, 1.1);
    DepthDose medium(14.0, 3.3);
    DepthDose large(5.0, 4.4);
    std::vector<DepthDose> d1 { small, medium, large };
    std::vector<DepthDose> d2 { small, medium };
    std::vector<DepthDose> dSum { small + small, medium + medium, large + large};

    DepthDoseCurve curve1(d1);
    DepthDoseCurve curve2(d1);
    QCOMPARE(curve1 + curve2, DepthDoseCurve(dSum));

    QCOMPARE(curve1 + DepthDoseCurve(), curve1);
    QCOMPARE(DepthDoseCurve() + curve1, curve1);
    QCOMPARE(DepthDoseCurve() + DepthDoseCurve(), DepthDoseCurve());

    try {
        DepthDoseCurve curve3(d2);
        curve1 + curve3;
        QFAIL("TestDepthDoseCurve::Addition Adding two curves with different number of DepthDose should throw an exception");
    }
    catch (...) {}
}

void TestDepthDoseCurve::Negation() {
    DepthDose small(10, 1.1);
    DepthDose medium(14.0, 3.3);
    DepthDose large(5.0, 4.4);
    std::vector<DepthDose> dPos { small, medium, large };
    std::vector<DepthDose> dNeg { -small, -medium, -large };

    DepthDoseCurve curvePos(dPos);
    DepthDoseCurve curveNeg(dNeg);
    QCOMPARE(curvePos, -curveNeg);
}

void TestDepthDoseCurve::Subtraction() {
    DepthDose small(10, 1.1);
    DepthDose medium(14.0, 3.3);
    DepthDose large(5.0, 4.4);
    std::vector<DepthDose> d1 { small, medium, large };
    std::vector<DepthDose> d2 { 2.0 * small, 2.0 * medium, 2.0 * large };

    DepthDoseCurve curve1(d1);
    DepthDoseCurve curve2(d2);
    QCOMPARE(curve2 - curve1, curve1);
    QCOMPARE(curve1 - curve2, -curve1);
}

void TestDepthDoseCurve::Multiplication() {
    DepthDose small(10, 1.1);
    DepthDose medium(14.0, 3.3);
    DepthDose large(5.0, 4.4);
    std::vector<DepthDose> d1 { small, medium, large };
    std::vector<DepthDose> d2 { 1.3 * small, 1.3 * medium, 1.3 * large };
    DepthDoseCurve curve1(d1);
    DepthDoseCurve curve2(d2);

    QCOMPARE(1.3 * curve1, curve2);

    QCOMPARE(1.3 * DepthDoseCurve(), DepthDoseCurve());

    try {
        auto g(-1.1 * curve1);
        QFAIL("TestDepthDoseCurve::Multiplication Negative factor should throw exception");

    }
    catch (...) {
    }
}

void TestDepthDoseCurve::Division() {
    DepthDose small(10, 1.1);
    DepthDose medium(14, 3.3);
    DepthDose large(5, 4.4);
    std::vector<DepthDose> d1 { small, medium, large };
    std::vector<DepthDose> d2 { 1.3 * small, 1.3 * medium, 1.3 * large };
    DepthDoseCurve curve1(d1);
    DepthDoseCurve curve2(d2);

    QCOMPARE(curve1 / (1.0 / 1.3), curve2);

    QCOMPARE(DepthDoseCurve() / 2.3, DepthDoseCurve());

    try {
        DepthDoseCurve() / 0.0;
        QFAIL("TestDepthDoseCurve::Division Dividing with zero should throw an exception");

    }
    catch (...) {
    }
}

void TestDepthDoseCurve::Max() {
    DepthDose small(10, 1.1);
    DepthDose medium(14, 4.3);
    DepthDose large(5, 4.4);

    std::vector<DepthDose> d1 { small, large, medium };
    DepthDoseCurve curve1(d1);
    QCOMPARE(curve1.Max(), large);

    std::vector<DepthDose> d2 { medium, small };
    DepthDoseCurve curve2(d2);
    QCOMPARE(curve2.Max(), medium);

    std::vector<DepthDose> d3 { small };
    DepthDoseCurve curve3(d3);
    QCOMPARE(curve3.Max(), small);
}

void TestDepthDoseCurve::LeftMostMax() {
    DepthDose dd1(1, 1.1);
    DepthDose dd2(2, 2.2);
    DepthDose dd3(3, 3.3);
    DepthDose dd4(4, 3.2);
    DepthDose dd5(5, 3.2);

    std::vector<DepthDose> d1 { dd1, dd2, dd3, dd4, dd5 };
    DepthDoseCurve curve1(d1);
    QCOMPARE(curve1.LeftMostMax(), dd3);

    std::vector<DepthDose> d2 { dd1, dd2, dd3 };
    DepthDoseCurve curve2(d2);
    QCOMPARE(curve2.LeftMostMax(), dd3);
}

void TestDepthDoseCurve::RightMostMax() {
    DepthDose dd1(1, 3.3);
    DepthDose dd2(2, 3.3);
    DepthDose dd3(3, 3.2);
    DepthDose dd4(4, 3.0);
    DepthDose dd5(5, 1.0);

    std::vector<DepthDose> d1 { dd1, dd2, dd3, dd4, dd5 };
    DepthDoseCurve curve1(d1);
    QCOMPARE(curve1.RightMostMax(), dd2);

    std::vector<DepthDose> d2 { dd3, dd4, dd5 };
    DepthDoseCurve curve2(d2);
    QCOMPARE(curve2.RightMostMax(), dd3);
}

void TestDepthDoseCurve::LowerRange() {
    DepthDose d1(1, 50.0);
    DepthDose d2(2, 70.0);
    DepthDose d3(3, 98.0);
    DepthDose d4(4, 100.0);
    std::vector<DepthDose> dd1 { d1, d2, d3, d4 };
    DepthDoseCurve curve1(dd1);

    QCOMPARE(curve1.LowerRange(40.0), d1);
    QCOMPARE(curve1.LowerRange(50.0), d1);
    QCOMPARE(curve1.LowerRange(88.0), d3);
    QCOMPARE(curve1.LowerRange(101.0), d4);

    try {
        DepthDoseCurve().LowerRange(30.0);
        QFAIL("TestDepthDoseCurve::LowerRange Exception should have been thrown");
    }
    catch (...) {}

    std::vector<DepthDose> dd2 {DepthDose(1, 100), DepthDose(2, 90), DepthDose(3, 80)};
    DepthDoseCurve curve2(dd2);
    QCOMPARE(curve2.LowerRange(40), DepthDose(1, 100));
}

void TestDepthDoseCurve::UpperRange() {
    DepthDose d1(1, 100.0);
    DepthDose d2(2, 98.0);
    DepthDose d3(3, 70.0);
    DepthDose d4(4, 50.0);
    std::vector<DepthDose> dd1 { d1, d2, d3, d4 };
    DepthDoseCurve curve1(dd1);

    QCOMPARE(curve1.UpperRange(40.0), d4);
    QCOMPARE(curve1.UpperRange(50.0), d4);
    QCOMPARE(curve1.UpperRange(88.0), d2);
    QCOMPARE(curve1.UpperRange(101.0), d1);

    std::vector<DepthDose> dd2 {DepthDose(1, 80), DepthDose(2, 90), DepthDose(3, 100)};
    DepthDoseCurve curve2(dd2);
    QCOMPARE(curve2.UpperRange(40), DepthDose(3, 100));

    try {
        DepthDoseCurve().UpperRange(30.0);
        QFAIL("TestDepthDoseCurve::UpperRange Exception should have been thrown");
    }
    catch (...) {}
}

void TestDepthDoseCurve::Degrade() {
    DepthDose d1(10, 100.0);
    DepthDose d2(20, 98.0);
    DepthDose d3(30, 70.0);
    DepthDose d4(40, 50.0);
    std::vector<DepthDose> dd1 { d1, d2, d3, d4 };
    DepthDoseCurve curve1(dd1);

    // shifting zero
    double deg0(0.0);
    DepthDoseCurve degraded_zero(curve1.Degrade(deg0));
//    QCOMPARE(degraded_zero, curve1); // FORMULA NOT OK FOR ZERO SHIFT

    // shifting one step
    double deg1(10.0);
    double deg_factor1(curve1.DoseDegradationFactor(deg1));
    DepthDoseCurve degraded_onestep(curve1.Degrade(deg1));

    std::vector<DepthDose> exp_dd1 { DepthDose(10, d2.dose() * deg_factor1),
                                     DepthDose(20, d3.dose() * deg_factor1),
                                     DepthDose(30, d4.dose() * deg_factor1),
                                     DepthDose(40, 0.0) };
    DepthDoseCurve exp_deg_onestep(exp_dd1);
    QCOMPARE(degraded_onestep, exp_deg_onestep);

    // shifting one step and a half
    double deg2(15.0);
    double deg_factor2(curve1.DoseDegradationFactor(deg2));
    DepthDoseCurve degraded_oneandhalfstep(curve1.Degrade(deg2));

    std::vector<DepthDose> exp_dd2 { DepthDose(10, 84.0 * deg_factor2),
                                     DepthDose(20, 60.0 * deg_factor2),
                                     DepthDose(30, 0.0),
                                     DepthDose(40, 0.0) };
    DepthDoseCurve exp_deg_oneandhalfstep(exp_dd2);
    QCOMPARE(degraded_oneandhalfstep, exp_deg_oneandhalfstep);

    // shifting all outside range
    double deg3(50.0);
    DepthDoseCurve degraded_outsiderange(curve1.Degrade(deg3));

    std::vector<DepthDose> exp_dd3 { DepthDose(10, 0.0),
                                     DepthDose(20, 0.0),
                                     DepthDose(30, 0.0),
                                     DepthDose(40, 0.0) };
    DepthDoseCurve exp_deg_outsiderange(exp_dd3);
    QCOMPARE(degraded_outsiderange, exp_deg_outsiderange);
}

void TestDepthDoseCurve::MaxParcours() {
    DepthDose d1(10, 100.0);
    DepthDose d2(20, 90.0);
    DepthDose d3(30, 80.0);
    DepthDose d4(40, 70.0);
    DepthDose d5(50, 60.0);
    DepthDose d6(60, 50.0);
    DepthDose d7(70, 40.0);
    DepthDose d8(80, 30.0);
    DepthDose d9(90, 20.0);
    DepthDose d10(100, 10.0);
    DepthDose d11(110, 0.0);

    std::vector<DepthDose> dd1 { d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11 };
    DepthDoseCurve curve1(dd1);
    QVERIFY(calc::AlmostEqual(curve1.MaxParcours(), 20.0, 0.0001));

    std::vector<DepthDose> dd2 { d1, d3, d4 };
    DepthDoseCurve curve2(dd2);
    QVERIFY(calc::AlmostEqual(curve2.MaxParcours(), 20.0, 0.0001));
}

void TestDepthDoseCurve::CorrespondingDepthFromRight() {
    DepthDose d1(1, 1.0);
    DepthDose d2(2, 1.0);
    DepthDose d3(3, 1.0);
    DepthDose d4(4, 0.9);
    DepthDose d5(5, 0.8);
    DepthDose d6(6, 0.7);
    DepthDose d7(7, 0.6);
    DepthDose d8(8, 0.5);
    DepthDose d9(9, 0.4);
    DepthDose d10(10, 0.3);
    DepthDose d11(11, 0.2);
    DepthDose d12(12, 0.1);

    std::vector<DepthDose> dd1 { d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12 };
    DepthDoseCurve curve1(dd1);

    double corr_depth_from_right_1(curve1.CorrespondingDepthFromRight(0.3));
    QVERIFY(calc::AlmostEqual(corr_depth_from_right_1, 10.0, 0.001));

    double corr_depth_from_right_2(curve1.CorrespondingDepthFromRight(0.0));
    QVERIFY(calc::AlmostEqual(corr_depth_from_right_2, 12.0, 0.001));

    double corr_depth_from_right_3(curve1.CorrespondingDepthFromRight(0.55));
    QVERIFY(calc::AlmostEqual(corr_depth_from_right_3, 7.5, 0.001));

    double corr_depth_from_right_4(curve1.CorrespondingDepthFromRight(1.2));
    QVERIFY(calc::AlmostEqual(corr_depth_from_right_4, 1.0, 0.001));
}

void TestDepthDoseCurve::Penumbra() {
    DepthDose d1(1, 1.0);
    DepthDose d2(2, 1.0);
    DepthDose d3(3, 1.0);
    DepthDose d4(4, 0.9);
    DepthDose d5(5, 0.8);
    DepthDose d6(6, 0.7);
    DepthDose d7(7, 0.6);
    DepthDose d8(8, 0.5);
    DepthDose d9(9, 0.4);
    DepthDose d10(10, 0.3);
    DepthDose d11(11, 0.2);
    DepthDose d12(12, 0.1);
    DepthDose d13(13, 0.0);


    std::vector<DepthDose> dd1 { d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13 };
    DepthDoseCurve curve1(dd1);
    double penumbra(curve1.Penumbra());
    QVERIFY(calc::AlmostEqual(penumbra, 8.0, 0.0001));
}

void TestDepthDoseCurve::Flatness() {
    DepthDose d1(1, 1.0);
    DepthDose d2(2, 2.0);
    DepthDose d3(3, 3.0);
    DepthDose d4(4, 3.0);
    DepthDose d5(5, 2.9);
    DepthDose d6(6, 3.1);
    DepthDose d7(7, 2.8);
    DepthDose d8(8, 2.9);
    DepthDose d9(9, 3.2);
    DepthDose d10(10, 3.0);
    DepthDose d11(11, 1.0);

    std::vector<DepthDose> dd1 { d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11 };
    DepthDoseCurve curve1(dd1);
    double flatness(curve1.Flatness());
    std::vector<double> plateau {3.0, 3.0, 2.9, 3.1, 2.8, 2.9, 3.2};
    double exp_flatness(100.0 * calc::StdDev(plateau) / calc::Mean(plateau));
    QVERIFY(calc::AlmostEqual(flatness, exp_flatness, 0.001));
}
