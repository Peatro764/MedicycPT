#include "TestDosimetry.h"

#include <iostream>
#include <QDateTime>

#include "Chambre.h"
#include "Dosimetry.h"
#include "Calc.h"

void TestDosimetry::initTestCase() {
     maxRelDiff = 0.001;

    QDateTime timestamp(QDateTime::currentDateTime());
    bool ref(false);
    bool mc(false);
    double t(23.0);
    double p(1001.0);
    double nd(2.22);
    double kqq0(3.33);
    Chambre chambre(0, QDateTime(), "mychambre", nd, kqq0, 0, 0);
    int d1(560);
    double charge(37.7);

    std::list<DosimetryRecord> records;
    DosimetryRecord rec1(d1, charge);
    records.push_back(rec1);
    dosimetry1_ = new Dosimetry(timestamp, ref, mc, t, p, chambre, records, "");
    DosimetryRecord rec2(d1 + 50, charge + 10.0);
    records.push_back(rec2);
    dosimetry2_ = new Dosimetry(timestamp, ref, mc, t, p, chambre, records, "");
    DosimetryRecord rec3(d1 - 30, charge - 8.0);
    records.push_back(rec3);
    dosimetry3_ = new Dosimetry(timestamp, ref, mc, t, p, chambre, records, "");

    debit1_.push_back(0.508926312);
    debit2_.push_back(0.508926312);
    debit2_.push_back(0.591139698);
    debit3_.push_back(0.508926312);
    debit3_.push_back(0.591139698);
    debit3_.push_back(0.423625565);
}

void TestDosimetry::cleanupTestCase() {
    delete dosimetry1_;
    delete dosimetry2_;
    delete dosimetry3_;
}

void TestDosimetry::init() {}

void TestDosimetry::cleanup() {}

Chambre TestDosimetry::GetChambre() {
    return Chambre(0, QDateTime(), "mychambre", 2.22, 3.33, 0, 0);
}

void TestDosimetry::Constructors() {
    Dosimetry dosimetry;
    QCOMPARE((int)dosimetry.GetRecords().size(), (int)0);
    QVERIFY(!dosimetry.IsMonteCarlo());
    QVERIFY(!dosimetry.IsReference());
    QVERIFY(!dosimetry.IsValid());
}

void TestDosimetry::Setters() {
    QDateTime timestamp(QDateTime::currentDateTime());
    bool ref(false);
    bool mc(false);
    double t(23.0);
    double p(1001.0);
    Chambre chambre(GetChambre());
    std::list<DosimetryRecord> records;

    Dosimetry dos(timestamp, ref, mc, t, p, chambre, records, "");
    QCOMPARE(dos.GetTimestamp(), timestamp);
    QCOMPARE(dos.GetTemperature(), t);
    QCOMPARE(dos.GetPressure(), p);
    QCOMPARE(dos.GetChambre(), chambre);
    QCOMPARE(dos.IsReference(), ref);
    QCOMPARE(dos.IsMonteCarlo(), mc);

    dos.SetTemperature(33.0);
    QCOMPARE(dos.GetTemperature(), 33.0f);
    dos.SetTemperature(t);
    QCOMPARE(dos.GetTemperature(), t);

    dos.SetPressure(2000.0);
    QCOMPARE(dos.GetPressure(), 2000.0f);
    dos.SetPressure(p);
    QCOMPARE(dos.GetPressure(), p);

    Chambre chambre2(0, QDateTime(), "mychambre2", 12.0, 30.0, 0, 0);
    dos.SetChambre(chambre2);
    QCOMPARE(dos.GetChambre(), chambre2);
    dos.SetChambre(chambre);
    QCOMPARE(dos.GetChambre(), chambre);

    dos.SetReference(true);
    QCOMPARE(dos.IsReference(), true);
    dos.SetReference(ref);
    QCOMPARE(dos.IsReference(), ref);

    dos.SetMonteCarlo(true);
    QCOMPARE(dos.IsMonteCarlo(), true);
    dos.SetMonteCarlo(mc);
    QCOMPARE(dos.IsMonteCarlo(), mc);
}

