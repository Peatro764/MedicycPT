#include "TestQCPCurveUtils.h"

#include "QCPCurveUtils.h"

using namespace qcpcurveutils;

void TestQCPCurveUtils::initTestCase() {
    maxRelDiff_ = 0.001;
}

void TestQCPCurveUtils::cleanupTestCase() {}

void TestQCPCurveUtils::init() {}

void TestQCPCurveUtils::cleanup() {}

void TestQCPCurveUtils::TestQCPCurveDataOperators() {
    QCPCurveData data1;
    QCPCurveData data2(0.0, 1.0, 3.0);
    QCPCurveData data3(0.0, 2.0, 4.0);
    QCPCurveData data4(1.0, 2.0, 4.0);

    QCOMPARE(data3, data3);
    QCOMPARE(data3, data4); // first parameter (sorting parameter) should not be compared
    QVERIFY(data1 != data2);
    QVERIFY(data2 != data3);
}

void TestQCPCurveUtils::TestQCPCurveDataContainerOperators() {
    QCPCurveDataContainer data1;
    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(0.0, 1.0, 2.0));
    data2.add(QCPCurveData(1.0, 2.0, 3.0));
    QCPCurveDataContainer data3;
    data3.add(QCPCurveData(0.0, 1.0, 2.0));
    data3.add(QCPCurveData(1.0, 3.0, 3.0));
    QCPCurveDataContainer data4;
    data3.add(QCPCurveData(0.0, 1.0, 2.0));
    data3.add(QCPCurveData(1.0, 3.0, 3.0));
    data3.add(QCPCurveData(3.0, 4.0, 3.0));

    QCOMPARE(data2, data2);
    QVERIFY(data1 != data2);
    QVERIFY(data2 != data3);
    QVERIFY(data3 != data4);
}

void TestQCPCurveUtils::TestDiscretize() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 100.0));
    data1.add(QCPCurveData(2.0, 1.0, 98.0));
    data1.add(QCPCurveData(3.0, 2.0, 97.0));
    data1.add(QCPCurveData(4.0, 3.0, 97.0));
    data1.add(QCPCurveData(5.0, 4.0, 95.0));
    data1.add(QCPCurveData(6.0, 5.0, 93.0));
    data1.add(QCPCurveData(7.0, 6.0, 92.0));

    QCPCurveDataContainer discretized1(qcpcurveutils::Discretize(data1, 2.1, 1.0));
    QCPCurveDataContainer exp_data1;
    exp_data1.add(QCPCurveData(1.0, 0.0, 100.0));
    exp_data1.add(QCPCurveData(2.0, 1.0, 98.0));
    exp_data1.add(QCPCurveData(3.0, 3.0, 97.0));
    exp_data1.add(QCPCurveData(4.0, 4.0, 95.0));
    exp_data1.add(QCPCurveData(5.0, 5.0, 93.0));
    exp_data1.add(QCPCurveData(6.0, 6.0, 92.0));

    QCOMPARE(discretized1, exp_data1);

    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(1.0, 0.0, 0.0));
    data2.add(QCPCurveData(2.0, 1.0, 1.0));
    data2.add(QCPCurveData(3.0, 2.0, 1.4));
    data2.add(QCPCurveData(4.0, 3.0, 2.0));
    data2.add(QCPCurveData(5.0, 4.0, 3.4));
    data2.add(QCPCurveData(6.0, 5.0, 3.5));
    data2.add(QCPCurveData(7.0, 6.0, 3.9));

    QCPCurveDataContainer discretized2(qcpcurveutils::Discretize(data2, 1.5, 1.0));
    QCPCurveDataContainer exp_data2;
    exp_data2.add(QCPCurveData(1.0, 0.0, 0.0));
    exp_data2.add(QCPCurveData(3.0, 2.0, 1.4));
    exp_data2.add(QCPCurveData(4.0, 3.0, 2.0));
    exp_data2.add(QCPCurveData(5.0, 5.0, 3.5));
    exp_data2.add(QCPCurveData(7.0, 6.0, 3.9));

    QCOMPARE(discretized2, exp_data2);

    try {
        qcpcurveutils::Discretize(data2, 1.5, 1.1);
        QFAIL("Discretize should have thrown 1");
    }
    catch (...) {}

    try {
        qcpcurveutils::Discretize(data2, 1.5, 0.0);
        QFAIL("Discretize should have thrown 2");
    }
    catch (...) {}
}

