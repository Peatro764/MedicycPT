#include <QtTest/QtTest>

class TestQCPCurveUtils : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestQCPCurveDataOperators();
    void TestQCPCurveDataContainerOperators();
    void TestDiscretize();
    void TestDiscretizeWithErrorScaling();
    void TestScaleValueAxis();
    void TestSetMaxValue();
    void TestCutAt();
    void TestStepCurve();
    void TestMaxElement();
    void TestMinElement();
    void TestSmooth();
    void TestValueAtKey();
    void TestLeftMostKeyForValue();
    void TestRightMostKeyForValue();
    void TestValuesInKeyRange();

private:
    double maxRelDiff_;
};

