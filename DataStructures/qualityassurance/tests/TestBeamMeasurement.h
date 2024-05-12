#include <QtTest/QtTest>

#include "BeamMeasurement.h"

class TestBeamMeasurement : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Comparisons();
    void Max();
    void AddPoint();
    void Sort();

    void IntensityCurve();
    void ChambreCurve();
    void NoiseCurve();
    void SetSignalNoise();

    void GetCurrents();
    void GetScaledPoints();
    void GetSignalPoints();
    void GetAxisPoints();
};