void TestQCPCurveUtils::TestDiscretizeWithErrorScaling() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 100.0));
    data1.add(QCPCurveData(2.0, 1.0, 98.0));
    data1.add(QCPCurveData(3.0, 2.0, 97.0));
    data1.add(QCPCurveData(4.0, 3.0, 97.0));
    data1.add(QCPCurveData(5.0, 4.0, 95.0));
    data1.add(QCPCurveData(6.0, 5.0, 93.0));
    data1.add(QCPCurveData(7.0, 6.0, 92.0));
    data1.add(QCPCurveData(8.0, 7.0, 91.5));
    data1.add(QCPCurveData(9.0, 8.0, 91.2));
    data1.add(QCPCurveData(10.0, 9.0, 91.0));

    double max_err(2.1);
    QCPCurveDataContainer discretized1(qcpcurveutils::Discretize(data1, max_err, 0.01));
    for (auto it = (discretized1.constBegin() + 1); it < discretized1.constEnd(); ++it) {
        QVERIFY(std::fabs(it->mainValue() - (it - 1)->mainValue()) <= max_err);
    }

    QCPCurveDataContainer discretized2(qcpcurveutils::Discretize(data1, max_err, 0.1));
    for (auto it = (discretized2.constBegin() + 1); it < discretized2.constEnd(); ++it) {
        QVERIFY(std::fabs(it->mainValue() - (it - 1)->mainValue()) <= max_err);
    }

    QCPCurveDataContainer discretized3(qcpcurveutils::Discretize(data1, max_err, 0.5));
    for (auto it = (discretized3.constBegin() + 1); it < discretized3.constEnd(); ++it) {
        QVERIFY(std::fabs(it->mainValue() - (it - 1)->mainValue()) <= max_err);
    }

    QCPCurveDataContainer discretized4(qcpcurveutils::Discretize(data1, max_err, 0.9));
    for (auto it = (discretized4.constBegin() + 1); it < discretized4.constEnd(); ++it) {
        QVERIFY(std::fabs(it->mainValue() - (it - 1)->mainValue()) <= max_err);
    }
}

void TestQCPCurveUtils::TestScaleValueAxis() {
    QCPCurveDataContainer data;
    data.add(QCPCurveData(1.0, 2.0, 3.0));
    data.add(QCPCurveData(2.0, 3.0, 4.0));
    data.add(QCPCurveData(3.0, 4.0, -4.0));
    qcpcurveutils::ScaleValueAxis(data, 2.0);

    QCPCurveDataContainer exp_data;
    exp_data.add(QCPCurveData(1.0, 2.0, 6.0));
    exp_data.add(QCPCurveData(2.0, 3.0, 8.0));
    exp_data.add(QCPCurveData(3.0, 4.0, -8.0));

    QCOMPARE(data, exp_data);
}

void TestQCPCurveUtils::TestSetMaxValue() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 2.0, 3.0));
    data1.add(QCPCurveData(2.0, 3.0, 4.0));
    data1.add(QCPCurveData(3.0, 4.0, -4.0));
    qcpcurveutils::SetMaxValue(data1, -5.0);
    QCPCurveDataContainer exp_data1;
    exp_data1.add(QCPCurveData(1.0, 2.0, -6.0));
    exp_data1.add(QCPCurveData(2.0, 3.0, -5.0));
    exp_data1.add(QCPCurveData(3.0, 4.0, -13.0));
    QCOMPARE(data1, exp_data1);

    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(1.0, 2.0, 3.0));
    data2.add(QCPCurveData(2.0, 3.0, 4.0));
    data2.add(QCPCurveData(3.0, 4.0, -4.0));
    qcpcurveutils::SetMaxValue(data2, 0.0);
    QCPCurveDataContainer exp_data2;
    exp_data2.add(QCPCurveData(1.0, 2.0, -1.0));
    exp_data2.add(QCPCurveData(2.0, 3.0, 0.0));
    exp_data2.add(QCPCurveData(3.0, 4.0, -8.0));
    QCOMPARE(data2, exp_data2);

    QCPCurveDataContainer data3;
    data3.add(QCPCurveData(1.0, 2.0, 3.0));
    data3.add(QCPCurveData(2.0, 3.0, 4.0));
    data3.add(QCPCurveData(3.0, 4.0, -4.0));
    qcpcurveutils::SetMaxValue(data3, 6.0);
    QCPCurveDataContainer exp_data3;
    exp_data3.add(QCPCurveData(1.0, 2.0, 5.0));
    exp_data3.add(QCPCurveData(2.0, 3.0, 6.0));
    exp_data3.add(QCPCurveData(3.0, 4.0, -2.0));
    QCOMPARE(data3, exp_data3);
}

