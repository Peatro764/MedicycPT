#include <QtTest/QtTest>

class TestUtil : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestInterpolateMonitorUnits();
    void TestExtrapolateMonitorUnits();
    void TestInterpolateDepthDoseCurve();
    void TestExtrapolateDepthDoseCurve();
    void TestDebitDebroca();
    void TestStripperCurrent();
    void TestSeanceDuration();
    void TestStripperCurrentAndSeanceDuration();

private:
    double maxRelDiff_;
};

