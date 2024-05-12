#include "TestDbUtil.h"

#include <iostream>
#include <QDateTime>
#include <QDebug>

#include "QCPCurveUtils.h"
#include "Calc.h"
#include "DbUtil.h"
#include "Modulateur.h"
#include "Degradeur.h"

void TestDbUtil::initTestCase() {
    maxRelDiff_ = 0.001;
}

void TestDbUtil::cleanupTestCase() {}

void TestDbUtil::init() {}

void TestDbUtil::cleanup() {}

void TestDbUtil::TestDecodeDegradeurArray() {
    QString a1("{}");
    QStringList r1;
    QCOMPARE(dbutil::DecodeDegradeurArray(a1), r1);

    QString a2("{33}");
    QStringList r2;
    r2 << "33";
    QCOMPARE(dbutil::DecodeDegradeurArray(a2), r2);

    QString a3("{33,44}");
    QStringList r3;
    r3 << "33" << "44";
    QCOMPARE(dbutil::DecodeDegradeurArray(a3), r3);
}

void TestDbUtil::TestEncodeDegradeurArray() {
    std::vector<Degradeur> empty;
    QCOMPARE(dbutil::EncodeDegradeurArray(empty), QString("{}"));

    std::vector<Degradeur> one { Degradeur(QString("2"), 800, true) };
    QCOMPARE(dbutil::EncodeDegradeurArray(one), QString("{2}"));

    std::vector<Degradeur> multiple { Degradeur(QString("2"), 800, true), Degradeur(QString("5"), 800, true) };
    QCOMPARE(dbutil::EncodeDegradeurArray(multiple), QString("{2,5}"));
}

void TestDbUtil::TestDecodeIntArray() {
    // standard cases
    QString a1("{}");
    QCOMPARE(dbutil::DecodeIntArray(a1), std::vector<int>());

    std::vector<int> r2 {3};
    QString a2("{3}");
    QCOMPARE(dbutil::DecodeIntArray(a2), r2);

    QString a3("{4,3}");
    std::vector<int> r3 {4,3};
    QCOMPARE(dbutil::DecodeIntArray(a3), r3);

    QString a4("{4,5,6}");
    std::vector<int> r4 {4,5,6};
    QCOMPARE(dbutil::DecodeIntArray(a4), r4);

    QString a4b("{400,5000,60000}");
    std::vector<int> r4b {400,5000,60000};
    QCOMPARE(dbutil::DecodeIntArray(a4b), r4b);

    // error cases
    QString a5("{1.1,4.4}");
    std::vector<int> r5 {0, 0};
    QCOMPARE(dbutil::DecodeIntArray(a5), r5);

    QString a6("1, 4");
    std::vector<int> r6 {1, 4};
    QCOMPARE(dbutil::DecodeIntArray(a6), r6);
}

void TestDbUtil::TestEncodeIntArray() {
    std::vector<int> r1;
    QCOMPARE(dbutil::EncodeIntArray(r1), QString("{}"));

    std::vector<int> r2 {2};
    QCOMPARE(dbutil::EncodeIntArray(r2), QString("{2}"));

    std::vector<int> r3 {3, 4};
    QCOMPARE(dbutil::EncodeIntArray(r3), QString("{3,4}"));

    std::vector<int> r4 {3, 4, 7};
    QCOMPARE(dbutil::EncodeIntArray(r4), QString("{3,4,7}"));

    std::vector<int> r5 {300, 4000, 70000};
    QCOMPARE(dbutil::EncodeIntArray(r5), QString("{300,4000,70000}"));
}

void TestDbUtil::TestDecodeDoubleArray() {
    // standard cases
    QString a1("{}");
    QCOMPARE(dbutil::DecodeDoubleArray(a1), std::vector<double>());

    QString a2("{3.14156}");
    std::vector<double> r2 {3.141560};
    QVERIFY(calc::AlmostEqual(dbutil::DecodeDoubleArray(a2), r2, 0.000001));

    QString a3("{4382829.1234,345666.73728256}");
    std::vector<double> r3 {4382829.1234,345666.737283};
    QVERIFY(calc::AlmostEqual(dbutil::DecodeDoubleArray(a3), r3, 0.0000000001));

    QString a4("{433.23347262,5.223,6292929.22}");
    std::vector<double> r4 {433.233473,5.223000,6292929.220000};
    QVERIFY(calc::AlmostEqual(dbutil::DecodeDoubleArray(a4), r4, 0.00000001));

    QString a5("{433,5,3393923}");
    std::vector<double> r5 {433.000000, 5.000000, 3393923.000000};
    QVERIFY(calc::AlmostEqual(dbutil::DecodeDoubleArray(a5), r5, 0.0001));

    // error cases
    QString a6("{1.1, 4.4}");
    std::vector<double> r6 {1.100000, 4.400000};
    QVERIFY(calc::AlmostEqual(dbutil::DecodeDoubleArray(a6), r6, 0.0001));
}

void TestDbUtil::TestEncodeDoubleArray() {
    std::vector<double> r1;
    QCOMPARE(dbutil::EncodeDoubleArray(r1), QString("{}"));

    std::vector<double> r2 {2.2828289};
    QCOMPARE(dbutil::EncodeDoubleArray(r2), QString("{2.282829}"));

    std::vector<double> r3 {329499.292, 4.92999292};
    QCOMPARE(dbutil::EncodeDoubleArray(r3), QString("{329499.292000,4.929993}"));

    std::vector<double> r4 {32223444.1, 4.2, 7};
    QCOMPARE(dbutil::EncodeDoubleArray(r4), QString("{32223444.100000,4.200000,7.000000}"));
}