void TestDosimetry::Comparisons() {
    QDateTime timestamp(QDateTime::currentDateTime());
    bool ref(false);
    bool mc(false);
    double t(23.0);
    double p(1001.0);
    Chambre chambre(GetChambre());
    std::list<DosimetryRecord> records;

    Dosimetry dos1(timestamp, ref, mc, t, p, chambre, records, "");
    Dosimetry dos2(timestamp, ref, mc, t, p, chambre, records, "");
    QCOMPARE(dos1, dos2);

    DosimetryRecord rec1(100, 15.0);
    records.push_back(rec1);
    Dosimetry dos3(timestamp, ref, mc, t, p, chambre, records, "");
    Dosimetry dos4(timestamp, ref, mc, t, p, chambre, records, "");
    QCOMPARE(dos3, dos4);

    Dosimetry dos5(timestamp, ref, mc, t, p, chambre, records, "");
    DosimetryRecord rec2(200, 18.0);
    records.push_back(rec2);
    Dosimetry dos6(timestamp, ref, mc, t, p, chambre, records, "");
    QVERIFY(!(dos5 == dos6));

    double p2(1002.0);
    Dosimetry dos7(timestamp, ref, mc, t, p, chambre, records, "");
    Dosimetry dos8(timestamp, ref, mc, t, p2, chambre, records, "");
    QVERIFY(!(dos7 == dos8));
}

void TestDosimetry::AddRecord() {
    QDateTime timestamp(QDateTime::currentDateTime());
    bool ref(false);
    bool mc(false);
    double t(23.0);
    double p(1001.0);
    Chambre chambre(GetChambre());
    std::list<DosimetryRecord> recordsNone;
    std::list<DosimetryRecord> recordsAll;
    std::list<DosimetryRecord> records12;
    std::list<DosimetryRecord> records1;
    DosimetryRecord r1(1, 1.1);
    DosimetryRecord r2(2, 2.2);
    DosimetryRecord r3(3, 3.3);
    recordsAll.push_back(r1);
    recordsAll.push_back(r2);
    recordsAll.push_back(r3);
    records1.push_back(r1);
    records12.push_back(r1);
    records12.push_back(r2);

    Dosimetry dos(timestamp, ref, mc, t, p, chambre, recordsNone, "");

    QCOMPARE(dos.GetRecords(), recordsNone);
    dos.AddRecord(r1);
    QCOMPARE(dos.GetRecords(), records1);
    dos.AddRecord(r2);
    QCOMPARE(dos.GetRecords(), records12);
}

void TestDosimetry::DeleteSingleRecord() {
    QDateTime timestamp(QDateTime::currentDateTime());
    bool ref(false);
    bool mc(false);
    double t(23.0);
    double p(1001.0);
    Chambre chambre(GetChambre());
    std::list<DosimetryRecord> recordsAll;
    std::list<DosimetryRecord> records13;
    std::list<DosimetryRecord> records3;
    DosimetryRecord r1(1, 1.1);
    DosimetryRecord r2(2, 2.2);
    DosimetryRecord r3(3, 3.3);
    recordsAll.push_back(r1);
    recordsAll.push_back(r2);
    recordsAll.push_back(r3);
    records13.push_back(r1);
    records13.push_back(r3);
    records3.push_back(r3);

    Dosimetry dos(timestamp, ref, mc, t, p, chambre, recordsAll, "");

    QCOMPARE(dos.GetRecords(), recordsAll);
    dos.DeleteSingleRecord(1);
    QCOMPARE(dos.GetRecords(), records13);
    dos.DeleteSingleRecord(0);
    QCOMPARE(dos.GetRecords(), records3);
}

