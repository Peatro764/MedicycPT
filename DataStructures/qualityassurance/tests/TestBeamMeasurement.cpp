#include "TestBeamMeasurement.h"

#include "BeamMeasurement.h"
#include "Calc.h"
#include "QCPCurveUtils.h"

void TestBeamMeasurement::initTestCase() {}

void TestBeamMeasurement::cleanupTestCase() {}

void TestBeamMeasurement::init() {}

void TestBeamMeasurement::cleanup() {}

void TestBeamMeasurement::Comparisons() {
    std::vector<MeasurementPoint> no_points;
    MeasurementCurrents c1(QDateTime::currentDateTime(), 1.1, 2.2, 3.3, 4.4, 5.5);
    MeasurementCurrents c2(QDateTime::currentDateTime(), 1.1, 2.3, 3.3, 4.4, 5.5);
    QVERIFY(BeamMeasurement(QA_HARDWARE::SCANNER2D, no_points, QDateTime(QDate(2017, 1, 1)), c1, 0.0) ==
            BeamMeasurement(QA_HARDWARE::SCANNER2D, no_points, QDateTime(QDate(2017, 1, 1)), c1, 0.0));
    QVERIFY(BeamMeasurement(QA_HARDWARE::SCANNER2D, no_points, QDateTime(QDate(2017, 1, 1)), c1, 0.0) !=
            BeamMeasurement(QA_HARDWARE::WHEEL, no_points, QDateTime(QDate(2017, 1, 1)), c1, 0.0));
    QVERIFY(BeamMeasurement(QA_HARDWARE::SCANNER2D, no_points, QDateTime(QDate(2017, 1, 1)), c1, 0.0) !=
            BeamMeasurement(QA_HARDWARE::SCANNER2D, no_points, QDateTime(QDate(2017, 1, 1)), c2, 0.0));

    std::vector<MeasurementPoint> one_point_a = { MeasurementPoint(Point(1.0, 2.0, 3.0), BeamSignal(4.0, 3.0)) };
    std::vector<MeasurementPoint> one_point_b = { MeasurementPoint(Point(1.0, 3.0, 3.0), BeamSignal(2.0, 3.0)) };
    std::vector<MeasurementPoint> two_points = { MeasurementPoint(Point(1.0, 2.0, 3.0), BeamSignal(4.0, 3.0)),
                                                   MeasurementPoint(Point(2.0, 2.0, 3.0), BeamSignal(2.0, 3.0)) };

    QVERIFY(BeamMeasurement(QA_HARDWARE::SCANNER2D, one_point_a, QDateTime(QDate(2017, 1, 1)), c1, 0.0) ==
            BeamMeasurement(QA_HARDWARE::SCANNER2D, one_point_a, QDateTime(QDate(2017, 1, 1)), c1, 0.0));
    QVERIFY(BeamMeasurement(QA_HARDWARE::SCANNER2D, one_point_a, QDateTime(QDate(2017, 1, 1)), c1, 0.0) !=
            BeamMeasurement(QA_HARDWARE::SCANNER2D, one_point_b, QDateTime(QDate(2017, 1, 1)), c1, 0.0));
    QVERIFY(BeamMeasurement(QA_HARDWARE::SCANNER2D, two_points, QDateTime(QDate(2017, 1, 1)), c1, 0.0) ==
            BeamMeasurement(QA_HARDWARE::SCANNER2D, two_points, QDateTime(QDate(2017, 1, 1)), c1, 0.0));
    QVERIFY(BeamMeasurement(QA_HARDWARE::SCANNER2D, two_points, QDateTime(QDate(2017, 1, 1)), c1, 0.0) !=
            BeamMeasurement(QA_HARDWARE::SCANNER2D, one_point_a, QDateTime(QDate(2017, 1, 1)), c1, 0.0));
}

