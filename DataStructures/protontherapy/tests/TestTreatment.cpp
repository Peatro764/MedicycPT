#include "TestTreatment.h"

#include "Patient.h"
#include "Seance.h"
#include "Treatment.h"
#include "Calc.h"
#include "Util.h"

void TestTreatment::initTestCase() {}

void TestTreatment::cleanupTestCase() {}

void TestTreatment::init() {}

void TestTreatment::cleanup() {}

void TestTreatment::Constructors() {
    Treatment treatment;

    QVERIFY(!treatment.GetPatientDosimetry().IsValid());
    QVERIFY(!treatment.GetPatientRefDosimetry().IsValid());
    QVERIFY(!treatment.GetRefDosimetryToday().IsValid());
}

SeanceRecord TestTreatment::GetSeanceRecord(const int& um_del, const double& debit, const double& est_dose) {
    std::vector<double> duration { 1.1, 2.2 };
    std::vector<double> i_chambre1 { 3.3, 4.4, 5.5 };
    std::vector<double> i_chambre2 { 3.3, 4.4, 5.5, 6.6 };
    std::vector<int> cf9_status { 1, 1, 1, 1 };
    std::vector<int> um_1 { 1, 2, um_del};
    std::vector<int> um_2 { 0, 1, um_del-1};

    return SeanceRecord(QDateTime::currentDateTime(), debit, duration,
                        i_chambre1, i_chambre2, cf9_status, 33.0, 44.0, 100, 20,
                        um_1, um_2, est_dose);
}

void TestTreatment::GetActiveSeanceNumber() {

    // one planned seance
    std::vector<Seance> seances2;
    std::list<SeanceRecord> records2;
    seances2.push_back(Seance(1, 1.0, records2));
    Treatment treatment2(1, Patient(1, "a","b"), seances2);
    QCOMPARE(treatment2.GetActiveSeanceNumber(), 0);

    treatment2.IncrementSeance();
    QCOMPARE(treatment2.GetActiveSeanceNumber(), -1);

    treatment2.IncrementSeance();
    QCOMPARE(treatment2.GetActiveSeanceNumber(), -1);

    // two planned seances
    std::vector<Seance> seances3;
    std::list<SeanceRecord> records3;
    seances3.push_back(Seance(1, 1.0, records3));
    seances3.push_back(Seance(2, 2.0, records3));
    Treatment treatment3(1, Patient(1, "a","b"), seances3);
    QCOMPARE(treatment3.GetActiveSeanceNumber(), 0);

    treatment3.IncrementSeance();
    QCOMPARE(treatment3.GetActiveSeanceNumber(), 1);

    treatment3.IncrementSeance();
    QCOMPARE(treatment3.GetActiveSeanceNumber(), -1);

    treatment3.IncrementSeance();
    QCOMPARE(treatment3.GetActiveSeanceNumber(), -1);
}

void TestTreatment::GetActiveSeance() {
    std::vector<Seance> seances;
    std::list<SeanceRecord> records;
    seances.push_back(Seance(1, 1.0, records));
    seances.push_back(Seance(2, 2.0, records));
    Treatment treatment(1, Patient(1, "a","b"), seances);
    QCOMPARE(treatment.GetActiveSeance()->GetSeanceId(), 1);

    treatment.IncrementSeance();
    QCOMPARE(treatment.GetActiveSeance()->GetSeanceId(), 2);

    treatment.IncrementSeance();
    QCOMPARE((bool)treatment.GetActiveSeance(), false);

    treatment.IncrementSeance();
    QCOMPARE((bool)treatment.GetActiveSeance(), false);
}

void TestTreatment::GetSeanceByIndex() {
    std::list<SeanceRecord> records;
    Seance s1(1, 1.0, records);
    Seance s2(2, 2.0, records);

    std::vector<Seance> seances0;
    std::vector<Seance> seances1;
    seances1.push_back(s1);
    std::vector<Seance> seances12;
    seances12.push_back(s1);
    seances12.push_back(s2);

    Treatment treatment0(0, Patient(1, "a","b"), seances0);
    try {
        auto seance = treatment0.GetSeance(0);
        QFAIL("TestTreatment::GetSeanceByIndex An exception should have been thrown");
    }
    catch (const std::exception& exc) {}

    Treatment treatment1(1, Patient(1, "a","b"), seances1);
    QCOMPARE(treatment1.GetSeance(0), s1);
    try {
        auto seance = treatment1.GetSeance(1);
        QFAIL("TestTreatment::GetSeanceByIndex An exception should have been thrown");
    }
    catch (const std::exception& exc) {}

    Treatment treatment2(2, Patient(1, "a","b"), seances12);
    QCOMPARE(treatment2.GetSeance(0), s1);
    QCOMPARE(treatment2.GetSeance(1), s2);
    try {
        auto seance = treatment2.GetSeance(2);
        QFAIL("TestTreatment::GetSeanceByIndex An exception should have been thrown");
    }
    catch (const std::exception& exc) {}

    try {
        auto seance = treatment2.GetSeance(-1);
        QFAIL("TestTreatment::GetSeanceByIndex An exception should have been thrown");
    }
    catch (const std::exception& exc) {}
}

