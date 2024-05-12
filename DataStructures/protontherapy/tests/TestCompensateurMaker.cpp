#include "TestCompensateurMaker.h"

#include "Calc.h"

void TestCompensateurMaker::Discretize() {
    CompensateurMaker maker;

    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 100.0));
    data1.add(QCPCurveData(2.0, 1.0, 98.0));
    data1.add(QCPCurveData(3.0, 2.0, 97.0));
    data1.add(QCPCurveData(4.0, 3.0, 97.0));
    data1.add(QCPCurveData(5.0, 4.0, 94.0));
    data1.add(QCPCurveData(6.0, 5.0, 93.0));
    data1.add(QCPCurveData(7.0, 6.0, 92.0));

    QCPCurveDataContainer discretized1(maker.Discretize(data1, 2.1));
    QCPCurveDataContainer exp_data1;
    exp_data1.add(QCPCurveData(1.0, 0.0, 100.0));
    exp_data1.add(QCPCurveData(3.0, 2.0, 97.0));
    exp_data1.add(QCPCurveData(5.0, 4.0, 94.0));
    exp_data1.add(QCPCurveData(7.0, 6.0, 92.0));
    QCOMPARE(discretized1, exp_data1);

    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(1.0, 0.0, 0.0));
    data2.add(QCPCurveData(2.0, 1.0, 1.0));
    data2.add(QCPCurveData(3.0, 2.0, 3.0));
    data2.add(QCPCurveData(4.0, 3.0, 5.0));
    data2.add(QCPCurveData(5.0, 4.0, 5.0));
    data2.add(QCPCurveData(6.0, 5.0, 6.0));
    data2.add(QCPCurveData(7.0, 6.0, 8.0));

    QCPCurveDataContainer discretized2(maker.Discretize(data2, 1.5));
    QCPCurveDataContainer exp_data2;
    exp_data2.add(QCPCurveData(1.0, 0.0, 0.0));
    exp_data2.add(QCPCurveData(3.0, 2.0, 3.0));
    exp_data2.add(QCPCurveData(4.0, 3.0, 5.0));
    exp_data2.add(QCPCurveData(7.0, 6.0, 8.0));
    QCOMPARE(discretized2, exp_data2);
}

void TestCompensateurMaker::ScaleValueAxis() {
    QCPCurveDataContainer data;
    data.add(QCPCurveData(1.0, 2.0, 3.0));
    data.add(QCPCurveData(2.0, 3.0, 4.0));
    data.add(QCPCurveData(3.0, 4.0, -4.0));

    CompensateurMaker maker;
    maker.ScaleValueAxis(data, 2.0);

    QCPCurveDataContainer exp_data;
    exp_data.add(QCPCurveData(1.0, 2.0, 6.0));
    exp_data.add(QCPCurveData(2.0, 3.0, 8.0));
    exp_data.add(QCPCurveData(3.0, 4.0, -8.0));

    QCOMPARE(data, exp_data);
}

void TestCompensateurMaker::SetMaxValue() {
    CompensateurMaker maker;

    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 2.0, 3.0));
    data1.add(QCPCurveData(2.0, 3.0, 4.0));
    data1.add(QCPCurveData(3.0, 4.0, -4.0));
    maker.SetMaxValue(data1, -5.0);
    QCPCurveDataContainer exp_data1;
    exp_data1.add(QCPCurveData(1.0, 2.0, -6.0));
    exp_data1.add(QCPCurveData(2.0, 3.0, -5.0));
    exp_data1.add(QCPCurveData(3.0, 4.0, -13.0));
    QCOMPARE(data1, exp_data1);

    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(1.0, 2.0, 3.0));
    data2.add(QCPCurveData(2.0, 3.0, 4.0));
    data2.add(QCPCurveData(3.0, 4.0, -4.0));
    maker.SetMaxValue(data2, 0.0);
    QCPCurveDataContainer exp_data2;
    exp_data2.add(QCPCurveData(1.0, 2.0, -1.0));
    exp_data2.add(QCPCurveData(2.0, 3.0, 0.0));
    exp_data2.add(QCPCurveData(3.0, 4.0, -8.0));
    QCOMPARE(data2, exp_data2);

    QCPCurveDataContainer data3;
    data3.add(QCPCurveData(1.0, 2.0, 3.0));
    data3.add(QCPCurveData(2.0, 3.0, 4.0));
    data3.add(QCPCurveData(3.0, 4.0, -4.0));
    maker.SetMaxValue(data3, 6.0);
    QCPCurveDataContainer exp_data3;
    exp_data3.add(QCPCurveData(1.0, 2.0, 5.0));
    exp_data3.add(QCPCurveData(2.0, 3.0, 6.0));
    exp_data3.add(QCPCurveData(3.0, 4.0, -2.0));
    QCOMPARE(data3, exp_data3);
}