void TestBeamMeasurement::Sort() {
    BeamMeasurement m;
    m << MeasurementPoint(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 3.0));
    m << MeasurementPoint(Point(3.0, 2.0, 4.0), BeamSignal(2.0, 3.0));
    m << MeasurementPoint(Point(2.0, 3.0, 1.0), BeamSignal(3.0, 3.0));
    BeamMeasurement m_sorted_x;
    m_sorted_x << MeasurementPoint(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 3.0));
    m_sorted_x << MeasurementPoint(Point(2.0, 3.0, 1.0), BeamSignal(3.0, 3.0));
    m_sorted_x << MeasurementPoint(Point(3.0, 2.0, 4.0), BeamSignal(2.0, 3.0));
    BeamMeasurement m_sorted_y;
    m_sorted_y << MeasurementPoint(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 3.0));
    m_sorted_y << MeasurementPoint(Point(3.0, 2.0, 4.0), BeamSignal(2.0, 3.0));
    m_sorted_y << MeasurementPoint(Point(2.0, 3.0, 1.0), BeamSignal(3.0, 3.0));
    BeamMeasurement m_sorted_z;
    m_sorted_z << MeasurementPoint(Point(2.0, 3.0, 1.0), BeamSignal(3.0, 3.0));
    m_sorted_z << MeasurementPoint(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 3.0));
    m_sorted_z << MeasurementPoint(Point(3.0, 2.0, 4.0), BeamSignal(2.0, 3.0));

    m.Sort(Axis::X);
    QCOMPARE(m, m_sorted_x);
    m.Sort(Axis::Y);
    QCOMPARE(m, m_sorted_y);
    m.Sort(Axis::Z);
    QCOMPARE(m, m_sorted_z);
}

void TestBeamMeasurement::Max() {
    BeamMeasurement m;
    m << MeasurementPoint(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 1.0));
    m << MeasurementPoint(Point(3.0, 2.0, 4.0), BeamSignal(2.0, 1.0));
    m << MeasurementPoint(Point(2.0, 3.0, 1.0), BeamSignal(3.0, 1.0));
    QCOMPARE(m.Max(), MeasurementPoint(Point(2.0, 3.0, 1.0), BeamSignal(3.0, 1.0)));
}