void TestTreatment::GetPastSeances() {
    std::list<SeanceRecord> records;
    Seance s1(1, 1.0, records);
    Seance s2(2, 2.0, records);
    Seance s3(3, 3.0, records);

    std::vector<Seance> seances0;
    std::vector<Seance> seances1;
    seances1.push_back(s1);
    std::vector<Seance> seances12;
    seances12.push_back(s1);
    seances12.push_back(s2);
    std::vector<Seance> seances123;
    seances123.push_back(s1);
    seances123.push_back(s2);
    seances123.push_back(s3);

    Treatment treatment(1, Patient(1, "a","b"), seances123);

    QCOMPARE(treatment.GetPastSeances(), seances0);

    treatment.IncrementSeance();
    QCOMPARE(treatment.GetPastSeances(), seances1);

    treatment.IncrementSeance();
    QCOMPARE(treatment.GetPastSeances(), seances12);

    treatment.IncrementSeance();
    QCOMPARE(treatment.GetPastSeances(), seances123);

    treatment.IncrementSeance();
    QCOMPARE(treatment.GetPastSeances(), seances123);
}

void TestTreatment::GetFutureSeances() {
    std::list<SeanceRecord> records;
    Seance s1(1, 1.0, records);
    Seance s2(2, 2.0, records);
    Seance s3(3, 3.0, records);

    std::vector<Seance> seances0;
    std::vector<Seance> seances3;
    seances3.push_back(s3);
    std::vector<Seance> seances23;
    seances23.push_back(s2);
    seances23.push_back(s3);
    std::vector<Seance> seances123;
    seances123.push_back(s1);
    seances123.push_back(s2);
    seances123.push_back(s3);

    Treatment treatment(1, Patient(1, "a","b"), seances123);

    QCOMPARE(treatment.GetFutureSeances(), seances23);

    treatment.IncrementSeance();
    QCOMPARE(treatment.GetFutureSeances(), seances3);

    treatment.IncrementSeance();
    QCOMPARE(treatment.GetFutureSeances(), seances0);

    treatment.IncrementSeance();
    QCOMPARE(treatment.GetFutureSeances(), seances0);
}