void TestCompensateurMaker::StepCurve() {
    CompensateurMaker maker;

    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(1.0, 0.0, 100.0));
    data1.add(QCPCurveData(2.0, 1.0, 95.0));
    data1.add(QCPCurveData(3.0, 2.0, 90.0));
    data1.add(QCPCurveData(4.0, 3.0, 80.0));
    data1.add(QCPCurveData(5.0, 4.0, 75.0));
    QCPCurveDataContainer step1(maker.StepCurve(data1, 1.0));

    QCPCurveDataContainer exp_data1;
    exp_data1.add(QCPCurveData(1.0, 0.0, 100.0));
    exp_data1.add(QCPCurveData(2.0, 1.0, 100.0));
    exp_data1.add(QCPCurveData(3.0, 1.0, 90.0));
    exp_data1.add(QCPCurveData(4.0, 3.0, 90.0));
    exp_data1.add(QCPCurveData(5.0, 3.0, 75.0));
    exp_data1.add(QCPCurveData(6.0, 4.0, 75.0));

    QCOMPARE(step1, exp_data1);

    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(1.0, 0.0, 100.0));
    data2.add(QCPCurveData(2.0, 1.0, 95.0));
    data2.add(QCPCurveData(3.0, 2.0, 90.0));
    data2.add(QCPCurveData(4.0, 3.0, 80.0));
    QCPCurveDataContainer step2(maker.StepCurve(data2, 1.0));

    QCPCurveDataContainer exp_data2;
    exp_data2.add(QCPCurveData(1.0, 0.0, 100.0));
    exp_data2.add(QCPCurveData(2.0, 1.0, 100.0));
    exp_data2.add(QCPCurveData(3.0, 1.0, 90.0));
    exp_data2.add(QCPCurveData(4.0, 3.0, 90.0));
    exp_data2.add(QCPCurveData(5.0, 3.0, 80.0));

    QCOMPARE(step2, exp_data2);

    // TEST: Never smaller plexi thickness than base_thickness argument
    QCPCurveDataContainer data3;
    data3.add(QCPCurveData(1.0, 0.0, 3.0));
    data3.add(QCPCurveData(2.0, 1.0, 1.0));
    data3.add(QCPCurveData(3.0, 2.0, 0.5));
    data3.add(QCPCurveData(4.0, 3.0, 0.0));

    QCPCurveDataContainer step3a(maker.StepCurve(data3, 1.0));
    QCPCurveDataContainer exp_data3a;
    exp_data3a.add(QCPCurveData(1.0, 0.0, 3.0));
    exp_data3a.add(QCPCurveData(2.0, 1.0, 3.0));
    exp_data3a.add(QCPCurveData(3.0, 1.0, 1.0));
    exp_data3a.add(QCPCurveData(4.0, 3.0, 1.0));
    exp_data3a.add(QCPCurveData(5.0, 3.0, 0.0));

    QCOMPARE(step3a, exp_data3a);

    QCPCurveDataContainer step3b(maker.StepCurve(data3, 0.5));
    QCPCurveDataContainer exp_data3b;
    exp_data3b.add(QCPCurveData(1.0, 0.0, 3.0));
    exp_data3b.add(QCPCurveData(2.0, 1.0, 3.0));
    exp_data3b.add(QCPCurveData(3.0, 1.0, 0.5));
    exp_data3b.add(QCPCurveData(4.0, 3.0, 0.5));
    exp_data3b.add(QCPCurveData(5.0, 3.0, 0.0));

    QCOMPARE(step3b, exp_data3b);
}

void TestCompensateurMaker::GetCompensateur() {
    Eye eye(20.0, 5.0);
    CompensateurMaker maker;
    maker.GetCompensateur(10000, eye, QCPCurveData(0.0, 0.0, 0.0), 4.0, 23.0, 0.2, 1.0);
    qDebug() << "ets";

}

