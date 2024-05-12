#include "TestDepthDoseMeasurement.h"

#include "QCPCurveUtils.h"
#include "Calc.h"

void TestDepthDoseMeasurement::initTestCase() {}

void TestDepthDoseMeasurement::cleanupTestCase() {}

void TestDepthDoseMeasurement::init() {}

void TestDepthDoseMeasurement::cleanup() {}

std::vector<MeasurementPoint> TestDepthDoseMeasurement::GetDataFromFile(QString filename) {
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestDepthDoseMeasurement::GetDataFromFile Could not open file: " << filename;
        return std::vector<MeasurementPoint>();
    }

    file.readLine(); // read header
    std::vector<MeasurementPoint> points;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QList<QByteArray> list = line.split(',');
        qDebug() << list;
        MeasurementPoint p = MeasurementPoint(Point(0, 0, list.at(0).trimmed().toDouble()), BeamSignal(list.at(2).trimmed().toDouble(), 1.0));
        points.push_back(p);
        qDebug() << "MeasurementPoint " << p.toString();
    }

    return points;
}

DepthDoseMeasurement TestDepthDoseMeasurement::GetPlateau(float z_scale) {
    std::vector<MeasurementPoint> p;
    p.push_back(MeasurementPoint(Point(0, 0, 1*z_scale), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 2*z_scale), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 3*z_scale), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 4*z_scale), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 5*z_scale), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 7*z_scale), BeamSignal(5, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 8*z_scale), BeamSignal(6, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 9*z_scale), BeamSignal(7, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 10*z_scale), BeamSignal(8, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 11*z_scale), BeamSignal(9, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 12*z_scale), BeamSignal(10, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 13*z_scale), BeamSignal(8, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 14*z_scale), BeamSignal(6, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 15*z_scale), BeamSignal(4, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 16*z_scale), BeamSignal(2, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 17*z_scale), BeamSignal(0, 1)));

    return DepthDoseMeasurement(QA_HARDWARE::UNK, p, 0.0);
}

DepthDoseMeasurement TestDepthDoseMeasurement::GetPeak(float z_scaling) {
    std::vector<MeasurementPoint> p;
    p.push_back(MeasurementPoint(Point(0, 0, 1*z_scaling), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 2*z_scaling), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 3*z_scaling), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 4*z_scaling), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 5*z_scaling), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 7*z_scaling), BeamSignal(5, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 8*z_scaling), BeamSignal(6, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 9*z_scaling), BeamSignal(7, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 10*z_scaling), BeamSignal(8, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 11*z_scaling), BeamSignal(9, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 12*z_scaling), BeamSignal(10, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 13*z_scaling), BeamSignal(8, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 14*z_scaling), BeamSignal(6, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 15*z_scaling), BeamSignal(4, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 16*z_scaling), BeamSignal(2, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 17*z_scaling), BeamSignal(0, 1)));

    return DepthDoseMeasurement(QA_HARDWARE::UNK, p, 0.0);
}

void TestDepthDoseMeasurement::Comparison() {
    std::vector<MeasurementPoint> p1;
    p1.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(0, 1)));

    std::vector<MeasurementPoint> p2;
    p2.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(0, 1)));
    p2.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(0, 1)));

    DepthDoseMeasurement d1(QA_HARDWARE::UNK, p1, 0.0);
    DepthDoseMeasurement d2(QA_HARDWARE::UNK, p2, 0.0);
    QCOMPARE(d1, d1);
    QCOMPARE(d2, d2);
    QVERIFY(d1 != d2);

    DepthDoseMeasurement d3(QA_HARDWARE::SCANNER2D, p1, 0.0);
    DepthDoseMeasurement d4(QA_HARDWARE::UNK, p1, 1.0);
    QVERIFY(d1 != d3);
    QVERIFY(d1 != d4);
}

void TestDepthDoseMeasurement::ScaleAxis() {
    auto dd1 = GetPlateau(1.0);
    auto dd2 = GetPlateau(2.0);
    auto dd1_scaled = dd1.ScaleAxis(Axis::Z, 2.0);
    QCOMPARE(dd2, dd1_scaled);
    QVERIFY(dd1 != dd2);
}

void TestDepthDoseMeasurement::ScaleIntensity() {
    std::vector<MeasurementPoint> p1;
    p1.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(4, 2)));
    p1.push_back(MeasurementPoint(Point(0, 0, 2), BeamSignal(6, 1)));
    p1.push_back(MeasurementPoint(Point(0, 0, 3), BeamSignal(9, 2)));
    DepthDoseMeasurement dd1(QA_HARDWARE::UNK, p1, 1.0);

    std::vector<MeasurementPoint> p2;
    p2.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(7, 2)));
    p2.push_back(MeasurementPoint(Point(0, 0, 2), BeamSignal(11, 1)));
    p2.push_back(MeasurementPoint(Point(0, 0, 3), BeamSignal(17, 2)));
    DepthDoseMeasurement dd2(QA_HARDWARE::UNK, p2, 1.0);

    DepthDoseMeasurement dd1_scaled = dd1.ScaleIntensity(10);
    QCOMPARE(dd1_scaled, dd2);
    QVERIFY(dd1 != dd2);
}

void TestDepthDoseMeasurement::TestBraggPeakMeasurements() {
    std::vector<MeasurementPoint> data = GetDataFromFile(":/data/braggpeak.csv");
    DepthDoseMeasurement d(QA_HARDWARE::WHEEL, data, 0.0);
    DepthDoseResults r = d.GetResults();
    QVERIFY(calc::AlmostEqual(r.parcours(), 30.64, 0.0001));
    QVERIFY(calc::AlmostEqual(r.width50(), 3.44, 0.001));
    QVERIFY(calc::AlmostEqual(r.penumbra(), 0.886, 0.001));
    QVERIFY(r.IsBraggPeak());
}

void TestDepthDoseMeasurement::TestDailySOBPMeasurements() {
    std::vector<MeasurementPoint> data = GetDataFromFile(":/data/sobp_10000_norangeshifter.csv");
    DepthDoseMeasurement d(QA_HARDWARE::WHEEL, data, 0.0);
    DepthDoseResults r = d.GetResults();
    QVERIFY(calc::AlmostEqual(r.parcours(), 30.43, 0.0001));
    QVERIFY(calc::AlmostEqual(r.width50(), 28.98, 0.001));
    QVERIFY(calc::AlmostEqual(r.penumbra(), 1.061, 0.001));
    QVERIFY(calc::AlmostEqual(r.mod98(), 15.66, 0.001));
    QVERIFY(calc::AlmostEqual(r.mod100(), 14.90, 0.001));
    QVERIFY(!r.IsBraggPeak());
}

void TestDepthDoseMeasurement::TestInclinedSOBPMeasurements() {
    std::vector<MeasurementPoint> data = GetDataFromFile(":/data/sobp_1084.csv");
    DepthDoseMeasurement d(QA_HARDWARE::WHEEL, data, 0.0);
    DepthDoseResults r = d.GetResults();
    QVERIFY(calc::AlmostEqual(r.parcours(), 22.86, 0.0001));
    QVERIFY(calc::AlmostEqual(r.width50(), 21.39, 0.001));
    QVERIFY(calc::AlmostEqual(r.penumbra(), 1.049, 0.001));
    QVERIFY(calc::AlmostEqual(r.mod98(), 11.133, 0.001));
    QVERIFY(calc::AlmostEqual(r.mod100(), 10.452, 0.001));
    QVERIFY(!r.IsBraggPeak());
}