void TestTreatment::GetRemainingDoseActiveSeance() {

    // single seance planned, none done
    std::list<SeanceRecord> records2;
    std::vector<Seance> seances2 = {Seance(1, 9.0, records2)};
    Treatment treatment2(2, Patient(1, "a","b"), seances2);
    QCOMPARE(treatment2.GetRemainingDoseActiveSeance(), 9.0f);

    // single seance planned, partially done
    double dmoyen3(3.75);
    int d13(80);
    auto rec3 = GetSeanceRecord(d13, dmoyen3, 0.0);
    double rec3_dose = dmoyen3 * d13 / 100.0;
    std::list<SeanceRecord> records3 { rec3 };
    std::vector<Seance> seances3 = {Seance(1, 9.0, records3)};
    Treatment treatment3(3, Patient(1, "a","b"), seances3);
    QCOMPARE(treatment3.GetRemainingDoseActiveSeance(), (double)(9.0 - rec3_dose));

    // single seance planned,  done
    double dmoyen4(3.75);
    int d14(400); // -> Delivered dose is larger than dose prevu
    auto rec4 = GetSeanceRecord(d14, dmoyen4, 0.0);
    std::list<SeanceRecord> records4 = { rec4 };
    std::vector<Seance> seances4 = {Seance(1, 9.0, records4)};
    Treatment treatment4(4, Patient(1, "a","b"), seances4);
    QCOMPARE(treatment4.GetRemainingDoseActiveSeance(), 0.0f);

    // multiple seances planned, none done
    std::list<SeanceRecord> records5;
    std::vector<Seance> seances5 = {Seance(1, 9.0, records5), Seance(2, 10.0, records5)};
    Treatment treatment5(5, Patient(1, "a","b"), seances5);
    QCOMPARE(treatment5.GetRemainingDoseActiveSeance(), 9.0f);

    // multiple seances planned, the first done (but all dose not delivered)
    double dmoyen6(3.75);
    int d16(80);
    auto rec6 = GetSeanceRecord(d16, dmoyen6, 0.0);
    auto rec6_dose = dmoyen6 * d16 / 100.0;
    std::list<SeanceRecord> records6a = { rec6 };
    std::list<SeanceRecord> records6b;
    std::vector<Seance> seances6 = {Seance(1, 9.0, records6a), Seance(2, 10.0, records6b)};
    Treatment treatment6(6, Patient(1, "a","b"), seances6);
    treatment6.IncrementSeance();
    QCOMPARE(treatment6.GetRemainingDoseActiveSeance(), (double)(10.0 + 9.0 - rec6_dose));

    // multiple seances planned, the first done, second partially done
    double dmoyen7a(3.75);
    int d17a(250);
    auto rec7a = GetSeanceRecord(d17a, dmoyen7a, 0.0);
    double dmoyen7b(3.75);
    int d17b(50);
    auto rec7b = GetSeanceRecord(d17b, dmoyen7b, 0.0);

    auto rec7a_dose = dmoyen7a * d17a / 100.0; // 9.375
    auto rec7b_dose = dmoyen7b * d17b / 100.0; // 1.875
    std::list<SeanceRecord> records7a = { rec7a };
    std::list<SeanceRecord> records7b = { rec7b };
    std::vector<Seance> seances7 = {Seance(1, 9.0, records7a), Seance(2, 10.0, records7b)};
    Treatment treatment7(7, Patient(1, "a","b"), seances7);
    treatment7.IncrementSeance();
    QCOMPARE(treatment7.GetRemainingDoseActiveSeance(), (double)(10.0 + 9.0 - rec7a_dose - rec7b_dose));

    // multiple seances planned, all done, but dose remains
    double dmoyen8a(3.75);
    int d8a(250);
    auto rec8a = GetSeanceRecord(d8a, dmoyen8a, 0.0);
    double dmoyen8b(3.75);
    int d8b(50);
    auto rec8b = GetSeanceRecord(d8b, dmoyen8b, 0.0);

    auto rec8a_dose = dmoyen8a * d8a / 100.0; // 9.375
    auto rec8b_dose = dmoyen8b * d8b / 100.0; // 1.875
    std::list<SeanceRecord> records8a = { rec8a };
    std::list<SeanceRecord> records8b = { rec8b };
    std::vector<Seance> seances8 = {Seance(1, 9.0, records8a), Seance(2, 10.0, records8b)};
    Treatment treatment8(8, Patient(1, "a","b"), seances8);
    treatment8.IncrementSeance();
    treatment8.IncrementSeance();
    QCOMPARE(treatment8.GetRemainingDoseActiveSeance(), (double)(10.0 + 9.0 - rec8a_dose - rec8b_dose));
}

void TestTreatment::GetDesiredDoseActiveSeance() {

    // single seance planned, none done
    std::list<SeanceRecord> records2;
    std::vector<Seance> seances2 = {Seance(1, 9.0, records2)};
    Treatment treatment2(2, Patient(1, "a","b"), seances2);
    QCOMPARE(treatment2.GetDesiredDoseActiveSeance(), 9.0f);

    // single seance planned, partially done
    double dmoyen3(3.75);
    int d13(80);
    auto rec3 = GetSeanceRecord(d13, dmoyen3, 0.0);
    std::list<SeanceRecord> records3 { rec3 };
    std::vector<Seance> seances3 = {Seance(1, 9.0, records3)};
    Treatment treatment3(3, Patient(1, "a","b"), seances3);
    QCOMPARE(treatment3.GetDesiredDoseActiveSeance(), (double)(9.0));

    // single seance planned,  done
    double dmoyen4(3.75);
    int d14(400); // -> Delivered dose is larger than dose prevu
    auto rec4 = GetSeanceRecord(d14, dmoyen4, 0.0);
    std::list<SeanceRecord> records4 = { rec4 };
    std::vector<Seance> seances4 = {Seance(1, 9.0, records4)};
    Treatment treatment4(4, Patient(1, "a","b"), seances4);
    QCOMPARE(treatment4.GetDesiredDoseActiveSeance(), 9.0f);

    // multiple seances planned, none done
    std::list<SeanceRecord> records5;
    std::vector<Seance> seances5 = {Seance(1, 9.0, records5), Seance(2, 10.0, records5)};
    Treatment treatment5(5, Patient(1, "a","b"), seances5);
    QCOMPARE(treatment5.GetDesiredDoseActiveSeance(), 9.0f);

    // multiple seances planned, the first done (but all dose not delivered)
    double dmoyen6(3.75);
    int d16(80);
    auto rec6 = GetSeanceRecord(d16, dmoyen6, 0.0);
    auto rec6_dose = dmoyen6 * d16 / 100.0;
    std::list<SeanceRecord> records6a = { rec6 };
    std::list<SeanceRecord> records6b;
    std::vector<Seance> seances6 = {Seance(1, 9.0, records6a), Seance(2, 10.0, records6b)};
    Treatment treatment6(6, Patient(1, "a","b"), seances6);
    treatment6.IncrementSeance();
    QCOMPARE(treatment6.GetDesiredDoseActiveSeance(), (double)(10.0 + 9.0 - rec6_dose));

    // multiple seances planned, the first done, second partially done
    double dmoyen7a(3.75);
    int d17a(250);
    auto rec7a = GetSeanceRecord(d17a, dmoyen7a, 0.0);
    double dmoyen7b(3.75);
    int d17b(50);
    auto rec7b = GetSeanceRecord(d17b, dmoyen7b, 0.0);

    auto rec7a_dose = dmoyen7a * d17a / 100.0; // 9.375
    std::list<SeanceRecord> records7a = { rec7a };
    std::list<SeanceRecord> records7b = { rec7b };
    std::vector<Seance> seances7 = {Seance(1, 9.0, records7a), Seance(2, 10.0, records7b)};
    Treatment treatment7(7, Patient(1, "a","b"), seances7);
    treatment7.IncrementSeance();
    QCOMPARE(treatment7.GetDesiredDoseActiveSeance(), (double)(10.0 + 9.0 - rec7a_dose));

}



