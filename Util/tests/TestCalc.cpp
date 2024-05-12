#include "TestCalc.h"

#include <iostream>
#include <QDateTime>

#include "Calc.h"

void TestCalc::initTestCase() {
    maxRelDiff_ = 0.001;
}

void TestCalc::cleanupTestCase() {}

void TestCalc::init() {}

void TestCalc::cleanup() {}

void TestCalc::TestAlmostEqual() {
    QVERIFY(calc::AlmostEqual(0.001, 0.0011, 0.15));
    QVERIFY(!calc::AlmostEqual(0.001, 0.0011, 0.05));

    QVERIFY(calc::AlmostEqual(1.0, 1.0001, 0.001));
    QVERIFY(!calc::AlmostEqual(1.0, 1.0001, 0.00001));

    QVERIFY(calc::AlmostEqual(10000.0, 10001.0, 0.001));
    QVERIFY(!calc::AlmostEqual(10000.0, 10001.0, 0.00001));

    std::vector<double> v1 { 0.0001, 0.001, 0.01, 0.1, 1.0, 10.0, 100.0, 1000.0, 10000.0 };
    std::vector<double> v2 { 0.00011, 0.0011, 0.011, 0.11, 1.1, 11.0, 110.0, 1100.0, 11000.0 };

    QVERIFY(calc::AlmostEqual(v1, v1, 0.001));
    QVERIFY(calc::AlmostEqual(v1, v2, 0.11));
    QVERIFY(!calc::AlmostEqual(v1, v2, 0.09));
}

void TestCalc::TestMean() {
    std::vector<double> empty;
    std::vector<double> oneElement = { 2.22 };
    std::vector<double> twoElements = { 1.11, 2.22 };
    std::vector<double> threeElements = { 1.11, 2.22, 3.33 };
    std::vector<double> twentyElements = { 1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2};

    QVERIFY(calc::AlmostEqual(calc::Mean(empty), 0.0, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Mean(oneElement), oneElement.at(0), maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Mean(twoElements), 0.5*(1.11 + 2.22), maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Mean(threeElements), (1.11 + 2.22 + 3.33)/3.0, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Mean(twentyElements), 1.5, maxRelDiff_));
}

void TestCalc::TestMedian() {
    std::vector<double> empty;
    std::vector<double> oneElement = { 2.0 };
    std::vector<double> twoElements = { 2.0, 1.0 };
    std::vector<double> threeElements = { 1.0, 30.0, 2.0 };
    std::vector<double> tenElements = { 1.0, 1000.0, 0.0, -1.0, -3.0, 5.0, 10.0, 5.0, 6.0, 4.0};

    QVERIFY(calc::AlmostEqual(calc::Median(empty), 0.0, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Median(oneElement), oneElement.at(0), maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Median(twoElements), 0.5*(1.0 + 2.0), maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Median(threeElements), 2.0, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Median(tenElements), 4.5, maxRelDiff_));
}

void TestCalc::TestStdDev() {
    std::vector<double> empty;
    std::vector<double> oneElement = { 2.22 };
    std::vector<double> twoElements = { 1.11, 2.22 };
    std::vector<double> threeElements = { 1.11, 2.22, 3.77 };
    std::vector<double> twentyElements = { 1,4,1,2,1,4,3,3,1,1,3,2,4,4,2,0,2,2,3,1};

    QVERIFY(calc::AlmostEqual(calc::StdDev(empty), 0.0, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::StdDev(oneElement), 0.0, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::StdDev(twoElements), 0.7848885271, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::StdDev(threeElements), 1.336051396, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::StdDev(twentyElements), 1.23969436, maxRelDiff_));
}

void TestCalc::TestSquare() {
    std::vector<double> oneElement = { 2.22 };
    std::vector<double> threeElements = { 1.11, 2.22, 3.77 };

    std::vector<double> expOne = { 2.22 * 2.22 };
    std::vector<double> expThree = { 1.11 * 1.11, 2.22 * 2.22, 3.77 *3.77 };
    QVERIFY(calc::AlmostEqual(calc::Square(oneElement), expOne, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::Square(threeElements), expThree, maxRelDiff_));
}

