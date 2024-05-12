#include <QtTest/QtTest>

#include "CuveSlice.h"

class TestCuveSlice : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Comparisons();
    void IntegratedSignal();
    void AverageSignal();
    void GetColorMapData();
};


