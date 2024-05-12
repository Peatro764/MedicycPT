#include <QtTest/QtTest>

#include "SeancePacket.h"

class TestSeancePacket : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestByteArray();
    void TestUnPack();
    void TestEquality();
    void TestSameSeance();

private:
    QByteArray GetByteArray() const;

};