void TestDbUtil::TestQCPCurveDataContainerToPSQLPath() {
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(0, 1.1, 10.10));
    data1.add(QCPCurveData(0, 2.2, 20.20));
    data1.add(QCPCurveData(0, 3.3, 30.30));
    data1.add(QCPCurveData(0, 4.4, 40.40));
    QCOMPARE(dbutil::QCPCurveDataContainerToPSQLPath(data1),
             QString("((1.1000,10.1000),(2.2000,20.2000),(3.3000,30.3000),(4.4000,40.4000))"));

    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(0, 1.1, 10.10));
    QCOMPARE(dbutil::QCPCurveDataContainerToPSQLPath(data2),
             QString("((1.1000,10.1000))"));

    QCPCurveDataContainer data3;
    QCOMPARE(dbutil::QCPCurveDataContainerToPSQLPath(data3),
             QString("()"));
}

void TestDbUtil::TestPSQLPathToQCPCurveDataContainer() {
    QString psql_path1("((1.1000,10.1000),(2.2000,20.2000),(3.3000,30.3000),(4.4000,40.4000))");
    QCPCurveDataContainer data1;
    data1.add(QCPCurveData(0, 1.1, 10.10));
    data1.add(QCPCurveData(0, 2.2, 20.20));
    data1.add(QCPCurveData(0, 3.3, 30.30));
    data1.add(QCPCurveData(0, 4.4, 40.40));
    QCOMPARE(data1, dbutil::PSQLPathToQCPCurveDataContainer(psql_path1));

    QString psql_path2("((1.1000,10.1000))");
    QCPCurveDataContainer data2;
    data2.add(QCPCurveData(0, 1.1, 10.10));
    QCOMPARE(data2, dbutil::PSQLPathToQCPCurveDataContainer(psql_path2));

    try {
        dbutil::PSQLPathToQCPCurveDataContainer(QString("()"));
        QFAIL("TestDbUtil::TestPSQLPathToQCPCurveDataContainer1");
    }
    catch(...) {}

    try {
        dbutil::PSQLPathToQCPCurveDataContainer("((3.1,A),(4.4,3.3))");
        QFAIL("TestDbUtil::TestPSQLPathToQCPCurveDataContainer2");
    }
    catch(...) {}
}

void TestDbUtil::TestPSQLPointToQCPCurveData() {
    QString psql_point1("(1.1000,10.1000)");
    QCOMPARE(QCPCurveData(0, 1.1000, 10.1000), dbutil::PSQLPointToQCPCurveData(psql_point1));

    try {
        dbutil::PSQLPointToQCPCurveData(QString("()"));
        QFAIL("TestDbUtil::TestPSQLPointToQCPCurve1");
    }
    catch (...) {}

    try {
        dbutil::PSQLPointToQCPCurveData(QString("(A,3.3)"));
        QFAIL("TestDbUtil::TestPSQLPointToQCPCurve2");
    }
    catch (...) {}

}

void TestDbUtil::TestQCPCurveDataToPSQLPoint() {
    QCPCurveData data1(0, 1.1, 10.10);
    QCOMPARE(QString("(1.1000,10.1000)"), dbutil::QCPCurveDataToPSQLPoint(data1));
}

void TestDbUtil::TestEncodeIntDoubleMap() {
    std::map<int, double> map1;
    map1.emplace(0, 1.0);
    map1.emplace(800, 0.4);
    map1.emplace(1600, 0.35);
    map1.emplace(2400, 0.3);

    QString encoded_map1(dbutil::EncodeIntDoubleMap(map1));
    QCOMPARE(encoded_map1, QString("{\"(0,1.000)\",\"(800,0.400)\",\"(1600,0.350)\",\"(2400,0.300)\"}"));

    std::map<int, double> map2;
    map2.emplace(0, 1.0);

    QString encoded_map2(dbutil::EncodeIntDoubleMap(map2));
    QCOMPARE(encoded_map2, QString("{\"(0,1.000)\"}"));
}

void TestDbUtil::TestDecodeIntDoubleMap() {
    QString encoded_map1("{\"(0,1.000)\",\"(800,0.400)\",\"(1600,0.350)\",\"(2400,0.300)\"}");
    std::map<int, double> exp_decoded_map1;
    exp_decoded_map1.emplace(0, 1.0);
    exp_decoded_map1.emplace(800, 0.4);
    exp_decoded_map1.emplace(1600, 0.35);
    exp_decoded_map1.emplace(2400, 0.3);

    std::map<int, double> decoded_map1(dbutil::DecodeIntDoubleMap(encoded_map1));
    QCOMPARE(exp_decoded_map1.size(), decoded_map1.size());
    for (auto item: decoded_map1) {
        QVERIFY(calc::AlmostEqual(item.second, exp_decoded_map1.at(item.first), 0.0001));
    }

    QString encoded_map2("{\"(0,1.000)\"}");
    std::map<int, double> exp_decoded_map2;
    exp_decoded_map2.emplace(0, 1.0);

    std::map<int, double> decoded_map2(dbutil::DecodeIntDoubleMap(encoded_map2));
    QCOMPARE(exp_decoded_map2.size(), decoded_map2.size());
    for (auto item: decoded_map2) {
        QVERIFY(calc::AlmostEqual(item.second, exp_decoded_map2.at(item.first), 0.0001));
    }
}