void TestQCPCurveUtils::TestCutAt() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 2.0, 3.0));
    data1.add(QCPCurveData(2.0, 3.0, 4.0));
    data1.add(QCPCurveData(3.0, 4.0, -4.0));
    auto cut1(qcpcurveutils::CutAt(data1, 0.0, true));
    QCPCurveDataContainer exp_data1;
    exp_data1.add(QCPCurveData(1.0, 2.0, 3.0));
    exp_data1.add(QCPCurveData(2.0, 3.0, 4.0));
    QCOMPARE(cut1, exp_data1);

    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(1.0, 2.0, -3.0));
    data2.add(QCPCurveData(2.0, 3.0, -4.0));
    data2.add(QCPCurveData(3.0, 4.0, -4.0));
    auto cut2(qcpcurveutils::CutAt(data2, 0.0, true));
    QCPCurveDataContainer exp_data2;
    QCOMPARE(cut2, exp_data2);

    QCPCurveDataContainer data3;
    data3.add(QCPCurveData(1.0, 2.0, 3.0));
    data3.add(QCPCurveData(2.0, 3.0, 4.0));
    data3.add(QCPCurveData(3.0, 4.0, 4.0));
    auto cut3(qcpcurveutils::CutAt(data3, 0.0, true));
    QCOMPARE(cut3, data3);

    QCPCurveDataContainer data4;
    data4.add(QCPCurveData(1.0, 2.0, 3.0));
    data4.add(QCPCurveData(2.0, 3.0, 4.0));
    data4.add(QCPCurveData(3.0, 4.0, -4.0));
    auto cut4(qcpcurveutils::CutAt(data4, 0.0, false));
    QCPCurveDataContainer exp_data4;
    exp_data4.add(QCPCurveData(3.0, 4.0, -4.0));
    QCOMPARE(cut4, exp_data4);
}