void TestDosimetry::DeleteRecords() {
    QDateTime timestamp(QDateTime::currentDateTime());
    bool ref(false);
    bool mc(false);
    double t(23.0);
    double p(1001.0);
    Chambre chambre(GetChambre());
    std::list<DosimetryRecord> recordsNone;
    std::list<DosimetryRecord> recordsAll;
    DosimetryRecord r1(1, 1.1);
    DosimetryRecord r2(2, 2.2);
    DosimetryRecord r3(3, 3.3);
    recordsAll.push_back(r1);
    recordsAll.push_back(r2);
    recordsAll.push_back(r3);

    Dosimetry dos(timestamp, ref, mc, t, p, chambre, recordsAll, "");

    QCOMPARE(dos.GetRecords(), recordsAll);
    dos.DeleteRecords();
    QCOMPARE(dos.GetRecords(), recordsNone);
    dos.DeleteRecords();
    QCOMPARE(dos.GetRecords(), recordsNone);
}

void TestDosimetry::GetFtp() {
    QDateTime timestamp(QDateTime::currentDateTime());
    bool ref(false);
    bool mc(false);
    double t(23.0);
    double p(1001.0);
    Chambre chambre(GetChambre());
    std::list<DosimetryRecord> records;

    Dosimetry dos(timestamp, ref, mc, t, p, chambre, records, "");

    QVERIFY(calc::AlmostEqual(dos.GetFtp(), 1.022596668, 0.0001));
    dos.SetPressure(-1.0f);
    QVERIFY(calc::AlmostEqual(dos.GetFtp(), 0.0, 0.0001));

}

void TestDosimetry::GetDebit() {
    QCOMPARE(dosimetry1_->GetDebit(), Debit(dosimetry1_->GetDebitMean(),
                                            dosimetry1_->GetDebitStdDev(),
                                            dosimetry1_->GetTimestamp(),
                                            dosimetry1_->IsMonteCarlo(),
                                            dosimetry1_->GetChambre()));
}

void TestDosimetry::GetDebits() {
    QVERIFY(calc::AlmostEqual(dosimetry1_->GetDebits(), debit1_, maxRelDiff));
    QVERIFY(calc::AlmostEqual(dosimetry2_->GetDebits(), debit2_, maxRelDiff));
    QVERIFY(calc::AlmostEqual(dosimetry3_->GetDebits(), debit3_, maxRelDiff));
}

void TestDosimetry::GetDebitMean() {
    QVERIFY(calc::AlmostEqual(dosimetry1_->GetDebitMean(), calc::Mean(debit1_), maxRelDiff));
    QVERIFY(calc::AlmostEqual(dosimetry2_->GetDebitMean(), calc::Mean(debit2_), maxRelDiff));
    QVERIFY(calc::AlmostEqual(dosimetry3_->GetDebitMean(), calc::Mean(debit3_), maxRelDiff));
}

void TestDosimetry::GetDebitStdDev() {
    QVERIFY(calc::AlmostEqual(dosimetry1_->GetDebitStdDev(), calc::StdDev(debit1_), maxRelDiff));
    QVERIFY(calc::AlmostEqual(dosimetry2_->GetDebitStdDev(), calc::StdDev(debit2_), maxRelDiff));
    QVERIFY(calc::AlmostEqual(dosimetry3_->GetDebitStdDev(), calc::StdDev(debit3_), maxRelDiff));
}

void TestDosimetry::GetRollingDebitMeans() {
    std::vector<double> rollingMean1 { calc::Mean(debit1_) };
    QVERIFY(calc::AlmostEqual(rollingMean1, dosimetry1_->GetRollingDebitMeans(), maxRelDiff));

    std::vector<double> rollingMean2 { calc::Mean(debit1_), calc::Mean(debit2_) };
    QVERIFY(calc::AlmostEqual(rollingMean2, dosimetry2_->GetRollingDebitMeans(), maxRelDiff));

    std::vector<double> rollingMean3 { calc::Mean(debit1_), calc::Mean(debit2_), calc::Mean(debit3_) };
    QVERIFY(calc::AlmostEqual(rollingMean3, dosimetry3_->GetRollingDebitMeans(), maxRelDiff));
}

