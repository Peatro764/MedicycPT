#include <QtTest/QtTest>

#include "MeasurementPoint.h"

class TestMeasurementPoint : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void Constructor();
    void ScaleAxis();
    void ScaleIntensity();
    void Translate();
};


