#include <QtTest/QtTest>

class TestDosimetryRecord : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Constructor1();
    void Constructor2();
    void GetTotalDuration();
    void GetTotalUMDelivered();
    void Equality();

};


