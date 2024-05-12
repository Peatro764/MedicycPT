#include "TestDosimetryRecord.h"

#include "DosimetryRecord.h"
#include "Calc.h"

void TestDosimetryRecord::initTestCase() {}

void TestDosimetryRecord::cleanupTestCase() {}

void TestDosimetryRecord::init() {}

void TestDosimetryRecord::cleanup() {}

void TestDosimetryRecord::Constructor1() {
    const int um_pre (66);
    const std::vector<int> um_del { 33 };
    const double charge (12.5);
    const double air_ic_charge(3.123);
    const DosimetryRecord record(std::vector<double>(),
                           std::vector<double>(),
                           std::vector<double>(),
                           0.0,
                           0.0,
                           um_pre,
                           um_del,
                           charge,
                           air_ic_charge);
    QCOMPARE(record.GetUMDelivered(), um_del);
    QCOMPARE(record.GetUMPrevu(), um_pre);
    QVERIFY(calc::AlmostEqual(record.GetCharge(), charge, 0.001));
    QVERIFY(calc::AlmostEqual(record.GetAirICCharge(), air_ic_charge, 0.001));
}

void TestDosimetryRecord::Constructor2() {
    const std::vector<int> um_del { 33 };
    const double charge (12.5);
    const DosimetryRecord record(33, charge);
    QCOMPARE(record.GetUMDelivered(), um_del);
    QVERIFY(calc::AlmostEqual(record.GetCharge(), charge, 0.001));
}

void TestDosimetryRecord::GetTotalDuration() {
    const int um_pre (66);
    const std::vector<int> um_del { 33 };

    const DosimetryRecord record1( std::vector<double>(),
                           std::vector<double>(),
                           std::vector<double>(),
                           0.0,
                           0.0,
                           um_pre,
                           um_del,
                           33.0, 1.0);
    QVERIFY(calc::AlmostEqual(record1.GetTotalDuration(), 0.0, 0.001));

    const std::vector<double> duration {1.1, 2.2, 3.3};
    const DosimetryRecord record2(duration,
                           std::vector<double>(),
                           std::vector<double>(),
                           0.0,
                           0.0,
                           um_pre,
                           um_del,
                           33.0, 1.0);
    QVERIFY(calc::AlmostEqual(record2.GetTotalDuration(), 3.3, 0.001));
}

void TestDosimetryRecord::GetTotalUMDelivered() {
    const int um_pre (66);

    const DosimetryRecord record1( std::vector<double>(),
                           std::vector<double>(),
                           std::vector<double>(),
                           0.0,
                           0.0,
                           um_pre,
                           std::vector<int>(),
                           33.0, 1.0);
    QCOMPARE(record1.GetTotalUMDelivered(), 0);

    const std::vector<int> um_del { 11, 22, 33 };
    const DosimetryRecord record2(std::vector<double>(),
                           std::vector<double>(),
                           std::vector<double>(),
                           0.0,
                           0.0,
                           um_pre,
                           um_del,
                           33.0, 1.0);
    QCOMPARE(record2.GetTotalUMDelivered(), 33);
}

void TestDosimetryRecord::Equality() {
    const std::vector<int> v_int_1 { 11, 22, 33 };
    const std::vector<int> v_int_2 { 11, 22 };
    const std::vector<double> v_double_1 { 1.0, 234.0 };
    const std::vector<double> v_double_2 { 1.0, 234.0, 444.0 };
    const std::vector<double> v_double_3 {  };
    const std::vector<double> v_double_4 { 1.0 };


    const DosimetryRecord r1(v_double_1, v_double_2, v_double_3, 1.1, 10.01, 101, v_int_1, 1.2828, 4.345);
    const DosimetryRecord r2(v_double_1, v_double_2, v_double_3, 1.12, 10.01, 101, v_int_1, 1.2828, 4.345);
    const DosimetryRecord r3(v_double_1, v_double_2, v_double_3, 1.1, 10.02, 101, v_int_1, 1.2828, 4.345);
    const DosimetryRecord r4(v_double_1, v_double_2, v_double_3, 1.1, 10.01, 102, v_int_1, 1.2828, 4.345);
    const DosimetryRecord r5(v_double_1, v_double_2, v_double_3, 1.1, 10.01, 101, v_int_2, 1.2828, 4.345);
    const DosimetryRecord r6(v_double_1, v_double_2, v_double_3, 1.1, 10.01, 101, v_int_1, 1.284, 4.345);
    const DosimetryRecord r7(v_double_1, v_double_2, v_double_3, 1.1, 10.01, 101, v_int_1, 1.2828, 4.36);
    const DosimetryRecord r8(v_double_3, v_double_2, v_double_3, 1.1, 10.01, 101, v_int_1, 1.2828, 4.345);
    const DosimetryRecord r9(v_double_1, v_double_4, v_double_3, 1.1, 10.01, 101, v_int_1, 1.2828, 4.345);
    const DosimetryRecord r10(v_double_1, v_double_2, v_double_1, 1.1, 10.01, 101, v_int_1, 1.2828, 4.345);
    const DosimetryRecord r11(v_double_1, v_double_2, v_double_3, 1.1, 10.01, 101, v_int_2, 1.2828, 4.345);

    QVERIFY(r1 == r1);
    QVERIFY(r1 != r2);
    QVERIFY(r1 != r3);
    QVERIFY(r1 != r4);
    QVERIFY(r1 != r5);
    QVERIFY(r1 != r6);
    QVERIFY(r1 != r7);
    QVERIFY(r1 != r8);
    QVERIFY(r1 != r9);
    QVERIFY(r1 != r10);
    QVERIFY(r1 != r11);
}
