#include <QtTest/QtTest>

class TestDbUtil : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestEncodeIntDoubleMap();
    void TestDecodeIntDoubleMap();
    void TestDecodeDegradeurArray();
    void TestEncodeDegradeurArray();
    void TestDecodeIntArray();
    void TestEncodeIntArray();
    void TestDecodeDoubleArray();
    void TestEncodeDoubleArray();
    void TestQCPCurveDataContainerToPSQLPath();
    void TestPSQLPathToQCPCurveDataContainer();
    void TestPSQLPointToQCPCurveData();
    void TestQCPCurveDataToPSQLPoint();


private:
    double maxRelDiff_;
};

