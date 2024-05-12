#include <QtTest/QtTest>

#include "SeanceConfig.h"

class TestSeanceConfig : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestByteArray();
    void TestUnPack();
    void TestConstructor();
    void TestEquality();
    void TestSetters();
    void TestChangeUM();
    void TestBelongsTo();
    void TestConvert();

private:
    QByteArray GetByteArray() const;

};
