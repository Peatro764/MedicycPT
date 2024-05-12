#include <QtTest/QtTest>

class TestAlgorithms : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestCalculateIntegratedDose();
    void TestCalculateDebit();
    void TestCalculateHoverD();

private:
};

