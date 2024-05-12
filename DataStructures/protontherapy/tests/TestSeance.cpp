#include "TestSeance.h"

#include "Seance.h"
#include "Calc.h"

void TestSeance::initTestCase() {}

void TestSeance::cleanupTestCase() {}

void TestSeance::init() {}

void TestSeance::cleanup() {}

SeanceRecord TestSeance::GetSeanceRecord(const int& um_del, const double& debit, double dose_est) {
    std::vector<double> duration { 1.1, 2.2 };
    std::vector<double> i_chambre1 { 3.3, 4.4, 5.5 };
    std::vector<double> i_chambre2 { 3.3, 4.4, 5.5, 6.6 };
    std::vector<int> cf9_status {1, 1, 1};
    std::vector<int> um_1 { 1, 2, um_del};
    std::vector<int> um_2 { 0, 1, um_del - 1};

    return SeanceRecord(QDateTime::currentDateTime(), debit, duration,
                        i_chambre1, i_chambre2, cf9_status, 33.0, 44.0, 100, 20,
                        um_1, um_2, dose_est);
}

void TestSeance::Comparisons() {
    Seance s1(1, 1.123, std::list<SeanceRecord>());
    Seance s2(1, 1.123, std::list<SeanceRecord>());
    Seance s3(2, 1.123, std::list<SeanceRecord>());
    Seance s4(1, 1.223, std::list<SeanceRecord>());
    std::list<SeanceRecord> records { GetSeanceRecord(1000, 2.0, 0.0) };
    Seance s5(1, 1.223, records);
    QCOMPARE(s1, s2);
    QVERIFY(s1 != s3);
    QVERIFY(s1 != s4);
    QVERIFY(s1 != s5);
}

void TestSeance::Getters() {
    std::list<SeanceRecord> records = { GetSeanceRecord(44, 3.2, 0.0) };
    int seance_id(333);
    double dose_prescribed(3.44);
    Seance s(seance_id, dose_prescribed, records);

    QCOMPARE(s.GetSeanceId(), seance_id);
    QCOMPARE(s.GetDosePrescribed(), dose_prescribed);
    QCOMPARE(s.GetSeanceRecords(), records);
}

void TestSeance::AddSeanceRecords() {
    auto r = GetSeanceRecord(44, 3.2, 0.0);
    std::list<SeanceRecord> all = {r, r, r};
    std::list<SeanceRecord> one = {r};
    std::list<SeanceRecord> two = {r, r};
    std::list<SeanceRecord> none;

    Seance seance(0, 100.0, none);
    QCOMPARE(seance.GetSeanceRecords(), none);
    seance.AddSeanceRecord(r);
    QCOMPARE(seance.GetSeanceRecords(), one);
    seance.AddSeanceRecord(r);
    QCOMPARE(seance.GetSeanceRecords(), two);
    seance.AddSeanceRecord(r);
    QCOMPARE(seance.GetSeanceRecords(), all);
}

void TestSeance::GetDoseDelivered() {
    // single record
    double debit(34.0);
    int um_del1(5);
    auto r1 = GetSeanceRecord(um_del1, debit, 0.0);
    std::list<SeanceRecord> records = { r1 };
    Seance seance(0, 100.0, records);

    QCOMPARE(seance.GetDoseDelivered(), (double)(um_del1 * debit / 100.0));

    // two records
    int um_del2(88);
    SeanceRecord r2 = GetSeanceRecord(um_del2, debit, 0.0);
    seance.AddSeanceRecord(r2);
    QCOMPARE(seance.GetDoseDelivered(), (double)(um_del1 * debit / 100.0 + um_del2 * debit / 100.0));

    // three records
    int um_del3(88);
    auto  r3 = GetSeanceRecord(um_del3, debit, 0.0);
    seance.AddSeanceRecord(r3);
    QCOMPARE(seance.GetDoseDelivered(), (double)(um_del1 * debit / 100.0
                                        + um_del2 * debit / 100.0
                                        + um_del3 * debit / 100.0));

    // two records with different debits
    int um_del4(1001);
    double debit_4(1.37);
    auto r4 = GetSeanceRecord(um_del4, debit_4, 0.0);
    int um_del5(800);
    double debit_5(1.27);
    auto r5 = GetSeanceRecord(um_del5, debit_5, 0.0);
    std::list<SeanceRecord> records45 = { r4, r5 };
    Seance seance45(0, 100.0, records45);
    QVERIFY(calc::AlmostEqual((um_del4 * debit_4 / 100.0f) + (um_del5 * debit_5 / 100.0f), seance45.GetDoseDelivered(), 0.001f));

}

void TestSeance::GetDoseEstimated() {
    // single record
    auto r1 = GetSeanceRecord(10, 1.0, 5.5);
    std::list<SeanceRecord> records = { r1 };
    Seance seance(0, 100.0, records);

    QCOMPARE(seance.GetDoseEstimated(), (double)(5.5));

    // two records
    SeanceRecord r2 = GetSeanceRecord(10, 1.0, 6.5);
    seance.AddSeanceRecord(r2);
    QCOMPARE(seance.GetDoseEstimated(), (double)(12.0));

    // three records
    auto  r3 = GetSeanceRecord(10.0, 1.0, 7.0);
    seance.AddSeanceRecord(r3);
    QCOMPARE(seance.GetDoseEstimated(), (double)(19.0));
}