void TestTreatment::GetUMPrevuActiveSeance() {
    const double temperature(23.0);
    const double pressure(1020.0);
    const int d1(967);
    const double charge(53.0);
    const Chambre chambre1(0, QDateTime(), "FWT-849", 25.94, 1.006, 0, 0);
    const double dref(1.37);
    std::list<SeanceRecord> seanceRecords;
    std::vector<Seance> seances = {Seance(1, 9.0, seanceRecords)};
    std::list<DosimetryRecord> dosimetryRecords;
    Dosimetry dosPatient(QDateTime::currentDateTime(), false, false, temperature, pressure, chambre1, dosimetryRecords, "");
    dosPatient.AddRecord(DosimetryRecord(d1, charge));
    Dosimetry dosRefPatient(QDateTime::currentDateTime(), false, false, temperature + 3.0, pressure + 50.0, chambre1, dosimetryRecords, "");
    dosRefPatient.AddRecord(DosimetryRecord(d1 + 10, charge + 5.0));
    Dosimetry dosRefToday(QDateTime::currentDateTime(), false, false, temperature + 1.0, pressure + 20.0, chambre1, dosimetryRecords, "");
    dosRefToday.AddRecord(DosimetryRecord(d1 + 30, charge + 2.0));

    // standard case
    Treatment treatment(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, dosRefPatient, dosRefToday, dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QCOMPARE(treatment.GetUMPrevuActiveSeance(), (int)calc::DoseToUM(treatment.GetBeamCalibration(), 9.0f));

    // no beam calibration done
    Treatment treatment2(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, dosRefPatient, Dosimetry(), dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QCOMPARE(treatment2.GetUMPrevuActiveSeance(), 0);
}

void TestTreatment::GetDeliveredDose() {
    // no seances planned
    std::vector<Seance> seances1;
    Treatment treatment1(1, Patient(1, "a","b"), seances1);
    QCOMPARE(treatment1.GetDeliveredDose(), 0.0f);

    // single seance planned, none done
    std::list<SeanceRecord> records2;
    std::vector<Seance> seances2 = {Seance(1, 9.0, records2)};
    Treatment treatment2(2, Patient(1, "a","b"), seances2);
    QCOMPARE(treatment2.GetDeliveredDose(), 0.0f);

    // single seance planned,  done
    double dmoyen4(3.75);
    int d14(400);
    auto rec4 = GetSeanceRecord(d14, dmoyen4, 0.0);
    std::list<SeanceRecord> records4 = { rec4 };
    std::vector<Seance> seances4 = {Seance(1, 9.0, records4)};
    Treatment treatment4(4, Patient(1, "a","b"), seances4);
    QCOMPARE(treatment4.GetDeliveredDose(), calc::UMToDose(dmoyen4, d14));

    // multiple seances planned, none done
    std::list<SeanceRecord> records5;
    std::vector<Seance> seances5 = {Seance(1, 9.0, records5), Seance(2, 10.0, records5)};
    Treatment treatment5(5, Patient(1, "a","b"), seances5);
    QCOMPARE(treatment5.GetDeliveredDose(), 0.0f);

    // multiple seances planned, the first done
    double dmoyen6(3.75);
    int d16(80);
    auto rec6 = GetSeanceRecord(d16, dmoyen6, 0.0);
    auto rec6_dose = dmoyen6 * d16 / 100.0;
    std::list<SeanceRecord> records6a = { rec6 };
    std::list<SeanceRecord> records6b;
    std::vector<Seance> seances6 = {Seance(1, 9.0, records6a), Seance(2, 10.0, records6b)};
    Treatment treatment6(6, Patient(1, "a","b"), seances6);
    treatment6.IncrementSeance();
    QCOMPARE(treatment6.GetDeliveredDose(), rec6_dose);

    // multiple seances planned, the first done, second partially done
    double dmoyen7a(3.75);
    int d17a(250);
    auto rec7a = GetSeanceRecord(d17a, dmoyen7a, 0.0);
    double dmoyen7b(3.75);
    int d17b(50);
    auto rec7b = GetSeanceRecord(d17b, dmoyen7b, 0.0);

    auto rec7a_dose = dmoyen7a * d17a / 100.0; // 9.375
    auto rec7b_dose = dmoyen7b * d17b / 100.0; // 1.875
    std::list<SeanceRecord> records7a = { rec7a };
    std::list<SeanceRecord> records7b = { rec7b };
    std::vector<Seance> seances7 = {Seance(1, 9.0, records7a), Seance(2, 10.0, records7b)};
    Treatment treatment7(7, Patient(1, "a","b"), seances7);
    treatment7.IncrementSeance();
    QCOMPARE(treatment7.GetDeliveredDose(), rec7a_dose + rec7b_dose);
}

void TestTreatment::GetEstimatedDeliveredDose() {
    // no seances planned
    std::vector<Seance> seances1;
    Treatment treatment1(1, Patient(1, "a","b"), seances1);
    QCOMPARE(treatment1.GetEstimatedDeliveredDose(), 0.0f);

    // single seance planned, none done
    std::list<SeanceRecord> records2;
    std::vector<Seance> seances2 = {Seance(1, 9.0, records2)};
    Treatment treatment2(2, Patient(1, "a","b"), seances2);
    QCOMPARE(treatment2.GetEstimatedDeliveredDose(), 0.0f);

    // single seance planned,  done
    auto rec4 = GetSeanceRecord(1.0, 1.0, 2.3f);
    std::list<SeanceRecord> records4 = { rec4 };
    std::vector<Seance> seances4 = {Seance(1, 9.0f, records4)};
    Treatment treatment4(4, Patient(1, "a","b"), seances4);
    QCOMPARE(treatment4.GetEstimatedDeliveredDose(), 2.3f);

    // multiple seances planned, none done
    std::list<SeanceRecord> records5;
    std::vector<Seance> seances5 = {Seance(1, 9.0, records5), Seance(2, 10.0, records5)};
    Treatment treatment5(5, Patient(1, "a","b"), seances5);
    QCOMPARE(treatment5.GetEstimatedDeliveredDose(), 0.0f);

    // multiple seances planned, the first done
    auto rec6 = GetSeanceRecord(1.0, 1.0, 3.5);
    std::list<SeanceRecord> records6a = { rec6 };
    std::list<SeanceRecord> records6b;
    std::vector<Seance> seances6 = {Seance(1, 9.0, records6a), Seance(2, 10.0, records6b)};
    Treatment treatment6(6, Patient(1, "a","b"), seances6);
    treatment6.IncrementSeance();
    QCOMPARE(treatment6.GetEstimatedDeliveredDose(), 3.5);

    // multiple seances planned, the first done, second partially done
    auto rec7a = GetSeanceRecord(1.0, 1.0, 1.5);
    auto rec7b = GetSeanceRecord(1.0, 1.0, 2.5);

    std::list<SeanceRecord> records7a = { rec7a };
    std::list<SeanceRecord> records7b = { rec7b };
    std::vector<Seance> seances7 = {Seance(1, 9.0, records7a), Seance(2, 10.0, records7b)};
    Treatment treatment7(7, Patient(1, "a","b"), seances7);
    treatment7.IncrementSeance();
    QCOMPARE(treatment7.GetEstimatedDeliveredDose(), 1.5 + 2.5);
}

void TestTreatment::GetPrescribedDose() {
    // no seances planned
    std::vector<Seance> seances1;
    Treatment treatment1(1, Patient(1, "a","b"), seances1);
    QCOMPARE(treatment1.GetPrescribedDose(), 0.0f);

    // single seance planned
    std::list<SeanceRecord> records2;
    std::vector<Seance> seances2 = {Seance(1, 9.0, records2)};
    Treatment treatment2(2, Patient(1, "a","b"), seances2);
    QCOMPARE(treatment2.GetPrescribedDose(), 9.0f);

    // multiple seances planned
    std::list<SeanceRecord> records5;
    std::vector<Seance> seances5 = {Seance(1, 9.0, records5), Seance(2, 10.0, records5)};
    Treatment treatment5(5, Patient(1, "a","b"), seances5);
    QCOMPARE(treatment5.GetPrescribedDose(), 9.0f + 10.0f);
}

void TestTreatment::BeamCalibrationDone() {
    std::vector<Seance> seances;
    std::list<DosimetryRecord> records;

    // measured dosi
    Dosimetry dos(QDateTime::currentDateTime(), false, false, 20.0, 1000.0, Chambre(0, QDateTime(), "FWT-849", 25.94, 1.006, 0, 90), records, "");

    Treatment treatmentAllDosiDone(20000, Patient(1, "Lucky", "Luke"), 0, seances, dos, dos, dos, 1.37, Modulateur(), DegradeurSet(), QString(""), QString(""));
    Treatment treatmentDosi1NotDone(20000, Patient(1, "Lucky", "Luke"), 0, seances, Dosimetry(), dos, dos, 1.37, Modulateur(), DegradeurSet(), QString(""), QString(""));
    Treatment treatmentDosi2NotDone(20000, Patient(1, "Lucky", "Luke"), 0, seances, dos, Dosimetry(), dos, 1.37, Modulateur(), DegradeurSet(), QString(""), QString(""));
    Treatment treatmentDosi3NotDone(20000, Patient(1, "Lucky", "Luke"), 0, seances, dos, dos, Dosimetry(), 1.37, Modulateur(), DegradeurSet(), QString(""), QString(""));

    QVERIFY(treatmentAllDosiDone.BeamCalibrationDone());
    QVERIFY(!treatmentDosi1NotDone.BeamCalibrationDone());
    QVERIFY(!treatmentDosi2NotDone.BeamCalibrationDone());
    QVERIFY(!treatmentDosi3NotDone.BeamCalibrationDone());

    // montecarlo dosi
    Dosimetry mcDos(QDateTime::currentDateTime(), false, true, 20.0, 1000.0, Chambre(0, QDateTime(),"FWT-849", 25.94, 1.006, 0, 0), records, "");

    Treatment treatmentAllDosiDoneMCPatientDos(20000, Patient(1, "Lucky", "Luke"), 0, seances, mcDos, dos, dos, 1.37, Modulateur(), DegradeurSet(), QString(""), QString(""));
    Treatment treatmentNoRefTopDeJourMCPatientDos(20000, Patient(1, "Lucky", "Luke"), 0, seances, mcDos, Dosimetry(), dos, 1.37, Modulateur(), DegradeurSet(), QString(""), QString(""));
    Treatment treatmentNoTDJMCPatientDos(20000, Patient(1, "Lucky", "Luke"), 0, seances, mcDos, Dosimetry(), Dosimetry(), 1.37, Modulateur(), DegradeurSet(), QString(""), QString(""));

    QVERIFY(treatmentAllDosiDoneMCPatientDos.BeamCalibrationDone());
    QVERIFY(treatmentNoRefTopDeJourMCPatientDos.BeamCalibrationDone());
    QVERIFY(!treatmentNoTDJMCPatientDos.BeamCalibrationDone());
}

void TestTreatment::GetBeamCalibrationMeasuredPatientDosimetry() {
    const double maxRelDiff(0.0001);
    const double temperature(23.0);
    const double pressure(1020.0);
    const int d1(967);
    const double charge(53.0);
    const double dref(1.37);
    bool isMonteCarlo(false);
    const Chambre chambre1(0, QDateTime(),"FWT-849", 25.94, 1.006, 0, 0);
    std::vector<Seance> seances;
    std::list<DosimetryRecord> records;

    // Standard case
    Dosimetry dosPatient(QDateTime::currentDateTime(), false, isMonteCarlo, temperature, pressure, chambre1, records, "");
    dosPatient.AddRecord(DosimetryRecord(d1, charge));
    Dosimetry dosRefPatient(QDateTime::currentDateTime(), false, false, temperature + 3.0, pressure + 50.0, chambre1, records, "");
    dosRefPatient.AddRecord(DosimetryRecord(d1 + 10, charge + 5.0));
    Dosimetry dosRefToday(QDateTime::currentDateTime(), false, false, temperature + 1.0, pressure + 20.0, chambre1, records, "");
    dosRefToday.AddRecord(DosimetryRecord(d1 + 30, charge + 2.0));
    Treatment treatment(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, dosRefPatient, dosRefToday, dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QVERIFY(treatment.BeamCalibrationDone());
    QVERIFY(calc::AlmostEqual(treatment.GetBeamCalibration(), dosPatient.GetDebitMean() * (dosRefToday.GetDebitMean() / dosRefPatient.GetDebitMean()), maxRelDiff));

    // Top du jour missing
    Treatment treatment2(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, dosRefPatient, Dosimetry(), dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QVERIFY(!treatment2.BeamCalibrationDone());
    QVERIFY(calc::AlmostEqual(treatment2.GetBeamCalibration(), 0.0f, maxRelDiff));

    // Top du jour on the day of the patient dosimetry is missing
    Treatment treatment3(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, Dosimetry(), dosRefToday, dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QVERIFY(!treatment3.BeamCalibrationDone());
    QVERIFY(calc::AlmostEqual(treatment3.GetBeamCalibration(), 0.0f, maxRelDiff));

    // Patient dosimetry missing
    Treatment treatment4(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        Dosimetry(), Dosimetry(), dosRefToday, dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QVERIFY(!treatment4.BeamCalibrationDone());
    QVERIFY(calc::AlmostEqual(treatment4.GetBeamCalibration(), 0.0f, maxRelDiff));

    // Everything is missing
    Treatment treatment5(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        Dosimetry(), Dosimetry(), Dosimetry(), dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QVERIFY(!treatment5.BeamCalibrationDone());
    QVERIFY(calc::AlmostEqual(treatment5.GetBeamCalibration(), 0.0f, maxRelDiff));
}

void TestTreatment::GetBeamCalibrationMonteCarloPatientDosimetry() {
    const double maxRelDiff(0.0001);
    const double temperature(23.0);
    const double pressure(1020.0);
    const int d1(967);
    const double charge(53.0);
    const double dref(1.37);
    const Chambre chambre1(0, QDateTime(),"FWT-849", 25.94, 1.006, 0, 0);
    std::vector<Seance> seances;
    std::list<DosimetryRecord> records;
    bool isMonteCarlo(true);

    // Standard case
    Dosimetry dosPatient(QDateTime::currentDateTime(), false, isMonteCarlo, temperature, pressure, chambre1, records, "");
    dosPatient.AddRecord(DosimetryRecord(d1, charge));
    Dosimetry dosRefPatient(QDateTime::currentDateTime(), false, false, temperature + 3.0, pressure + 50.0, chambre1, records, "");
    dosRefPatient.AddRecord(DosimetryRecord(d1 + 10, charge + 5.0));
    Dosimetry dosRefToday(QDateTime::currentDateTime(), false, false, temperature + 1.0, pressure + 20.0, chambre1, records, "");
    dosRefToday.AddRecord(DosimetryRecord(d1 + 30, charge + 2.0));
    Treatment treatment(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, dosRefPatient, dosRefToday, dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QVERIFY(treatment.BeamCalibrationDone());
    QVERIFY(calc::AlmostEqual(treatment.GetBeamCalibration(), dosPatient.GetDebitMean() * (dosRefToday.GetDebitMean() / dref), maxRelDiff));
    QVERIFY(!calc::AlmostEqual(treatment.GetBeamCalibration(), dosPatient.GetDebitMean() * (dosRefToday.GetDebitMean() / dosRefPatient.GetDebitMean()), maxRelDiff));

    // Top du jour missing
    Treatment treatment2(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, dosRefPatient, Dosimetry(), dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QVERIFY(!treatment2.BeamCalibrationDone());
    QVERIFY(calc::AlmostEqual(treatment2.GetBeamCalibration(), 0.0f, maxRelDiff));

    // Top du jour on the day of the patient dosimetry is missing (not needed for MC dosimetries so should not change anything)
    Treatment treatment3(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, Dosimetry(), dosRefToday, dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    QVERIFY(treatment3.BeamCalibrationDone());
    QVERIFY(calc::AlmostEqual(treatment.GetBeamCalibration(), dosPatient.GetDebitMean() * (dosRefToday.GetDebitMean() / dref), maxRelDiff));
}


void TestTreatment::GetSeanceConfig() {
    const double temperature(23.0);
    const double pressure(1020.0);
    const int d1(967);
    const double charge(53.0);
    const Chambre chambre1(0, QDateTime(), "FWT-849", 25.94, 1.006, 0, 0);
    const double dref(1.37);
    const double duration = 10.0;

    std::list<SeanceRecord> seanceRecords;
    std::vector<Seance> seances = {Seance(1, duration, seanceRecords)};

    std::list<DosimetryRecord> dosimetryRecords;
    Dosimetry dosPatient(QDateTime::currentDateTime(), false, false, temperature, pressure, chambre1, dosimetryRecords, "");
    dosPatient.AddRecord(DosimetryRecord(d1, charge));
    Dosimetry dosRefPatient(QDateTime::currentDateTime(), false, false, temperature + 3.0, pressure + 50.0, chambre1, dosimetryRecords, "");
    dosRefPatient.AddRecord(DosimetryRecord(d1 + 10, charge + 5.0));
    Dosimetry dosRefToday(QDateTime::currentDateTime(), false, false, temperature + 1.0, pressure + 20.0, chambre1, dosimetryRecords, "");
    dosRefToday.AddRecord(DosimetryRecord(d1 + 30, charge + 2.0));


    // no seances done
    Treatment treatment(20000, Patient(1, "Lucky", "Luke"), 0, seances,
                        dosPatient, dosRefPatient, dosRefToday, dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    SeanceConfig config1 = treatment.GetSeanceConfig();
    double act_duration(0.0);
    double i_stripper(0.0);
    util::GetStripperCurrentAndSeanceDuration(treatment.GetBeamCalibration(),
                                              treatment.GetRemainingDoseActiveSeance(),
                                              10.0, &i_stripper, &act_duration);
    SeanceConfig expConfig1("Lucky", 20000, 1, treatment.GetRemainingDoseActiveSeance(), treatment.GetBeamCalibration(),
                            treatment.GetUMPrevuActiveSeance(), 0, static_cast<uint32_t>(1000*act_duration), static_cast<uint32_t>(i_stripper));
    QCOMPARE(config1, expConfig1);

    // one seance done, 2nd beam on second seance
    double debit2a(1.80);
    int d2a(500);
    auto rec2a = GetSeanceRecord(d2a, debit2a, 0.0);
    double debit2b(1.1);
    int d2b(545);
    auto rec2b = GetSeanceRecord(d2b, debit2b, 0.0);

    std::list<SeanceRecord> records2a = { rec2a };
    std::list<SeanceRecord> records2b = { rec2b };
    std::vector<Seance> seances2 = {Seance(1, 9.0, records2a), Seance(2, 10.0, records2b)};

    Treatment treatment2(20000, Patient(1, "Lucky", "Luke"), 0, seances2,
                        dosPatient, dosRefPatient, dosRefToday, dref, Modulateur(), DegradeurSet(), QString(""), QString(""));
    treatment2.IncrementSeance();

    SeanceConfig config2 = treatment2.GetSeanceConfig();
    double debit2 = treatment2.GetBeamCalibration();
    double act_duration2;
    double current_stripper2;
    util::GetStripperCurrentAndSeanceDuration(debit2, treatment2.GetRemainingDoseActiveSeance(),
                                              10.0, &current_stripper2, &act_duration2);
    SeanceConfig expConfig2("Lucky", 20000, 2, treatment2.GetRemainingDoseActiveSeance(),
                                                 treatment2.GetBeamCalibration(),
                                                 treatment2.GetUMPrevuActiveSeance(),
                                                 0, static_cast<uint32_t>(1000*act_duration2), static_cast<uint32_t>(current_stripper2));
    QCOMPARE(config2, expConfig2);

    // all seances done, but dose remaining
    treatment2.IncrementSeance();
    SeanceConfig expConfig3("Lucky", 20000, 0, treatment2.GetRemainingDoseActiveSeance(), 0.0, 0, 0, 0, 0);
    QCOMPARE(treatment2.GetSeanceConfig(), expConfig3);
}

void TestTreatment::CloseDossier() {
    std::vector<Seance> seances;
    std::list<SeanceRecord> records;
    seances.push_back(Seance(1, 1.0, records));
    seances.push_back(Seance(2, 2.0, records));

    Treatment t1(1, Patient(1, "a","b"), seances);
    Treatment t2(1, Patient(1, "a","b"), seances);

    QVERIFY(!t1.IsFinished());
    t1.Close();
    QVERIFY(t1.IsFinished());

    QVERIFY(!t2.IsFinished());
    t2.IncrementSeance();
    QVERIFY(!t2.IsFinished());
    t2.Close();
    QVERIFY(t2.IsFinished());
}
