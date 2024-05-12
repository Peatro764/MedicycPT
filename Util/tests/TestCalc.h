#include <QtTest/QtTest>

class TestCalc : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestAlmostEqual();
    void TestMean();
    void TestMedian();
    void TestStdDev();

    void TestSquare();
    void TestCenterOfGravity();
    void TestPearson();
    void TestTStatistic();
    void TestLinearFit();

    void TestFiltered();
    void TestDoseToUM();
    void TestUM2MM();
    void TestMM2UM();

private:
    double maxRelDiff_;
};

