#include <QtTest/QtTest>

class TestMaterial : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestPlexiToTissue();
    void TestToPlexiglas();
    void TestToWater();
    void TestToTissue();

private:
    double maxRelDiff_;
};

