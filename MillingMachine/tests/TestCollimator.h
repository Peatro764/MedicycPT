#include <QtTest/QtTest>

class TestCollimator : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void CreateCollimatorProgram();
    void EyePlanToMillingProgram();
};


