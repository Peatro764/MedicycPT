#include <QtTest/QtTest>

class TestUtil : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestSplit();

private:
};