void TestCalc::TestCenterOfGravity() {
    std::vector<double> a = { 1, 4, 6 };
    std::vector<double> b = { 2, 1, 5 };
    double exp = (1.0 / 3.0) * (2 + 4 + 30);
    QVERIFY(calc::AlmostEqual(calc::CenterOfGravity(a, b), exp, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(calc::CenterOfGravity(b, a), exp, maxRelDiff_));
}

void TestCalc::TestPearson() {
    std::vector<double> a = { 1, 2, 3 };
    std::vector<double> b = { 2, 4, 6 };
    const double pearson = calc::PearsonCorrelation(a, b);
    QVERIFY(calc::AlmostEqual(pearson, 1.0, maxRelDiff_));
}

void TestCalc::TestTStatistic() {
    std::vector<double> a = {1,2,2,3,3,4,4,5,5,6};
    std::vector<double> b = { 1,2,4,5,5,5,6,6,7,9 };
    const double t = calc::TStatistic(a, b);
    QVERIFY(calc::AlmostEqual(t, -1.6948, maxRelDiff_));
}

void TestCalc::TestLinearFit() {
    std::vector<double> a = { 1, 2, 3 };
    std::vector<double> b = { 2, 4, 6 };
    const std::pair<double, double> fit_param = calc::LinearFit(a, b);
    qDebug() << "FITPARAM " << fit_param.first << " " << fit_param.second;
    QVERIFY(calc::AlmostEqual(fit_param.first, 0.0, maxRelDiff_));
    QVERIFY(calc::AlmostEqual(fit_param.second, 2.0, maxRelDiff_));
}

void TestCalc::TestFiltered() {
    std::vector<double> raw1 = { 1.0, 1.0, 0.7, 0.8, 1.1, 0.9, 2.0, 2.2, 0.7, 0.9 };

    // 1.13 += 0.5
    std::vector<double> exp_filtered_1sigma = { 1.0, 1.0, 0.7, 0.8, 1.1, 0.9, 0.7, 0.9 };
    auto real_filtered_1sigma = calc::Filter(raw1, 1.0);
    QCOMPARE(exp_filtered_1sigma, real_filtered_1sigma);

    // 1.13 += 0.25
    std::vector<double> exp_filtered_0k5sigma = { 1.0, 1.0, 1.1, 0.9, 0.9 };
    auto real_filtered_0k5sigma = calc::Filter(raw1, 0.5);
    QCOMPARE(exp_filtered_0k5sigma, real_filtered_0k5sigma);
}

void TestCalc::TestDoseToUM() {
    QVERIFY(calc::AlmostEqual(calc::DoseToUM(1.37f, 12.0f), 875.912, 0.0001));
    QVERIFY(calc::AlmostEqual(calc::DoseToUM(0.0, 12.0), 0.0, 0.0001));
    QVERIFY(calc::AlmostEqual(calc::DoseToUM(1.37, 0.0), 0.0, 0.0001));
    QVERIFY(calc::AlmostEqual(calc::DoseToUM(1.37, -12.0), 0.0, 0.0001));
}


void TestCalc::TestUM2MM() {
    QVERIFY(calc::AlmostEqual(calc::UM2MM(200), 0.2, 0.0001));
    QVERIFY(calc::AlmostEqual(calc::UM2MM(1200), 1.2, 0.0001));
    QVERIFY(calc::AlmostEqual(calc::UM2MM(6578), 6.578, 0.0001));
}

void TestCalc::TestMM2UM() {
    QCOMPARE(calc::MM2UM(3.2), 3200);
    QCOMPARE(calc::MM2UM(3.345676), 3346);
    QCOMPARE(calc::MM2UM(1234), 1234000);
}
