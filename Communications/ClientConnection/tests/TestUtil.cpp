#include "TestUtil.h"
#include "Util.h"

#include <iostream>
#include <QDateTime>


void TestUtil::initTestCase() {}

void TestUtil::cleanupTestCase() {}

void TestUtil::init() {}

void TestUtil::cleanup() {}

void TestUtil::TestSplit() {
    QByteArray b0("ABC");
    std::vector<QByteArray> exp0 { QByteArray("ABC") };
    QCOMPARE(util::Split(b0, QByteArray("")), exp0);

    QByteArray b1("");
    std::vector<QByteArray> exp1 { QByteArray("") };
    QCOMPARE(util::Split(b1, QByteArray("A")), exp1);

    QByteArray b2("AB");
    std::vector<QByteArray> exp2 { QByteArray("AB") };
    QCOMPARE(util::Split(b2, QByteArray("ABC")), exp2);

    QByteArray b3("ABC");
    std::vector<QByteArray> exp3 { QByteArray("ABC") };
    QCOMPARE(util::Split(b3, QByteArray("ABC")), exp3);

    QByteArray b4("123ABC");
    std::vector<QByteArray> exp4{ QByteArray("123ABC") };
    QCOMPARE(util::Split(b4, QByteArray("ABC")), exp4);

    QByteArray b5("ABC123ABC");
    std::vector<QByteArray> exp5{ QByteArray("ABC"), QByteArray("123ABC") };
    QCOMPARE(util::Split(b5, QByteArray("ABC")), exp5);

    QByteArray b6("123ABC456ABC");
    std::vector<QByteArray> exp6{ QByteArray("123ABC"), QByteArray("456ABC") };
    QCOMPARE(util::Split(b6, QByteArray("ABC")), exp6);

    QByteArray b7("123ABC456ABC7891ABC");
    std::vector<QByteArray> exp7{ QByteArray("123ABC"), QByteArray("456ABC"), QByteArray("7891ABC") };
    QCOMPARE(util::Split(b7, QByteArray("ABC")), exp7);
}