void TestBeamMeasurement::AddPoint() {
    MeasurementPoint p1(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 1.0));
    MeasurementPoint p2(Point(2.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p3(Point(3.0, 1.0, 3.0), BeamSignal(4.0, 1.0));

    std::vector<MeasurementPoint> v1 { p1 };
    std::vector<MeasurementPoint> v2 { p1, p2 };
    std::vector<MeasurementPoint> v3 { p1, p2, p3 };

    BeamMeasurement m;
    m << p1;
    QCOMPARE(m.GetPoints(), v1);
    m << p2;
    QCOMPARE(m.GetPoints(), v2);
    m << p3;
    QCOMPARE(m.GetPoints(), v3);
}

void TestBeamMeasurement::IntensityCurve() {
    MeasurementPoint p1(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 1.0));
    MeasurementPoint p2(Point(2.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p3(Point(3.0, 1.0, 3.0), BeamSignal(4.0, 2.0));
    std::vector<MeasurementPoint> v { p1, p2, p3 };

    BeamMeasurement m(QA_HARDWARE::UNK, v, 0.0);

    QCPCurveDataContainer act_intensity_x = m.GetIntensityCurve(Axis::X);
    QCPCurveDataContainer exp_intensity_x;
    exp_intensity_x.add(QCPCurveData(0, 1.0, 1.0));
    exp_intensity_x.add(QCPCurveData(1, 2.0, 3.0));
    exp_intensity_x.add(QCPCurveData(2, 3.0, 2.0));
    QCOMPARE(act_intensity_x, exp_intensity_x);

    QCPCurveDataContainer act_intensity_y = m.GetIntensityCurve(Axis::Y);
    QCPCurveDataContainer exp_intensity_y;
    exp_intensity_y.add(QCPCurveData(0, 1.0, 1.0));
    exp_intensity_y.add(QCPCurveData(1, 1.0, 3.0));
    exp_intensity_y.add(QCPCurveData(2, 1.0, 2.0));
    QCOMPARE(act_intensity_y, exp_intensity_y);

    QCPCurveDataContainer act_intensity_z = m.GetIntensityCurve(Axis::Z);
    QCPCurveDataContainer exp_intensity_z;
    exp_intensity_z.add(QCPCurveData(0, 3.0, 1.0));
    exp_intensity_z.add(QCPCurveData(1, 3.0, 3.0));
    exp_intensity_z.add(QCPCurveData(2, 3.0, 2.0));
    QCOMPARE(act_intensity_z, exp_intensity_z);
}

void TestBeamMeasurement::ChambreCurve() {
    MeasurementPoint p1(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 1.0));
    MeasurementPoint p2(Point(2.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p3(Point(3.0, 1.0, 3.0), BeamSignal(4.0, 2.0));
    std::vector<MeasurementPoint> v { p1, p2, p3 };

    BeamMeasurement m(QA_HARDWARE::UNK,v, 0.0);

    QCPCurveDataContainer act_chambre_x = m.GetChambreCurve(Axis::X);
    QCPCurveDataContainer exp_chambre_x;
    exp_chambre_x.add(QCPCurveData(0, 1.0, 1.0));
    exp_chambre_x.add(QCPCurveData(1, 2.0, 1.0));
    exp_chambre_x.add(QCPCurveData(2, 3.0, 2.0));
    QCOMPARE(act_chambre_x, exp_chambre_x);

    QCPCurveDataContainer act_chambre_y = m.GetChambreCurve(Axis::Y);
    QCPCurveDataContainer exp_chambre_y;
    exp_chambre_y.add(QCPCurveData(0, 1.0, 1.0));
    exp_chambre_y.add(QCPCurveData(1, 1.0, 1.0));
    exp_chambre_y.add(QCPCurveData(2, 1.0, 2.0));
    QCOMPARE(act_chambre_y, exp_chambre_y);

    QCPCurveDataContainer act_chambre_z = m.GetChambreCurve(Axis::Z);
    QCPCurveDataContainer exp_chambre_z;
    exp_chambre_z.add(QCPCurveData(0, 3.0, 1.0));
    exp_chambre_z.add(QCPCurveData(1, 3.0, 1.0));
    exp_chambre_z.add(QCPCurveData(2, 3.0, 2.0));
    QCOMPARE(act_chambre_z, exp_chambre_z);
}

void TestBeamMeasurement::NoiseCurve() {
    MeasurementPoint p1(Point(1.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p2(Point(2.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p3(Point(3.0, 1.0, 3.0), BeamSignal(4.0, 2.0));
    std::vector<MeasurementPoint> v { p1, p2, p3 };

    BeamMeasurement m(QA_HARDWARE::UNK, v, 0.4);

    QCPCurveDataContainer act_noise_x = m.GetSignalNoiseCurve(Axis::X);
    QCPCurveDataContainer exp_noise_x;
    exp_noise_x.add(QCPCurveData(0, 1.0, 0.4));
    exp_noise_x.add(QCPCurveData(1, 2.0, 0.4));
    exp_noise_x.add(QCPCurveData(2, 3.0, 0.4));
    QCOMPARE(act_noise_x, exp_noise_x);

    QCPCurveDataContainer act_noise_y = m.GetSignalNoiseCurve(Axis::Y);
    QCPCurveDataContainer exp_noise_y;
    exp_noise_y.add(QCPCurveData(0, 1.0, 0.4));
    exp_noise_y.add(QCPCurveData(1, 1.0, 0.4));
    exp_noise_y.add(QCPCurveData(2, 1.0, 0.4));
    QCOMPARE(act_noise_y, exp_noise_y);

    QCPCurveDataContainer act_noise_z = m.GetSignalNoiseCurve(Axis::Z);
    QCPCurveDataContainer exp_noise_z;
    exp_noise_z.add(QCPCurveData(0, 3.0, 0.4));
    exp_noise_z.add(QCPCurveData(1, 3.0, 0.4));
    exp_noise_z.add(QCPCurveData(2, 3.0, 0.4));
    QCOMPARE(act_noise_z, exp_noise_z);

}

void TestBeamMeasurement::SetSignalNoise() {
    MeasurementPoint p1(Point(1.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p2(Point(2.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p3(Point(3.0, 1.0, 3.0), BeamSignal(4.0, 2.0));
    std::vector<MeasurementPoint> v { p1, p2, p3 };

    BeamMeasurement m(QA_HARDWARE::UNK, v, 0.3);

    m.SetSignalNoise(2.0);
    QCPCurveDataContainer act_noise = m.GetSignalNoiseCurve(Axis::X);
    QCPCurveDataContainer exp_noise;
    exp_noise.add(QCPCurveData(0, 1.0, 2.0));
    exp_noise.add(QCPCurveData(1, 2.0, 2.0));
    exp_noise.add(QCPCurveData(2, 3.0, 2.0));

    QCOMPARE(act_noise, exp_noise);
}

void TestBeamMeasurement::GetCurrents() {
    BeamMeasurement m;

    QDateTime t1 = QDateTime::currentDateTime();
    QDateTime t2 = t1.addDays(1);
    MeasurementCurrents c1(t1, 1.1, 2.2, 3.3, 4.4, 5.5);
    MeasurementCurrents c2(t2, 1.1, 2.2, 3.3, 4.4, 5.5);
    MeasurementCurrents c3(t1, 1.2, 2.2, 3.3, 4.4, 5.5);
    MeasurementCurrents c4(t1, 1.1, 2.3, 3.3, 4.4, 5.5);
    MeasurementCurrents c5(t1, 1.1, 2.2, 3.4, 4.4, 5.5);
    MeasurementCurrents c6(t1, 1.1, 2.2, 3.3, 4.5, 5.5);
    MeasurementCurrents c7(t1, 1.1, 2.2, 3.3, 4.4, 5.6);

    m.SetCurrents(c1);
    QVERIFY(c1 == m.GetCurrents());
    QVERIFY(c2 != m.GetCurrents());
    QVERIFY(c3 != m.GetCurrents());
    QVERIFY(c4 != m.GetCurrents());
    QVERIFY(c5 != m.GetCurrents());
    QVERIFY(c6 != m.GetCurrents());
    QVERIFY(c7 != m.GetCurrents());
}

void TestBeamMeasurement::GetScaledPoints() {
    MeasurementPoint p1(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 1.0));
    MeasurementPoint p2(Point(2.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p3(Point(3.0, 1.0, 3.0), BeamSignal(4.0, 2.0));
    std::vector<MeasurementPoint> v { p1, p2, p3 };
    BeamMeasurement m(QA_HARDWARE::UNK, v, 0.0);

    std::vector<MeasurementPoint> v_exp { p1.ScaleAxis(Axis::Z, 2.0), p2.ScaleAxis(Axis::Z, 2.0), p3.ScaleAxis(Axis::Z, 2.0) };

    std::vector<MeasurementPoint> v_scaled = m.GetScaledPoints(Axis::Z, 2.0);
    QCOMPARE(v_scaled, v_exp);
    QVERIFY(v_scaled != v);
}

void TestBeamMeasurement::GetSignalPoints() {
    MeasurementPoint p1(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 1.0));
    MeasurementPoint p2(Point(2.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p3(Point(3.0, 1.0, 3.0), BeamSignal(4.0, 2.0));
    std::vector<MeasurementPoint> v { p1, p2, p3 };
    BeamMeasurement m(QA_HARDWARE::UNK, v, 1.0);
    std::vector<double> exp_points { 0.0, 2.0, 1.5 };
    QVERIFY(calc::AlmostEqual(exp_points, m.GetSignalValues(), 0.0001));
}

void TestBeamMeasurement::GetAxisPoints() {
    MeasurementPoint p1(Point(1.0, 1.0, 3.0), BeamSignal(1.0, 1.0));
    MeasurementPoint p2(Point(2.0, 1.0, 3.0), BeamSignal(3.0, 1.0));
    MeasurementPoint p3(Point(3.0, 1.0, 3.0), BeamSignal(4.0, 2.0));
    std::vector<MeasurementPoint> v { p1, p2, p3 };
    BeamMeasurement m(QA_HARDWARE::UNK, v, 1.0);
    std::vector<double> exp_points { 1.0, 2.0, 3.0 };
    QVERIFY(calc::AlmostEqual(exp_points, m.GetAxisValues(Axis::X), 0.0001));
}