void TestDosimetry::GetRollingDebitStdDevs() {
    std::vector<double> rollingStdDev1 { calc::StdDev(debit1_) };
    QVERIFY(calc::AlmostEqual(rollingStdDev1, dosimetry1_->GetRollingDebitStdDevs(), maxRelDiff));

    std::vector<double> rollingStdDev2 { calc::StdDev(debit1_), calc::StdDev(debit2_) };
    QVERIFY(calc::AlmostEqual(rollingStdDev2, dosimetry2_->GetRollingDebitStdDevs(), maxRelDiff));

    std::vector<double> rollingStdDev3 { calc::StdDev(debit1_), calc::StdDev(debit2_), calc::StdDev(debit3_) };
    QVERIFY(calc::AlmostEqual(rollingStdDev3, dosimetry3_->GetRollingDebitStdDevs(), maxRelDiff));
}

void TestDosimetry::GetExpectedCharge() {
   maxRelDiff = 0.001;

   QDateTime timestamp(QDateTime::currentDateTime());
   bool ref(false);
   bool mc(false);
   double t(23.0);
   double p(1001.0);
   double nd(2.22);
   double kqq0(3.33);
   Chambre chambre(0, QDateTime(), "mychambre", nd, kqq0, 0, 0);
   int d1(560);
   double charge(37.7);

   std::list<DosimetryRecord> records;
   DosimetryRecord rec1(d1, charge);
   records.push_back(rec1);
   Dosimetry dos1(timestamp, ref, mc, t, p, chambre, records, "");

   const double debit = dos1.GetDebitMean();

   QVERIFY(calc::AlmostEqual(charge, dos1.GetExpectedCharge(debit, d1), 0.0001));
   QVERIFY(calc::AlmostEqual(dos1.GetExpectedCharge(-1.0, d1), 0.0, 0.0001));
   QVERIFY(calc::AlmostEqual(dos1.GetExpectedCharge(debit, -1), 0.0, 0.0001));
}

void TestDosimetry::MeasurementConditionsNotGiven() {
    maxRelDiff = 0.001;

   QDateTime timestamp(QDateTime::currentDateTime());
   bool ref(false);
   bool mc(false);
   double t(23.0);
   double p(1001.0);
   double nd(2.22);
   double kqq0(3.33);
   Chambre chambre(0, QDateTime(), "mychambre", nd, kqq0, 0, 0);
   int d1(560);
   double charge(37.7);

   std::list<DosimetryRecord> records;
   DosimetryRecord rec1(d1, charge);
   records.push_back(rec1);
   Dosimetry dos1;
   dos1.SetReference(ref);
   dos1.SetMonteCarlo(mc);
   dos1.SetTimestamp(timestamp);
   dos1.AddRecord(rec1);

   QCOMPARE(dos1.MeasurementConditionsEntered(), false);
   QVERIFY(calc::AlmostEqual(dos1.GetDebit().mean(), 0.0, maxRelDiff));
   QVERIFY(calc::AlmostEqual(dos1.GetDebitMean(), 0.0, maxRelDiff));
   QVERIFY(calc::AlmostEqual(dos1.GetDebitStdDev(), 0.0, maxRelDiff));
   QVERIFY(calc::AlmostEqual(dos1.GetFtp(), 0.0, maxRelDiff));
   std::vector<double> rolling_debit_mean = dos1.GetRollingDebitMeans();
   std::vector<double> rolling_debit_sd = dos1.GetRollingDebitStdDevs();
   std::vector<double> zero_vector = {0};
   QVERIFY(calc::AlmostEqual(rolling_debit_mean, zero_vector, maxRelDiff));
   QVERIFY(calc::AlmostEqual(rolling_debit_sd, zero_vector, maxRelDiff));
   QVERIFY(calc::AlmostEqual(dos1.GetDebits(), zero_vector, maxRelDiff));

   dos1.SetTemperature(t);
   QCOMPARE(dos1.MeasurementConditionsEntered(), false);
   dos1.SetPressure(p);
   QCOMPARE(dos1.MeasurementConditionsEntered(), false);
   dos1.SetChambre(chambre);
   QCOMPARE(dos1.MeasurementConditionsEntered(), true);
   const double exp_debit(0.508926312);
   QVERIFY(calc::AlmostEqual(dos1.GetDebit().mean(), exp_debit, maxRelDiff));
}
