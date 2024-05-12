#include <QtTest/QtTest>

#include "DepthDoseMeasurement.h"

class TestDepthDoseMeasurement : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    DepthDoseMeasurement GetPlateau(float z_scale);
    DepthDoseMeasurement GetPeak(float z_scale);
    std::vector<MeasurementPoint> GetDataFromFile(QString filename);

    void Comparison();

    void ScaleAxis();
    void ScaleIntensity();

    void TestBraggPeakMeasurements();
    void TestDailySOBPMeasurements();
    void TestInclinedSOBPMeasurements();
};