void TestQCPCurveUtils::TestStepCurve() {
    // Minimum step height respected 1
    QCPCurveDataContainer data3a;
    data3a.add(QCPCurveData(1.0, 0.0, 3.0));
    data3a.add(QCPCurveData(2.0, 1.0, 1.0));
    data3a.add(QCPCurveData(3.0, 2.0, 0.5));
    data3a.add(QCPCurveData(4.0, 2.0, 0.0));

    QCPCurveDataContainer step3a(qcpcurveutils::StepCurve(data3a, 1.0));
    QCPCurveDataContainer exp_data3a;
    exp_data3a.add(QCPCurveData(1.0, 0.0, 3.0));
    exp_data3a.add(QCPCurveData(2.0, 1.0, 3.0));
    exp_data3a.add(QCPCurveData(3.0, 1.0, 1.0));
    exp_data3a.add(QCPCurveData(4.0, 2.0, 1.0));
    exp_data3a.add(QCPCurveData(5.0, 2.0, 0.0));
    QCOMPARE(step3a, exp_data3a);

    // Minimum step height respected 2
    QCPCurveDataContainer data3b;
    data3b.add(QCPCurveData(1.0, 0.0, 4.0));
    data3b.add(QCPCurveData(2.0, 1.0, 3.0));
    data3b.add(QCPCurveData(3.0, 2.0, 2.0));
    data3b.add(QCPCurveData(4.0, 3.0, 0.5));
    data3b.add(QCPCurveData(5.0, 3.0, 0.0));

    QCPCurveDataContainer step3b(qcpcurveutils::StepCurve(data3b, 1.0));
    QCPCurveDataContainer exp_data3b;
    exp_data3b.add(QCPCurveData(1.0, 0.0, 4.0));
    exp_data3b.add(QCPCurveData(2.0, 1.0, 4.0));
    exp_data3b.add(QCPCurveData(3.0, 1.0, 2.0));
    exp_data3b.add(QCPCurveData(4.0, 3.0, 2.0));
    exp_data3b.add(QCPCurveData(5.0, 3.0, 0.0));
    QCOMPARE(step3b, exp_data3b);

    // last step "from below"
    QCPCurveDataContainer data4;
    data4.add(QCPCurveData(1.0, 0.0, 6.0));
    data4.add(QCPCurveData(2.0, 1.0, 5.0));
    data4.add(QCPCurveData(3.0, 2.0, 4.0));
    data4.add(QCPCurveData(4.0, 3.0, 3.0));
    data4.add(QCPCurveData(5.0, 4.0, 2.0));
    data4.add(QCPCurveData(6.0, 5.0, 1.0));
    data4.add(QCPCurveData(7.0, 5.0, 0.0));

    QCPCurveDataContainer step4(qcpcurveutils::StepCurve(data4, 0.0));
    QCPCurveDataContainer exp_data4;
    exp_data4.add(QCPCurveData(1.0, 0.0, 6.0));
    exp_data4.add(QCPCurveData(2.0, 1.0, 6.0));
    exp_data4.add(QCPCurveData(3.0, 1.0, 4.0));
    exp_data4.add(QCPCurveData(4.0, 3.0, 4.0));
    exp_data4.add(QCPCurveData(5.0, 3.0, 2.0));
    exp_data4.add(QCPCurveData(6.0, 5.0, 2.0));
    exp_data4.add(QCPCurveData(7.0, 5.0, 0.0));

    QCOMPARE(step4, exp_data4);

    // last step "from above"
    QCPCurveDataContainer data5;
    data5.add(QCPCurveData(1.0, 0.0, 5.0));
    data5.add(QCPCurveData(2.0, 1.0, 4.0));
    data5.add(QCPCurveData(3.0, 2.0, 3.0));
    data5.add(QCPCurveData(4.0, 3.0, 2.0));
    data5.add(QCPCurveData(5.0, 4.0, 1.0));
    data5.add(QCPCurveData(6.0, 4.0, 0.0));

    QCPCurveDataContainer step5(qcpcurveutils::StepCurve(data5, 0.0));
    QCPCurveDataContainer exp_data5;
    exp_data5.add(QCPCurveData(1.0, 0.0, 5.0));
    exp_data5.add(QCPCurveData(2.0, 1.0, 5.0));
    exp_data5.add(QCPCurveData(3.0, 1.0, 3.0));
    exp_data5.add(QCPCurveData(4.0, 3.0, 3.0));
    exp_data5.add(QCPCurveData(5.0, 3.0, 1.0));
    exp_data5.add(QCPCurveData(6.0, 4.0, 1.0));
    exp_data5.add(QCPCurveData(7.0, 4.0, 0.0));

    QCOMPARE(step5, exp_data5);
}

void TestQCPCurveUtils::TestMaxElement() {
    QCPCurveDataContainer data;
    data.add(QCPCurveData(1.0, 0.0, 3.0));
    data.add(QCPCurveData(2.0, 1.0, 1.0));
    data.add(QCPCurveData(3.0, 2.0, 3.5));
    data.add(QCPCurveData(4.0, 3.0, 0.0));
    QCPCurveData max1 = qcpcurveutils::MaxElement(data);
    QCOMPARE(max1, QCPCurveData(3.0, 2.0, 3.5));
    QCPCurveData max2 = qcpcurveutils::MaxElement(data, 0, 0);
    QCOMPARE(max2, QCPCurveData(1.0, 0.0, 3.0));
    QCPCurveData max3 = qcpcurveutils::MaxElement(data, 0, 1);
    QCOMPARE(max3, QCPCurveData(1.0, 0.0, 3.0));
    QCPCurveData max4 = qcpcurveutils::MaxElement(data, 0, 2);
    QCOMPARE(max4, QCPCurveData(3.0, 2.0, 3.5));
    QCPCurveData max5 = qcpcurveutils::MaxElement(data, 0, 3);
    QCOMPARE(max5, QCPCurveData(3.0, 2.0, 3.5));
}

