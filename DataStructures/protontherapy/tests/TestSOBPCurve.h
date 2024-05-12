#include <QtTest/QtTest>

#include "SOBPCurve.h"

class TestSOBPCurve : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Constructor();
    void Curve();
};


