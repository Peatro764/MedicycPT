#include "TestSeanceRecord.h"

#include "SeanceRecord.h"
#include "Calc.h"

void TestSeanceRecord::initTestCase() {}

void TestSeanceRecord::cleanupTestCase() {}

void TestSeanceRecord::init() {}

void TestSeanceRecord::cleanup() {}

SeanceRecord TestSeanceRecord::GetSeanceRecord(const int& um_del, const double& debit) {
    std::vector<double> duration { 1.1, 2.2 };
    std::vector<double> i_chambre1 { 3.3, 4.4, 5.5 };
    std::vector<double> i_chambre2 { 3.3, 4.4, 5.5, 6.6 };
    std::vector<int> cf9_status { 1, 1, 1, 1 };
    std::vector<int> um1 { 1, 2, um_del};
    std::vector<int> um2 { 0, 1, um_del - 1};
    int um_des = 100;
    int um_corr = 20;

    return SeanceRecord(QDateTime::currentDateTime(), debit, duration,
                        i_chambre1, i_chambre2, cf9_status, 33.0, 44.0, um_des, um_corr,
                        um1, um2, 1.123);
}

void TestSeanceRecord::Equality() {
    double d1 = 1.34;
    double d2 = 1.3402;
    double d3 = 1.3405;
    int i1 = 33;
    int i2 = 34;
    int i3 = 35;
    int i4 = 36;
    QDateTime time1 = QDateTime::currentDateTime();
    QDateTime time2 = time1.addDays(-1);
    std::vector<double> dVector1 { 1.1, 2.2 };
    std::vector<double> dVector2 { 2.2, 3.3 };
    std::vector<double> dVector3 { 3.3, 4.4 };
    std::vector<double> dVector4 { 5.5, 6.6 };
    std::vector<int> iVector1 { 1, 2, 3 };
    std::vector<int> iVector2 { 2, 3, 4 };
    std::vector<int> iVector3 { 3, 4, 5 };
    std::vector<int> iVector4 { 5, 6, 7 };

    SeanceRecord r1(time1, d1, dVector1, dVector2, dVector3, iVector1, i1, d1, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r2(time2, d1, dVector1, dVector2, dVector3, iVector1, i1, d1, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r3(time1, d2, dVector1, dVector2, dVector3, iVector1, i1, d1, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r4(time1, d1, dVector2, dVector2, dVector3, iVector1, i1, d1, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r5(time1, d1, dVector1, dVector3, dVector3, iVector1, i1, d1, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r6(time1, d1, dVector1, dVector2, dVector4, iVector1, i1, d1, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r7(time1, d1, dVector1, dVector2, dVector3, iVector2, i1, d1, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r8(time1, d1, dVector1, dVector2, dVector3, iVector1, i2, d1, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r9(time1, d1, dVector1, dVector2, dVector3, iVector1, i1, d2, i2, i3, iVector2, iVector3, d2);
    SeanceRecord r10(time1, d1, dVector1, dVector2, dVector3, iVector1, i1, d1, i3, i3, iVector2, iVector3, d2);
    SeanceRecord r11(time1, d1, dVector1, dVector2, dVector3, iVector1, i1, d1, i2, i4, iVector2, iVector3, d2);
    SeanceRecord r12(time1, d1, dVector1, dVector2, dVector3, iVector1, i1, d1, i2, i3, iVector3, iVector3, d2);
    SeanceRecord r13(time1, d1, dVector1, dVector2, dVector3, iVector1, i1, d1, i2, i3, iVector2, iVector4, d3);
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
    QVERIFY(r1 != r12);
    QVERIFY(r1 != r13);
}

void TestSeanceRecord::Constructor() {
    SeanceRecord sEmptyVectors(QDateTime::currentDateTime(), 10.0,
                               std::vector<double>(),
                               std::vector<double>(),
                               std::vector<double>(),
                               std::vector<int>(),
                               11.0,
                               12.0,
                               13,
                               5,
                               std::vector<int>(),
                               std::vector<int>(),
                               1.123);
    QVERIFY(calc::AlmostEqual(sEmptyVectors.GetDebit(), 10.0, 0.0001));
    QCOMPARE(sEmptyVectors.GetDuration(), std::vector<double>());
    QCOMPARE(sEmptyVectors.GetIChambre1(), std::vector<double>());
    QCOMPARE(sEmptyVectors.GetIChambre2(), std::vector<double>());
    QCOMPARE(sEmptyVectors.GetCF9Status(), std::vector<int>());
    QVERIFY(calc::AlmostEqual(sEmptyVectors.GetIStripper(), 11.0, 0.0001));
    QVERIFY(calc::AlmostEqual(sEmptyVectors.GetICF9(), 12.0, 0.0001));
    QCOMPARE(sEmptyVectors.GetUMPrevu(), 13);
    QCOMPARE(sEmptyVectors.GetUMCorr(), 5);
    QVERIFY(sEmptyVectors.GetUM1Delivered().empty());
    QVERIFY(sEmptyVectors.GetUM2Delivered().empty());
    QVERIFY(calc::AlmostEqual(sEmptyVectors.GetDoseEstimated(), 1.123, 0.0001));

    sEmptyVectors.SetEstimatedDose(4.44);
    QVERIFY(calc::AlmostEqual(sEmptyVectors.GetDoseEstimated(), 4.44, 0.0001));

    const std::vector<double> duration { 1.1 };
    const std::vector<double> i_chambre1 { 2.2, 3.3 };
    const std::vector<double> i_chambre2 { 4.4, 5.5, 6.6 };
    const std::vector<int> cf9_status { 1, 1, 3 };
    const std::vector<int> um_del_1 { 7, 8, 9, 10 };
    const std::vector<int> um_del_2 { 7, 8, 10, 12 };
    const double dose_est = 13.567;
    SeanceRecord sNonEmptyVectors(QDateTime::currentDateTime(), 10.0,
                                  duration,
                                  i_chambre1,
                                  i_chambre2,
                                  cf9_status,
                                  11.0,
                                  12.0,
                                  13,
                                  4,
                                  um_del_1,
                                  um_del_2,
                                  dose_est);
    QVERIFY(calc::AlmostEqual(sNonEmptyVectors.GetDuration(), duration, 0.0001));
    QVERIFY(calc::AlmostEqual(sNonEmptyVectors.GetIChambre1(), i_chambre1, 0.0001));
    QVERIFY(calc::AlmostEqual(sNonEmptyVectors.GetIChambre2(), i_chambre2, 0.0001));
    QCOMPARE(sNonEmptyVectors.GetCF9Status(), cf9_status);
    QCOMPARE(sNonEmptyVectors.GetUM1Delivered(), um_del_1);
    QCOMPARE(sNonEmptyVectors.GetUM2Delivered(), um_del_2);
    QVERIFY(calc::AlmostEqual(sNonEmptyVectors.GetDoseEstimated(), dose_est, 0.0001));
}

void TestSeanceRecord::GetTotalUMDelivered() {
    SeanceRecord sEmptyVectors(QDateTime::currentDateTime(), 10.0,
                               std::vector<double>(),
                               std::vector<double>(),
                               std::vector<double>(),
                               std::vector<int>(),
                               11.0,
                               12.0,
                               13,
                               4,
                               std::vector<int>(),
                               std::vector<int>(),
                               14.0);
    QCOMPARE(sEmptyVectors.GetTotalUMDelivered(), 0);

    const std::vector<int> um_del_1 { 7, 8, 9, 10 };
    const std::vector<int> um_del_2 { 7, 8, 9, 9 };
    SeanceRecord sNonEmptyVectors(QDateTime::currentDateTime(), 10.0,
                                  std::vector<double>(),
                                  std::vector<double>(),
                                  std::vector<double>(),
                                  std::vector<int>(),
                                  11.0,
                                  12.0,
                                  13,
                                  4,
                                  um_del_1,
                                  um_del_2,
                                  14.0);
     QCOMPARE(sNonEmptyVectors.GetTotalUMDelivered(), 10);
}

void TestSeanceRecord::GetTotalDuration() {
    SeanceRecord sEmptyVectors(QDateTime::currentDateTime(), 10.0,
                               std::vector<double>(),
                               std::vector<double>(),
                               std::vector<double>(),
                               std::vector<int>(),
                               11.0,
                               12.0,
                               13,
                               4,
                               std::vector<int>(),
                               std::vector<int>(),
                               14.0);
    QVERIFY(calc::AlmostEqual(sEmptyVectors.GetTotalDuration(), 0.0, 0.0001));

    const std::vector<double> duration { 7.1, 8.1, 9.1, 10.1 };
    SeanceRecord sNonEmptyVectors(QDateTime::currentDateTime(), 10.0,
                                  duration,
                                  std::vector<double>(),
                                  std::vector<double>(),
                                  std::vector<int>(),
                                  11.0,
                                  12.0,
                                  13,
                                  4,
                                  std::vector<int>(),
                                  std::vector<int>(),
                                  14.0);
     QVERIFY(calc::AlmostEqual(sNonEmptyVectors.GetTotalDuration(), 10.1-7.1, 0.0001));
}

void TestSeanceRecord::GetDoseDelivered() {
    int d1_1(5);
    double debit_1(34.0);
    auto r1 = GetSeanceRecord(d1_1, debit_1);
    QVERIFY(calc::AlmostEqual(r1.GetDoseDelivered(), (double)(d1_1 * debit_1 / 100.0), 0.0001));

    int d1_2(88);
    double debit_2(0.0);
    auto r2 = GetSeanceRecord(d1_2, debit_2);
    QVERIFY(calc::AlmostEqual(r2.GetDoseDelivered(), 0.0, 0.0001));

    int d1_3(0);
    double debit_3(3.222);
    auto r3 = GetSeanceRecord(d1_3, debit_3);
    QVERIFY(calc::AlmostEqual(r3.GetDoseDelivered(), 0.0, 0.0001));

    int d1_4(-100);
    double debit_4(3.222);
    auto r4 = GetSeanceRecord(d1_4, debit_4);
    QVERIFY(calc::AlmostEqual(r4.GetDoseDelivered(), 0.0, 0.0001));

    int d1_5(100);
    double debit_5(-3.222);
    auto r5 = GetSeanceRecord(d1_5, debit_5);
    QVERIFY(calc::AlmostEqual(r5.GetDoseDelivered(), 0.0, 0.0001));

    int d1_6(-100);
    double debit_6(-3.222);
    auto r6 = GetSeanceRecord(d1_6, debit_6);
    QVERIFY(calc::AlmostEqual(r6.GetDoseDelivered(), 0.0, 0.0001));
}


void TestSeanceRecord::PartOf() {
    const std::vector<double> i_chambre { 0.0, 200.0, 500.0, 1000.0, 1200.0 };

    const std::vector<double> duration_full { 0.0, 200.0, 500.0, 1000.0, 1200.0 };
    const std::vector<int> cf9_full { 0, 0, 1, 1, 0 };
    const std::vector<int> um_del_1_full { 1, 7, 8, 9, 10 };
    const std::vector<int> um_del_2_full { 1, 7, 8, 9, 9 };

    const std::vector<double> duration_subset { 0.0, 200.0, 500.0};
    const std::vector<int> cf9_subset { 0, 0, 1};
    const std::vector<int> um_del_1_subset { 1, 7, 8 };
    const std::vector<int> um_del_2_subset { 1, 7, 8 };

    const std::vector<double> duration_notsubset { 0.0, 201.0, 500.0};
    const std::vector<int> cf9_notsubset { 0, 1, 1};
    const std::vector<int> um_del_1_notsubset { 1, 7, 8, 10 };
    const std::vector<int> um_del_2_notsubset { 1, 7, 8, 9, 9, 23 };

    const double debit = 1.37;
    const int um_prevu = 345;
    const int um_corr = 23;

    SeanceRecord full(QDateTime::currentDateTime(),
                      debit,
                      duration_full, i_chambre, i_chambre, cf9_full,
                      100.0, 120.0,
                      um_prevu, um_corr,
                      um_del_1_full, um_del_2_full,
                      0.0);

    SeanceRecord subset(QDateTime::currentDateTime(),
                      debit,
                      duration_subset, i_chambre, i_chambre, cf9_subset,
                      100.0, 120.0,
                      um_prevu, um_corr,
                      um_del_1_subset, um_del_2_subset,
                      0.0);

    SeanceRecord not_subset_1(QDateTime::currentDateTime(),
                      debit,
                      duration_notsubset, i_chambre, i_chambre, cf9_subset,
                      100.0, 120.0,
                      um_prevu, um_corr,
                      um_del_1_subset, um_del_2_subset,
                      0.0);

    SeanceRecord not_subset_2(QDateTime::currentDateTime(),
                      debit,
                      duration_subset, i_chambre, i_chambre, cf9_notsubset,
                      100.0, 120.0,
                      um_prevu, um_corr,
                      um_del_1_subset, um_del_2_subset,
                      0.0);

    SeanceRecord not_subset_3(QDateTime::currentDateTime(),
                      debit,
                      duration_subset, i_chambre, i_chambre, cf9_subset,
                      100.0, 120.0,
                      um_prevu, um_corr,
                      um_del_1_notsubset, um_del_2_subset,
                      0.0);

    SeanceRecord not_subset_4(QDateTime::currentDateTime(),
                      debit,
                      duration_subset, i_chambre, i_chambre, cf9_subset,
                      100.0, 120.0,
                      um_prevu, um_corr,
                      um_del_1_subset, um_del_2_notsubset,
                      0.0);

    SeanceRecord not_subset_5(QDateTime::currentDateTime(),
                      debit + 0.1,
                      duration_subset, i_chambre, i_chambre, cf9_subset,
                      100.0, 120.0,
                      um_prevu, um_corr,
                      um_del_1_subset, um_del_2_subset,
                      0.0);

    SeanceRecord not_subset_6(QDateTime::currentDateTime(),
                      debit,
                      duration_subset, i_chambre, i_chambre, cf9_subset,
                      100.0, 120.0,
                      um_prevu + 1, um_corr,
                      um_del_1_subset, um_del_2_subset,
                      0.0);

    SeanceRecord not_subset_7(QDateTime::currentDateTime(),
                      debit,
                      duration_subset, i_chambre, i_chambre, cf9_subset,
                      100.0, 120.0,
                      um_prevu, um_corr + 1,
                      um_del_1_subset, um_del_2_subset,
                      0.0);

    QVERIFY(subset.PartOf(full));
    QVERIFY(!not_subset_1.PartOf(full));
    QVERIFY(!not_subset_2.PartOf(full));
    QVERIFY(!not_subset_3.PartOf(full));
    QVERIFY(!not_subset_4.PartOf(full));
    QVERIFY(!not_subset_5.PartOf(full));
    QVERIFY(!not_subset_6.PartOf(full));
    QVERIFY(!not_subset_7.PartOf(full));
}

void TestSeanceRecord::Subtract() {
    QDateTime timestamp = QDateTime::currentDateTime();
    double debit = 1.37;
    std::vector<double> duration { 1.0, 2.0, 3.0 };
    std::vector<double> i_ch_1;
    std::vector<double> i_ch_2;
    std::vector<int> cf9_status;
    double stripper = 100;
    double cf9 = 200;
    int um_prevu = 444;
    int um_corr = 55;
    std::vector<int> um1 {30, 100, 500 };
    std::vector<int> um2 {40, 120, 540 };
    double dose_est = 0.0;

    SeanceRecord record(timestamp, debit, duration, i_ch_1, i_ch_2, cf9_status,
                        stripper, cf9, um_prevu, um_corr, um1, um2, dose_est);

    try {
        record.Subtract(31, 20, 0.4);
        QFAIL("should have thrown 1");
    }
    catch (std::exception &exc) {}

    try {
        record.Subtract(10, 41, 0.4);
        QFAIL("should have thrown 2");
    }
    catch (std::exception &exc) {}

    try {
        record.Subtract(10, 10, 2.0);
        QFAIL("should have thrown 3");
    }
    catch (std::exception &exc) {}

    QCOMPARE(record, record.Subtract(0, 0, 0.0));

    std::vector<int> um1_a {20, 90, 490 };
    SeanceRecord record_a(timestamp, debit, duration, i_ch_1, i_ch_2, cf9_status,
                        stripper, cf9, um_prevu, um_corr, um1_a, um2, dose_est);
    QCOMPARE(record_a, record.Subtract(10, 0, 0.0));

    std::vector<int> um2_b {20, 100, 520 };
    SeanceRecord record_b(timestamp, debit, duration, i_ch_1, i_ch_2, cf9_status,
                        stripper, cf9, um_prevu, um_corr, um1, um2_b, dose_est);
    QCOMPARE(record_b, record.Subtract(0, 20, 0.0));

    std::vector<double> duration_c { 0.9, 1.9, 2.9 };
    SeanceRecord record_c(timestamp, debit, duration_c, i_ch_1, i_ch_2, cf9_status,
                        stripper, cf9, um_prevu, um_corr, um1, um2, dose_est);
    QCOMPARE(record_c, record.Subtract(0, 0, 0.1));
}