void TestQCPCurveUtils::TestMinElement() {
    QCPCurveDataContainer data;
    data.add(QCPCurveData(1.0, 0.0, 3.0));
    data.add(QCPCurveData(2.0, 1.0, 1.0));
    data.add(QCPCurveData(3.0, 2.0, -1.5));
    data.add(QCPCurveData(4.0, 3.0, 0.0));
    QCPCurveData min1 = qcpcurveutils::MinElement(data);
    QCOMPARE(min1, QCPCurveData(3.0, 2.0, -1.5));
    QCPCurveData min2 = qcpcurveutils::MinElement(data, 0, 0);
    QCOMPARE(min2, QCPCurveData(1.0, 0.0, 3.0));
    QCPCurveData min3 = qcpcurveutils::MinElement(data, 0, 1);
    QCOMPARE(min3, QCPCurveData(2.0, 1.0, 1.0));
    QCPCurveData min4 = qcpcurveutils::MinElement(data, 0, 2);
    QCOMPARE(min4, QCPCurveData(3.0, 2.0, -1.5));
    QCPCurveData min5 = qcpcurveutils::MinElement(data, 0, 3);
    QCOMPARE(min5, QCPCurveData(3.0, 2.0, -1.5));
}

void TestQCPCurveUtils::TestSmooth() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 3.0));
    data1.add(QCPCurveData(2.0, 1.0, 1.0));
    data1.add(QCPCurveData(3.0, 2.0, 2.0));
    data1.add(QCPCurveData(4.0, 3.0, 0.0));
    data1.add(QCPCurveData(5.0, 4.0, 1.0));
    data1.add(QCPCurveData(6.0, 5.0, 4.0));

    QCPCurveDataContainer smoothed_data1;
    smoothed_data1.add(QCPCurveData(1.0, 0.0, 6.0 / 3.0));
    smoothed_data1.add(QCPCurveData(2.0, 1.0, 6.0 / 4.0));
    smoothed_data1.add(QCPCurveData(3.0, 2.0, 7.0 / 5.0));
    smoothed_data1.add(QCPCurveData(4.0, 3.0, 8.0 / 5.0));
    smoothed_data1.add(QCPCurveData(5.0, 4.0, 7.0 / 4.0));
    smoothed_data1.add(QCPCurveData(6.0, 5.0, 5.0 / 3.0));

    QCPCurveDataContainer s = qcpcurveutils::Smooth(data1, 2);
    QCOMPARE(qcpcurveutils::Smooth(data1, 2), smoothed_data1);
}

void TestQCPCurveUtils::TestValueAtKey() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 3.0));
    data1.add(QCPCurveData(2.0, 1.0, 1.0));
    data1.add(QCPCurveData(3.0, 2.0, 2.0));
    data1.add(QCPCurveData(4.0, 3.0, 0.0));
    data1.add(QCPCurveData(5.0, 4.0, 1.0));
    data1.add(QCPCurveData(6.0, 5.0, 4.0));

    double at1 = qcpcurveutils::ValueAtKey(data1, 0.5);
    QVERIFY(AlmostEqual(at1, 2.0, 0.0001));
    double at2 = qcpcurveutils::ValueAtKey(data1, 0.0);
    QVERIFY(qcpcurveutils::AlmostEqual(at2, 3.0, 0.0001));
    double at3 = qcpcurveutils::ValueAtKey(data1, 3.5);
    QVERIFY(qcpcurveutils::AlmostEqual(at3, 0.5, 0.0001));
    double at4 = qcpcurveutils::ValueAtKey(data1, 4.0);
    QVERIFY(qcpcurveutils::AlmostEqual(at4, 1.0, 0.0001));
    double at5 = qcpcurveutils::ValueAtKey(data1, 5.0);
    QVERIFY(qcpcurveutils::AlmostEqual(at5, 4.0, 0.0001));
    try {
        qcpcurveutils::ValueAtKey(data1, -0.1);
        QFAIL("TestQCPCurveUtils::TestValueAtKey should have thrown1");
    }
    catch (std::exception& exc) {}
    try {
        qcpcurveutils::ValueAtKey(data1, 5.1);
        QFAIL("TestQCPCurveUtils::TestValueAtKey should have thrown2");
    }
    catch (std::exception& exc) {}

}

void TestQCPCurveUtils::TestLeftMostKeyForValue() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 3.0));
    data1.add(QCPCurveData(2.0, 1.0, 1.0));
    data1.add(QCPCurveData(3.0, 2.0, 2.0));
    data1.add(QCPCurveData(4.0, 3.0, 0.0));
    data1.add(QCPCurveData(5.0, 4.0, 1.0));
    data1.add(QCPCurveData(6.0, 5.0, 4.0));

    double key1 = qcpcurveutils::LeftMostKeyForValue(data1, 2.0);
    QVERIFY(qcpcurveutils::AlmostEqual(key1, 0.5, 0.0001));

    double key2 = qcpcurveutils::LeftMostKeyForValue(data1, 3.0);
    QVERIFY(qcpcurveutils::AlmostEqual(key2, 0.0, 0.0001));

    double key3 = qcpcurveutils::LeftMostKeyForValue(data1, 3.5);
    QVERIFY(qcpcurveutils::AlmostEqual(key3, 4.833333, 0.0001));

    double key4 = qcpcurveutils::LeftMostKeyForValue(data1, 4.0);
    QVERIFY(qcpcurveutils::AlmostEqual(key4, 5.0, 0.0001));

    try {
        qcpcurveutils::LeftMostKeyForValue(data1, -0.5);
        QFAIL("TestQCPCurveUtils::TestLestMostKeyForValue Should have thrown1");
    }
    catch (std::exception& exc) {}

    try {
        qcpcurveutils::LeftMostKeyForValue(data1, 4.5);
        QFAIL("TestQCPCurveUtils::TestLestMostKeyForValue Should have thrown2");
    }
    catch (std::exception& exc) {}
}

void TestQCPCurveUtils::TestRightMostKeyForValue() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 3.0));
    data1.add(QCPCurveData(2.0, 1.0, 1.0));
    data1.add(QCPCurveData(3.0, 2.0, 2.0));
    data1.add(QCPCurveData(4.0, 3.0, 0.0));
    data1.add(QCPCurveData(5.0, 4.0, 1.0));
    data1.add(QCPCurveData(6.0, 5.0, 4.0));

    double key1 = qcpcurveutils::RightMostKeyForValue(data1, 2.0);
    QVERIFY(qcpcurveutils::AlmostEqual(key1, 4.33333, 0.0001));

    double key2 = qcpcurveutils::RightMostKeyForValue(data1, 4.0);
    QVERIFY(qcpcurveutils::AlmostEqual(key2, 5.0, 0.0001));

    double key3 = qcpcurveutils::RightMostKeyForValue(data1, 0.5);
    QVERIFY(qcpcurveutils::AlmostEqual(key3, 3.5, 0.0001));

    try {
        qcpcurveutils::RightMostKeyForValue(data1, -0.5);
        QFAIL("TestQCPCurveUtils::TestRightMostKeyForValue Should have thrown1");
    }
    catch (std::exception& exc) {}

    try {
        qcpcurveutils::RightMostKeyForValue(data1, 4.5);
        QFAIL("TestQCPCurveUtils::TestRightMostKeyForValue Should have thrown2");
    }
    catch (std::exception& exc) {}
}

void TestQCPCurveUtils::TestValuesInKeyRange() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 3.0));
    data1.add(QCPCurveData(2.0, 1.0, 1.0));
    data1.add(QCPCurveData(3.0, 2.0, 2.0));
    data1.add(QCPCurveData(4.0, 3.0, 0.0));
    data1.add(QCPCurveData(5.0, 4.0, 1.0));
    data1.add(QCPCurveData(6.0, 5.0, 4.0));

    std::vector<double> w1 = qcpcurveutils::ValuesInKeyRange(data1, 2, 5);
    std::vector<double> e1 { 2.0, 0.0, 1.0, 4.0 };
    QCOMPARE(w1, e1);

    std::vector<double> w2 = qcpcurveutils::ValuesInKeyRange(data1, 5, 2);
    QCOMPARE(w2, e1);

    std::vector<double> w3 = qcpcurveutils::ValuesInKeyRange(data1, 4, 10);
    std::vector<double> e3 { 1.0, 4.0 };
    QCOMPARE(w3, e3);

    std::vector<double> w4 = qcpcurveutils::ValuesInKeyRange(data1, -1, 1);
    std::vector<double> e4 { 3.0, 1.0 };
    QCOMPARE(w4, e4);
}
