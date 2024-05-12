#include "TestPTRepo.h"

#include <iostream>
#include <QtDebug>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QDateTime>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>
#include <map>

#include "Chambre.h"
#include "PTRepo.h"
#include "Dosimetry.h"
#include "DosimetryRecord.h"
#include "Seance.h"
#include "Calc.h"

void TestPTRepo::initTestCase() {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MTD.ini"), QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    dbHostName_ = settings.value("unittestdatabase/host", "unknown").toString();
    dbPort_ = settings.value("unittestdatabase/port", "5432").toInt();
    dbDatabaseName_ = settings.value("unittestdatabase/name", "unknown").toString();
    dbUserName_ = settings.value("unittestdatabase/user", "unknown").toString();
    dbPassword_ = settings.value("unittestdatabase/password", "unknown").toString();
    dbConnName_ = "unittestSetup";
    qDebug() << "DB host " << dbHostName_;
    qDebug() << "DB name " << dbDatabaseName_;
    if (dbHostName_ == "unknown") qCritical() << "TestPTRepo::inittestCase Unknown db hostname";
    if (dbDatabaseName_ == "unknown") qCritical() << "TestPTRepo::inittestCase Unknown db name";
    if (dbUserName_ == "unknown") qCritical() << "TestPTRepo::inittestCase Unknown db username";
    if (dbPassword_ == "unknown") qCritical() << "TestPTRepo::inittestCase Unknown db password";

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL7", dbConnName_);
    db.setHostName(dbHostName_);
    db.setPort(dbPort_);
    db.setDatabaseName(dbDatabaseName_);
    db.setUserName(dbUserName_);
    db.setPassword(dbPassword_);
    if (!db.open()) {
        qWarning() << "Dd could not be opened";
    }

    if (!LoadSqlResourceFile(":/scripts/createPatient.sql")) AbortUnitTests("createPatient");
    if (!LoadSqlResourceFile(":/scripts/createModulateur.sql")) AbortUnitTests("createModulateur");
    if (!LoadSqlResourceFile(":/scripts/createDegradeur.sql")) AbortUnitTests("createDegradeur");
    if (!LoadSqlResourceFile(":/scripts/createChambre.sql")) AbortUnitTests("createChambre");
    if (!LoadSqlResourceFile(":/scripts/createTreatmentType.sql")) AbortUnitTests("createTreatmentType");
    if (!LoadSqlResourceFile(":/scripts/createTreatment.sql")) AbortUnitTests("createTreatment");
    if (!LoadSqlResourceFile(":/scripts/createActiveTreatment.sql")) AbortUnitTests("createActiveTreatment");
    if (!LoadSqlResourceFile(":/scripts/createCollimateur.sql")) AbortUnitTests("createCollimateur");
    if (!LoadSqlResourceFile(":/scripts/createCompensateur.sql")) AbortUnitTests("createCompensateur");
    if (!LoadSqlResourceFile(":/scripts/createMCNPXDoseCurve.sql")) AbortUnitTests("createMCNPXDoseCurve");
    if (!LoadSqlResourceFile(":/scripts/createFraiseuseProgram.sql")) AbortUnitTests("createFraiseuseProgram");
    if (!LoadSqlResourceFile(":/scripts/createSeance.sql")) AbortUnitTests("createSeance");
    if (!LoadSqlResourceFile(":/scripts/createDosimetrySummary.sql")) AbortUnitTests("createDosimetrySummary");
    if (!LoadSqlResourceFile(":/scripts/createDosimetry.sql")) AbortUnitTests("createDosimetry");
    if (!LoadSqlResourceFile(":/scripts/createDosimetryRecord.sql")) AbortUnitTests("createDosimetryRecord");
    if (!LoadSqlResourceFile(":/scripts/createSeanceRecord.sql")) AbortUnitTests("createSeanceRecord");
    if (!LoadSqlResourceFile(":/scripts/createBaliseLevel.sql")) AbortUnitTests("createBaliseLevel");
    if (!LoadSqlResourceFile(":/scripts/createBaliseRecord.sql")) AbortUnitTests("createBaliseRecord");
    if (!LoadSqlResourceFile(":/scripts/createBaliseCalibration.sql")) AbortUnitTests("createBaliseCalibration");
    if (!LoadSqlResourceFile(":/scripts/createBaliseConfiguration.sql")) AbortUnitTests("createBaliseConfiguration");
    if (!LoadSqlResourceFile(":/scripts/createXRayRecord.sql")) AbortUnitTests("createXRayRecord");
    if (!LoadSqlResourceFile(":/scripts/createDefaults.sql")) AbortUnitTests("createDefaults");
    if (!LoadSqlResourceFile(":/scripts/createCheckConnection.sql")) AbortUnitTests("checkConnection");
   // if (!LoadSqlResourceFile(":/scripts/createOIDS.sql")) AbortUnitTests("createOIDS");
}


void TestPTRepo::AbortUnitTests(QString msg) {
    qCritical() << "TestPTRepo Tests aborted: " << msg;
    cleanupTestCase();
    QFAIL(msg.toStdString().c_str());
}

bool TestPTRepo::LoadSqlResourceFile(QString file) {
    qDebug() << "TestPTRepo::loadsqlresourcefile " << file;
    QSqlQuery query(QSqlDatabase::database(dbConnName_));

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestPTRepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    return query.exec(cmd);
}

void TestPTRepo::cleanupTestCase() {
    qDebug() << "TestPTRepo::cleanuptestcase";
    QSqlQuery query(QSqlDatabase::database(dbConnName_));

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "Query create schema failed" << query.lastError();
    }
    query.clear();

    QSqlDatabase::removeDatabase(dbConnName_);
}

void TestPTRepo::init() {
    qDebug() << "TestPTRepo::init";
    if (!LoadSqlResourceFile(":/scripts/setupUnitTests.sql")) {
        qDebug() << "COULD NOT RUN SETUPUNITTESTS.SQL";
        AbortUnitTests("init");
    } else {
        qDebug() << "SUCCESSFULLY RUN SETUPUNITTESTS.SQL";
    }
}

void TestPTRepo::cleanup() {
    qDebug() << "TestPTRepo::cleanup";
     QSqlQuery query(QSqlDatabase::database(dbConnName_));
     if (!query.exec("TRUNCATE Chambre, Degradeur, Modulateur, DosimetrySummary, Dosimetry, DosimetryRecord, Seance, SeanceRecord, BaliseInstantaneousLevel,"
                     "BaliseIntegratedLevel, BaliseBufferLevel, BaliseRecord, BaliseCalibration, BaliseConfiguration, TreatmentType, CheckConnection, "
                     "XRayRecord, Patient, Treatment, ActiveTreatment, Defaults, MCNPXDoseCurve, Collimateur, Compensateur, TreatmentFraiseuseProgram, MiscFraiseuseProgram "
                     "RESTART IDENTITY")) {
         qWarning() << "Query truncate tables failed: " << query.lastError();
     }
     query.clear();
}

void TestPTRepo::GetChambreUsingId() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre1 = repo.GetChambre(1);
        QCOMPARE(chambre1, Chambre(1, QDateTime(QDate(2017, 1, 1)), QString("FWT-849"), 25.940, 1.006, 11, 22));
        auto chambre2 = repo.GetChambre(2);
        QCOMPARE(chambre2, Chambre(2, QDateTime(QDate(2017, 1, 1)), QString("PTW-593"), 31.090, 1.030, 11, 22));
        try {
            repo.GetChambre(3);
            QFAIL("TestPTRepo::GetChambreUsingId Should have thrown 1");
        }
        catch (...){}
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetChambreUsingId");
    }
}

void TestPTRepo::GetChambreUsingNameReturnsLast() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        // test 1
        auto chambre1 = repo.GetChambre("FWT-849");
        QCOMPARE(chambre1, Chambre(1, QDateTime(QDate(2017, 1, 1)), QString("FWT-849"), 25.940, 1.006, 11, 22));

        // test 2
        QSqlQuery query1(QSqlDatabase::database(dbConnName_));
        query1.prepare("INSERT INTO Chambre(timestamp, name, nd, kqq0, yoffset, zoffset) "
                       "VALUES(:timestamp, :name, :nd, :kqq0, :yoffset, :zoffset)");
        query1.bindValue(":timestamp", QDateTime(QDate(2017, 2, 2)));
        query1.bindValue(":name", QString("FWT-849"));
        query1.bindValue(":nd", 26.940);
        query1.bindValue(":kqq0", 2.006);
        query1.bindValue(":yoffset", 11);
        query1.bindValue(":zoffset", 22);
        if (!query1.exec()) {
            QFAIL("GetChambreUsingNameReturnsLast Could not insert chambre ");
        }
        QSqlQuery query2(QSqlDatabase::database(dbConnName_));
        query2.prepare("INSERT INTO Chambre(timestamp, name, nd, kqq0, yoffset, zoffset) "
                       "VALUES(:timestamp, :name, :nd, :kqq0, :yoffset, :zoffset)");
        query2.bindValue(":timestamp", QDateTime(QDate(2017, 3, 3)));
        query2.bindValue(":name", "PTW-593");
        query2.bindValue(":nd", 26.940);
        query2.bindValue(":kqq0", 2.006);
        query2.bindValue(":yoffset", 11);
        query2.bindValue(":zoffset", 22);
        if (!query2.exec()) {
            QFAIL("GetChambreUsingNameReturnsLast Could not insert chambre");
        }

        auto chambre2 = repo.GetChambre("FWT-849");
        QCOMPARE(chambre2, Chambre(3, QDateTime(QDate(2017, 2, 2)), QString("FWT-849"), 26.940, 2.006, 11, 22));

        try {
            repo.GetChambre("DOESNTEXIST");
            QFAIL("TestPTRepo::GetChambreUsingNameReturnsLast Should have thrown 1");
        }
        catch (...){}
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetChambreUsingNameReturnsLast");
    }
}


bool TestPTRepo::InsertDegradeur(int id, int um_plexi, bool available) {
    try {
        QString query_string = QString("INSERT INTO Degradeur(degradeur_id, um_plexi, available) VALUES('%1',%2,%3);")
                .arg(id)
                .arg(QString::number(um_plexi))
                .arg(available ? "true" : "false");
        QSqlQuery query(QSqlDatabase::database(dbConnName_));
        return query.exec(query_string);
    }
    catch (...) {
        return false;
    }
}

bool TestPTRepo::InsertModulateur(int id, double mod100, double mod98, double parcours, int degradeur, int thickness, double weight) {
    try {
        QString query_string = QString("INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, um_degradeur, um_plexi_weight, available, "
                                       " opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod, decalage, conv_criteria)"
                                       " VALUES(%1,%2,%3,%4,%5,'{\"(%6, %7)\"}', true, 'Joel99', 'Shifted bragg', 4, 0.8, 20.0, 10.0, 0.4, 0.0001);")
                .arg(id)
                .arg(QString::number(mod100))
                .arg(QString::number(mod98))
                .arg(QString::number(parcours))
                .arg(QString::number(degradeur))
                .arg(QString::number(thickness))
                .arg(QString::number(weight, 'f', 2));
        QSqlQuery query(QSqlDatabase::database(dbConnName_));
        return query.exec(query_string);
    }
    catch (...) {
        return false;
    }
}

void TestPTRepo::DossierExist() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        QCOMPARE(repo.DossierExist(10000), true);
        QCOMPARE(repo.DossierExist(10001), false);
    }
    catch(...) {
        QFAIL("An exception was thrown in test DossierExist");
    }
}

void TestPTRepo::PatientExist() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        QVERIFY(repo.PatientExist(10000));
        QVERIFY(!repo.PatientExist(1));
    }
    catch(...) {
        QFAIL("An exception was thrown in test PatientExist");
    }
}

void TestPTRepo::GetPatient() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        Patient patient = repo.GetPatient(10000);
        QCOMPARE(patient.GetFirstName(), QString("REF"));
        QCOMPARE(patient.GetLastName(), QString("REF"));
    }
    catch(...) {
        QFAIL("An exception was thrown in test GetPatient");
    }
}

void TestPTRepo::SavePatient() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        Patient patient("Luke", "Skywalker");
        const int patient_id = repo.SavePatient(patient);
        auto patients = repo.GetPatients("Luke", "Skywalker");
        QVERIFY((int)patients.size() == (int)1);
        QVERIFY(patients.at(0).SameNameAs(patient));
        QVERIFY(patient_id > 0);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test SavePatient");
    }
}

void TestPTRepo::GetDosimetryByTimestamp() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");
        const int dossier(10000);

        const QDateTime timestamp1(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3)));
        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        Dosimetry dosimetry1(timestamp1, false, false,
                             23.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry1);

        const QDateTime timestamp2(QDateTime(QDate(2015, 1, 1), QTime(1, 3, 3)));
        Dosimetry dosimetry2(timestamp2, false, false,
                             24.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry2);

        Dosimetry retrDosimetry = repo.GetDosimetry(timestamp1);
        QCOMPARE(dosimetry1, retrDosimetry);

        try {
            repo.GetDosimetry(QDateTime(QDate(2016, 1, 1)));
            QFAIL("TestPTRepo::GetDosimetryByTimestamp Test should have thrown 1");
        }
        catch (...) {}
    }

    catch(...) {
        QFAIL("An exception was thrown in the test SavedAndRetrievedDosimetryAr");
    }
}

void TestPTRepo::GetMeasuredDosimetry() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");
        const int dossier(10000);

        const QDateTime timestamp1(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3)));
        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        Dosimetry dosimetry1(timestamp1, false, true, // monte_carlo = true
                             23.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry1);

        const QDateTime timestamp2(QDateTime(QDate(2016, 1, 1), QTime(1, 3, 3)));
        Dosimetry dosimetry2(timestamp2, false, false, // monte_carlo = false
                             24.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry2);

        const QDateTime timestamp3(QDateTime(QDate(2015, 1, 1), QTime(1, 3, 3)));
        Dosimetry dosimetry3(timestamp3, false, false, // monte_carlo = false
                             24.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry3);

        Dosimetry retrDosimetry = repo.GetMeasuredDosimetry(dossier);
        QCOMPARE(dosimetry2, retrDosimetry);
        QVERIFY(retrDosimetry != dosimetry1);
        QVERIFY(retrDosimetry != dosimetry3);
    }

    catch(...) {
        QFAIL("An exception was thrown in the test GetMeasuredDosimetry");
    }
}

void TestPTRepo::GetLastMeasuredDosimetry() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");
        repo.CreateTreatment(20000, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        const QDateTime timestamp1(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3)));
        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        Dosimetry dosimetry1(timestamp1, false, true, // monte_carlo = true
                             23.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(20000, dosimetry1);

        const QDateTime timestamp2(QDateTime(QDate(2016, 1, 1), QTime(1, 3, 3)));
        Dosimetry dosimetry2(timestamp2, false, false, // monte_carlo = false
                             24.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(20000, dosimetry2);

        const QDateTime timestamp3(QDateTime(QDate(2017, 1, 1), QTime(1, 3, 3)));
        Dosimetry dosimetry3(timestamp3, true, false, // monte_carlo = true
                             24.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(10000, dosimetry3);

        Dosimetry retrDosimetry = repo.GetLastMeasuredPatientDosimetry();
        QCOMPARE(dosimetry2, retrDosimetry);
        QVERIFY(retrDosimetry != dosimetry1);
        QVERIFY(retrDosimetry != dosimetry3);
    }

    catch(...) {
        QFAIL("An exception was thrown in the test GetLastMeasuredDosimetry");
    }
}

void TestPTRepo::GetDossierForDosimetry() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");

        const QDateTime timestamp1(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3)));
        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        Dosimetry dosimetry1(timestamp1, false, true, // monte_carlo = true
                             23.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(10000, dosimetry1);

        QCOMPARE(10000, repo.GetDossierForDosimetry(timestamp1));
    }

    catch(...) {
        QFAIL("An exception was thrown in the test GetLastMeasuredDosimetry");
    }
}

void TestPTRepo::GetMCDosimetry() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");
        const int dossier(10000);

        const QDateTime timestamp1(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3)));
        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        Dosimetry dosimetry1(timestamp1, false, true, // monte_carlo = true
                             23.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry1);

        const QDateTime timestamp2(QDateTime(QDate(2016, 1, 1), QTime(1, 3, 3)));
        Dosimetry dosimetry2(timestamp2, false, false, // monte_carlo = false
                             24.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry2);

        const QDateTime timestamp3(QDateTime(QDate(2015, 1, 1), QTime(1, 3, 3)));
        Dosimetry dosimetry3(timestamp3, false, true, // monte_carlo = true
                             24.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry3);

        Dosimetry retrDosimetry = repo.GetMCDosimetry(dossier);
        QCOMPARE(dosimetry3, retrDosimetry);
        QVERIFY(retrDosimetry != dosimetry1);
        QVERIFY(retrDosimetry != dosimetry2);
    }

    catch(...) {
        QFAIL("An exception was thrown in the test GetMCDosimetry");
    }
}

void TestPTRepo::SaveMonteCarloDosimetry() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        const double debit1(1.12345);
        repo.SaveMonteCarloDosimetry(10001, debit1);
        Dosimetry retr1 = repo.GetMCDosimetry(10001);
        const double retr_debit1 = retr1.GetDebitMean();
        QVERIFY(calc::AlmostEqual(debit1, retr_debit1, 0.0001));

        const double debit2(0.34567);
        repo.SaveMonteCarloDosimetry(10001, debit2);
        Dosimetry retr2 = repo.GetMCDosimetry(10001);
        const double retr_debit2 = retr2.GetDebitMean();
        QVERIFY(calc::AlmostEqual(debit2, retr_debit2, 0.0001));

        const double debit3(0.5);
        repo.SaveMonteCarloDosimetry(10001, debit3);
        Dosimetry retr3 = repo.GetMCDosimetry(10001);
        const double retr_debit3 = retr3.GetDebitMean();
        QVERIFY(calc::AlmostEqual(debit3, retr_debit3, 0.0001));
        QVERIFY(!calc::AlmostEqual(debit3, retr_debit3 + 0.001, 0.0001));
    }
    catch(std::exception& exc) {
        QFAIL("SaveMonteCarloDosimetry Exception thrown in test");
    }
}

void TestPTRepo::SaveMonteCarloDosimetry_ErrorCases() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        try {
            repo.SaveMonteCarloDosimetry(10000, 1.0);
            QFAIL("SaveMonteCarloDosimetry_ErrorCases Should have thrown1");
        }
        catch (std::exception& exc) {}

        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));
        try {
            repo.SaveMonteCarloDosimetry(10001, 0.0);
            QFAIL("SaveMonteCarloDosimetry_ErrorCases Should have thrown2");
        }
        catch (std::exception& exc) {}
    }
    catch(std::exception& exc) {
        QFAIL("SaveMonteCarloDosimetry_ErrorCases Exception thrown in test");
    }
}

void TestPTRepo::DosimetryIsRetrievedWithCorrectChambre() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");
        QSqlQuery query1(QSqlDatabase::database(dbConnName_));
        query1.prepare("INSERT INTO Chambre(timestamp, name, nd, kqq0, yoffset, zoffset) "
                       "VALUES(:timestamp, :name, :nd, :kqq0, :yoffset, :zoffset)");
        query1.bindValue(":timestamp", QDateTime(QDate(2017, 2, 2)));
        query1.bindValue(":name", QString("FWT-849"));
        query1.bindValue(":nd", 26.940);
        query1.bindValue(":kqq0", 2.006);
        query1.bindValue(":yoffset", 11);
        query1.bindValue(":zoffset", 22);
        if (!query1.exec()) {
            QFAIL("GetChambreUsingNameReturnsLast Could not insert chambre ");
        }

        const int dossier(10000);
        const QDateTime timestamp1(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3)));
        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        Dosimetry dosimetry1(timestamp1, false, false,
                             23.0, 1001,
                             chambre,
                             records, "");
        repo.SaveDosimetry(dossier, dosimetry1);

        Dosimetry retrDosimetry = repo.GetDosimetry(timestamp1);
        QCOMPARE(chambre, retrDosimetry.GetChambre());
    }
    catch(...) {
        QFAIL("An exception was thrown in the test DosimetryIsRetrievedWithCorrectChambre");
    }
}


void TestPTRepo::SavedAndRetrievedPatientDosimetryAreEqual() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");
        const int dossier(10000);

        // single value dosimetryrecords
        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        Dosimetry dosimetry1(QDateTime::currentDateTime(), false, false,
                            23.0, 1001,
                            chambre,
                            records, "");

        repo.SaveDosimetry(dossier, dosimetry1);
        Dosimetry retrDosimetry = repo.GetPatientDosimetry(dossier);
        QCOMPARE(dosimetry1, retrDosimetry);

        // multiple value dosimetryrecords
        std::vector<double> duration {1.1, 2.1, 3.1, 4.1};
        std::vector<int> um_del {4, 8, 12};
        std::vector<double> i_chambre1 {100.2, 50, 200.4, 66.6};
        std::vector<double> i_chambre2 {300.8, 44.5, 33};
        DosimetryRecord record3(duration, i_chambre1, i_chambre2, 33.0, 44.0, 400, um_del, 333.0, 123.0);
        std::list<DosimetryRecord> records3 = {record3};
        Dosimetry dosimetry2(QDateTime::currentDateTime(), true, false,
                            22.0, 1002,
                            chambre,
                            records3, "");

        repo.SaveDosimetry(dossier, dosimetry2);
        Dosimetry retrDosimetry2 = repo.GetPatientDosimetry(dossier);
        QCOMPARE(dosimetry2, retrDosimetry2);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test SavedAndRetrievedDosimetryAr");
    }
}

void TestPTRepo::SavedAndRetrievedPatientDosimetryPrecision() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");
        const int dossier(10000);

        // max debit possible
        std::list<DosimetryRecord> records1 {DosimetryRecord(1, 1000.0)};
        Dosimetry dosimetry1(QDateTime::currentDateTime(), false, false,
                            50.0, 800.0,
                            chambre,
                            records1, "");

        repo.SaveDosimetry(dossier, dosimetry1);
        Dosimetry retrDosimetry1 = repo.GetPatientDosimetry(dossier);
        QVERIFY(calc::AlmostEqual(dosimetry1.GetDebitMean(), 36434.16658014, 0.00000000001));
        QVERIFY(calc::AlmostEqual(dosimetry1.GetDebitMean(), retrDosimetry1.GetDebitMean(), 0.00000000001));

        // min debit possible
        std::list<DosimetryRecord> records2 {DosimetryRecord(10000, 1.0)};
        Dosimetry dosimetry2(QDateTime::currentDateTime(), false, false,
                            0.0, 1200.0,
                            chambre,
                            records2, "");

        repo.SaveDosimetry(dossier, dosimetry2);
        Dosimetry retrDosimetry2 = repo.GetPatientDosimetry(dossier);
        QVERIFY(calc::AlmostEqual(dosimetry2.GetDebitMean(), 0.0020531214, 0.0000001));
        QVERIFY(calc::AlmostEqual(dosimetry2.GetDebitMean(), retrDosimetry2.GetDebitMean(), 0.00000001));

    }
    catch(...) {
        QFAIL("An exception was thrown in the test SavedAndRetrievedDosimetryAr");
    }
}


void TestPTRepo::LastPatientDosimetryInDossierIsReturned() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");
        const int dossier(10000);

        // test1: single dosimetry in db
        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        Dosimetry dosimetry1(QDateTime::currentDateTime(), false, false,
                            23.0, 1001,
                            chambre,
                            records, "");

        repo.SaveDosimetry(dossier, dosimetry1);
        Dosimetry retrDosimetry = repo.GetPatientDosimetry(dossier);
        QCOMPARE(dosimetry1, retrDosimetry);

        // test2: two dosimetries in db with same dossier number
        Dosimetry dosimetry2(QDateTime::currentDateTime(), true, true,
                            33.0, 2001,
                            chambre,
                            records, "");

        repo.SaveDosimetry(dossier, dosimetry2);
        Dosimetry retrDosimetry2 = repo.GetPatientDosimetry(dossier);
        QCOMPARE(dosimetry2, retrDosimetry2);

        // test3: dosimetries with different dossier numbers in db
        const int dossier2(10001);
        repo.CreateTreatment(dossier2, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));
        Dosimetry dosimetry3(QDateTime::currentDateTime(), true, true,
                            43.0, 2005,
                            chambre,
                            records, "");

        repo.SaveDosimetry(dossier2, dosimetry3);
        Dosimetry retrDosimetry3 = repo.GetPatientDosimetry(dossier);
        QCOMPARE(dosimetry2, retrDosimetry3);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test LastPatientDosimetryInDossierIsReturned");
    }
}

void TestPTRepo::LastReferenceBeforePatientDosimetryIsReturned() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        auto chambre = repo.GetChambre("FWT-849");
        bool reference(true);
        Dosimetry dosimetry1(QDateTime(QDate(2016, 8, 10), QTime(10, 20)), reference, false,
                            23.0, 1001,
                            chambre,
                            records, "");
        Dosimetry dosimetry2(QDateTime(QDate(2016, 8, 10), QTime(10, 21)), reference, false,
                            22.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry3(QDateTime(QDate(2016, 8, 10), QTime(10, 22)), !reference, false,
                            23.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry4(QDateTime(QDate(2016, 8, 12), QTime(10, 23)), reference, false,
                            24.0, 1003,
                            chambre,
                            records, "");

        const int dossier(10000);
        repo.SaveDosimetry(dossier, dosimetry1);
        repo.SaveDosimetry(dossier, dosimetry2);
        repo.SaveDosimetry(10001, dosimetry3);
        repo.SaveDosimetry(dossier, dosimetry4);

        try {
            repo.GetReferenceDosimetry(QDateTime(QDate(2016, 8, 9), QTime(1, 1)));
            QFAIL("LastReferenceBeforePatientDosimetryIsReturned should have thrown 1");
        }
        catch (...) {}

        QCOMPARE(repo.GetReferenceDosimetry(QDateTime(QDate(2016, 8, 10), QTime(10, 20, 1))), dosimetry1);
        QCOMPARE(repo.GetReferenceDosimetry(QDateTime(QDate(2016, 8, 10), QTime(10, 21, 1))), dosimetry2);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test LastReferenceBeforePatientDosimetryIsReturned");
    }
}

void TestPTRepo::FirstReferenceAfterPatientDosimetryIsReturned() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        auto chambre = repo.GetChambre("FWT-849");
        bool reference(true);
        Dosimetry dosimetry1(QDateTime(QDate(2016, 8, 10), QTime(10, 20)), reference, false,
                            23.0, 1001,
                            chambre,
                            records, "");
        Dosimetry dosimetry2(QDateTime(QDate(2016, 8, 10), QTime(10, 21)), reference, false,
                            22.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry3(QDateTime(QDate(2016, 8, 10), QTime(10, 19)), !reference, false,
                            23.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry4(QDateTime(QDate(2016, 8, 10), QTime(10, 18)), !reference, false,
                            23.0, 1000,
                            chambre,
                            records, "");


        const int dossier(10000);
        repo.SaveDosimetry(dossier, dosimetry1);
        repo.SaveDosimetry(dossier, dosimetry2);
        repo.SaveDosimetry(10001, dosimetry3);
        repo.SaveDosimetry(dossier, dosimetry4);

        QCOMPARE(repo.GetReferenceDosimetry(QDateTime(QDate(2016, 8, 10), QTime(10, 1))), dosimetry1);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test FirstReferenceAfterPatientDosimetryIsReturned");
    }
}


void TestPTRepo::LastTopDeJourDosimetryOnDayIsReturned() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        auto chambre = repo.GetChambre("FWT-849");
        bool reference(true);
        Dosimetry dosimetry1(QDateTime(QDate(2016, 8, 10), QTime(10, 20)), reference, false,
                            23.0, 1001,
                            chambre,
                            records, "");
        Dosimetry dosimetry2(QDateTime(QDate(2016, 8, 10), QTime(10, 21)), reference, false,
                            22.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry3(QDateTime(QDate(2016, 8, 10), QTime(10, 22)), !reference, false,
                            23.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry4(QDateTime(QDate(2016, 8, 12), QTime(10, 23)), reference, false,
                            24.0, 1003,
                            chambre,
                            records, "");

        const int dossier(10000);
        repo.SaveDosimetry(dossier, dosimetry1);
        repo.SaveDosimetry(dossier, dosimetry2);
        repo.SaveDosimetry(10001, dosimetry3);
        repo.SaveDosimetry(dossier, dosimetry4);
        Dosimetry retrDosimetry = repo.GetTopDeJourDosimetry(QDate(2016, 8, 10));
        QCOMPARE(dosimetry2, retrDosimetry);
        QVERIFY(!(dosimetry1 == retrDosimetry));
        QVERIFY(!(dosimetry3 == retrDosimetry));
        QVERIFY(!(dosimetry4 == retrDosimetry));
    }
    catch(...) {
        QFAIL("An exception was thrown in the test LastTopDeJourDosimetryOnDayIsReturned");
    }
}

void TestPTRepo::LastTopDeJourDosimetryOnDayIsReturnedShuffled() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        auto chambre = repo.GetChambre("FWT-849");
        bool reference(true);
        Dosimetry dosimetry1(QDateTime(QDate(2016, 8, 10), QTime(10, 20)), reference, false,
                            23.0, 1001,
                            chambre,
                            records, "");
        Dosimetry dosimetry2(QDateTime(QDate(2016, 8, 10), QTime(10, 21)), reference, false,
                            22.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry3(QDateTime(QDate(2016, 8, 10), QTime(10, 22)), !reference, false,
                            23.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry4(QDateTime(QDate(2016, 8, 12), QTime(10, 23)), reference, false,
                            24.0, 1003,
                            chambre,
                            records, "");

        const int dossier(10000);
        repo.SaveDosimetry(dossier, dosimetry4);
        repo.SaveDosimetry(10001, dosimetry3);
        repo.SaveDosimetry(dossier, dosimetry2);
        repo.SaveDosimetry(dossier, dosimetry1);

        Dosimetry retrDosimetry = repo.GetTopDeJourDosimetry(QDate(2016, 8, 10));
        QCOMPARE(dosimetry2, retrDosimetry);
        QVERIFY(!(dosimetry1 == retrDosimetry));
        QVERIFY(!(dosimetry3 == retrDosimetry));
        QVERIFY(!(dosimetry4 == retrDosimetry));
    }
    catch(...) {
        QFAIL("An exception was thrown in the test LastTopDeJourDosimetryOnDayIsReturnedShuffled");
    }
}

void TestPTRepo::GetDebitsSinceDate() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        DosimetryRecord record1(2, 0.44);
        DosimetryRecord record2(4, 0.88);
        std::list<DosimetryRecord> records = {record1, record2};
        auto chambre = repo.GetChambre("FWT-849");
        bool reference(true);
        Dosimetry dosimetry1(QDateTime(QDate(2016, 8, 10), QTime(10, 20)), reference, false,
                            23.0, 1001,
                            chambre,
                            records, "");
        Dosimetry dosimetry2(QDateTime(QDate(2016, 8, 10), QTime(10, 21)), reference, false,
                            22.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry3(QDateTime(QDate(2016, 8, 10), QTime(10, 22)), !reference, false,
                            23.0, 1000,
                            chambre,
                            records, "");
        Dosimetry dosimetry4(QDateTime(QDate(2016, 8, 12), QTime(10, 23)), reference, false,
                            24.0, 1003,
                            chambre,
                            records, "");

        const int dossier(10000);

        QVERIFY(repo.GetDebits(dossier, QDate(2016, 8, 10), QDate(2018, 8, 10)).empty());

        repo.SaveDosimetry(dossier, dosimetry4);
        repo.SaveDosimetry(10001, dosimetry3);
        repo.SaveDosimetry(dossier, dosimetry2);
        repo.SaveDosimetry(dossier, dosimetry1);

        std::vector<Debit> debits124 { Debit(dosimetry1.GetDebit()),
                                       Debit(dosimetry2.GetDebit()),
                                       Debit(dosimetry4.GetDebit()) };

        QCOMPARE(repo.GetDebits(dossier, QDate(2016, 8, 10), QDate(2028, 8, 10)), debits124);


        std::vector<Debit> debit4 { Debit(dosimetry4.GetDebit()) };

        QCOMPARE(repo.GetDebits(dossier, QDate(2016, 8, 11), QDate(2026, 8, 10)), debit4);
        QCOMPARE(repo.GetDebits(dossier, QDate(2016, 8, 12), QDate(2026, 8, 10)), debit4);
        QVERIFY(repo.GetDebits(dossier, QDate(2016, 8, 13), QDate(2026, 8, 10)).empty());
    }
    catch (...) {
        QFAIL("TestPTRepo::GetDebitsSinceDate An exception was thrown in the test");
    }
}

void TestPTRepo::CreateAndRetrieveSingleEmptySeance() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10000);
        const double dose_prescribed(3.33);
        repo.CreateSeance(dossier, dose_prescribed);
        std::list<SeanceRecord> records;
        Seance seance(1, dose_prescribed, records);
        std::vector<Seance> seances = { seance };

        auto retr_seances = repo.GetSeances(dossier);

        QCOMPARE(seances, retr_seances);
    }
    catch (...) {
        QFAIL("An exception was thrown in the test CreateAndRetrieveSingleEmptySeance");
    }
}

void TestPTRepo::CreateAndRetrieveSingleNonEmptySeance() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10000);
        const double dose_prescribed(3.33);
        repo.CreateSeance(dossier, dose_prescribed);
        Seance seance = repo.GetSeances(dossier).back();
        SeanceRecord r1(1.1, 11);
        SeanceRecord r2(2.2, 22);
        repo.SaveSeanceRecord(seance.GetSeanceId(), r1);
        repo.SaveSeanceRecord(seance.GetSeanceId(), r2);

        std::list<SeanceRecord> records = { r1, r2 };
        std::vector<Seance> seances = { Seance(1, dose_prescribed, records) };

        auto retr_seances = repo.GetSeances(dossier);

        QCOMPARE(seances, retr_seances);
    }
    catch (...) {
        QFAIL("An exception was thrown in the test CreateAndRetrieveSingleNonEmptySeance");
    }
}

void TestPTRepo::CreateAndRetrieveSeanceCheckPrecision() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();


        const int dossier(10000);
        repo.CreateSeance(dossier,   0.001);
        repo.SaveSeanceRecord(1, SeanceRecord(0.001, 100000));
        repo.CreateSeance(dossier,   0.002);
        repo.SaveSeanceRecord(2, SeanceRecord(0.002, 100000));
        repo.CreateSeance(dossier,   0.033);
        repo.SaveSeanceRecord(3, SeanceRecord(0.033, 10000));
        repo.CreateSeance(dossier,   0.444);
        repo.SaveSeanceRecord(4, SeanceRecord(0.444, 1000));
        repo.CreateSeance(dossier,   5.555);
        repo.SaveSeanceRecord(5, SeanceRecord(5.555, 100));
        repo.CreateSeance(dossier,  66.666);
        repo.SaveSeanceRecord(6, SeanceRecord(66.666, 10));
        repo.CreateSeance(dossier, 777.777);
        repo.SaveSeanceRecord(7, SeanceRecord(777.777, 1));
        repo.CreateSeance(dossier,8888.888);
        repo.SaveSeanceRecord(8, SeanceRecord(8888.888, 1));

        auto seances = repo.GetSeances(dossier);

        QVERIFY(calc::AlmostEqual(seances.at(0).GetDosePrescribed(), 0.001, 0.0000001));
        QVERIFY(calc::AlmostEqual(seances.at(0).GetDoseDelivered(), calc::UMToDose(0.001, 100000), 0.0001));

        QVERIFY(calc::AlmostEqual(seances.at(1).GetDosePrescribed(), 0.002, 0.0000001));
        QVERIFY(calc::AlmostEqual(seances.at(1).GetDoseDelivered(), calc::UMToDose(0.002, 100000), 0.0001));

        QVERIFY(calc::AlmostEqual(seances.at(2).GetDosePrescribed(), 0.033, 0.0000001));
        QVERIFY(calc::AlmostEqual(seances.at(2).GetDoseDelivered(), calc::UMToDose(0.033, 10000), 0.0001));

        QVERIFY(calc::AlmostEqual(seances.at(3).GetDosePrescribed(), 0.444, 0.0000001));
        QVERIFY(calc::AlmostEqual(seances.at(3).GetDoseDelivered(), calc::UMToDose(0.444, 1000), 0.0001));

        QVERIFY(calc::AlmostEqual(seances.at(4).GetDosePrescribed(), 5.555, 0.0000001));
        QVERIFY(calc::AlmostEqual(seances.at(4).GetDoseDelivered(), calc::UMToDose(5.555, 100), 0.0001));

        QVERIFY(calc::AlmostEqual(seances.at(5).GetDosePrescribed(), 66.666, 0.0000001));
        QVERIFY(calc::AlmostEqual(seances.at(5).GetDoseDelivered(), calc::UMToDose(66.666, 10), 0.0001));

        QVERIFY(calc::AlmostEqual(seances.at(6).GetDosePrescribed(), 777.777, 0.0000001));
        QVERIFY(calc::AlmostEqual(seances.at(6).GetDoseDelivered(), calc::UMToDose(777.777, 1), 0.0001));

        QVERIFY(calc::AlmostEqual(seances.at(7).GetDosePrescribed(), 8888.888, 0.0000001));
        QVERIFY(calc::AlmostEqual(seances.at(7).GetDoseDelivered(), calc::UMToDose(8888.888, 1), 0.0001));

    }
    catch (...) {
        QFAIL("An exception was thrown in the test CreateAndRetrieveSeanceCheckPrecision");
    }
}

void TestPTRepo::CreateAndRetrieveSingleNonEmptySeanceMultipleValuesInRecords() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10000);
        const double dose_prescribed(3.33);
        repo.CreateSeance(dossier, dose_prescribed);
        Seance seance = repo.GetSeances(dossier).back();

        SeanceRecord r1 = SeanceRecord::GetRandom();
        SeanceRecord r2 = SeanceRecord::GetRandom();

        repo.SaveSeanceRecord(seance.GetSeanceId(), r1);
        repo.SaveSeanceRecord(seance.GetSeanceId(), r2);

        std::list<SeanceRecord> records = { r1, r2 };
        std::vector<Seance> seances = { Seance(1, dose_prescribed, records) };

        auto retr_seances = repo.GetSeances(dossier);

        QCOMPARE(seances, retr_seances);
    }
    catch (...) {
        QFAIL("An exception was thrown in the test CreateAndRetrieveSingleNonEmptySeance");
    }
}

void TestPTRepo::CreateAndRetrieveMultipleNonEmptySeances() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10000);
        const double dose_prescribed_1(3.33);
        const double dose_prescribed_2(2.22);
        repo.CreateSeance(dossier, dose_prescribed_1);
        repo.CreateSeance(dossier, dose_prescribed_2);
        auto seances = repo.GetSeances(dossier);

        SeanceRecord r1 = SeanceRecord::GetRandom();
        SeanceRecord r2 = SeanceRecord::GetRandom();
        SeanceRecord r3 = SeanceRecord::GetRandom();
        SeanceRecord r4 = SeanceRecord::GetRandom();
        SeanceRecord r5 = SeanceRecord::GetRandom();

        repo.SaveSeanceRecord(seances.front().GetSeanceId(), r1);
        repo.SaveSeanceRecord(seances.front().GetSeanceId(), r2);
        repo.SaveSeanceRecord(seances.back().GetSeanceId(), r3);
        repo.SaveSeanceRecord(seances.back().GetSeanceId(), r4);
        repo.SaveSeanceRecord(seances.back().GetSeanceId(), r5);

        std::list<SeanceRecord> records_1 = { r1, r2 };
        std::list<SeanceRecord> records_2 = { r3, r4, r5 };
        std::vector<Seance> exp_seances = { Seance(1, dose_prescribed_1, records_1),
                                            Seance(2, dose_prescribed_2, records_2) };

        auto retr_seances = repo.GetSeances(dossier);

        QCOMPARE(exp_seances, retr_seances);
    }
    catch (...) {
        QFAIL("An exception was thrown in the test CreateAndRetrieveMultipleNonEmptySeances");
    }
}

void TestPTRepo::SeancesForCorrectDossierIsReturned() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        // first dossier
        const int dossier(10000);
        const double dose_prescribed_1(3.33);
        const double dose_prescribed_2(2.22);
        repo.CreateSeance(dossier, dose_prescribed_1);
        repo.CreateSeance(dossier, dose_prescribed_2);
        auto seances = repo.GetSeances(dossier);
        SeanceRecord r1 = SeanceRecord::GetRandom();
        SeanceRecord r2 = SeanceRecord::GetRandom();
        SeanceRecord r3 = SeanceRecord::GetRandom();
        SeanceRecord r4 = SeanceRecord::GetRandom();

        repo.SaveSeanceRecord(seances.front().GetSeanceId(), r1);
        repo.SaveSeanceRecord(seances.front().GetSeanceId(), r2);
        repo.SaveSeanceRecord(seances.front().GetSeanceId(), r3);
        repo.SaveSeanceRecord(seances.back().GetSeanceId(), r4);

        // second dossier
        const int dossier2(10001);
        repo.CreateTreatment(dossier2, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1")),
        repo.CreateSeance(dossier2, dose_prescribed_1 + 10.0);
        repo.CreateSeance(dossier2, dose_prescribed_2 + 10.0);
        auto seances2 = repo.GetSeances(dossier2);
        SeanceRecord r5 = SeanceRecord::GetRandom();
        SeanceRecord r6 = SeanceRecord::GetRandom();
        SeanceRecord r7 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(seances2.front().GetSeanceId(), r5);
        repo.SaveSeanceRecord(seances2.front().GetSeanceId(), r6);
        repo.SaveSeanceRecord(seances2.back().GetSeanceId(), r7);

        // expected result
        std::list<SeanceRecord> records_1 = { r1, r2, r3 };
        std::list<SeanceRecord> records_2 = { r4 };
        std::vector<Seance> exp_seances = { Seance(1, dose_prescribed_1, records_1),
                                            Seance(2, dose_prescribed_2, records_2) };

        auto retr_seances = repo.GetSeances(dossier);

        QCOMPARE(exp_seances, retr_seances);
    }
    catch (...) {
        QFAIL("An exception was thrown in the test CreateAndRetrieveMultipleNonEmptySeances");
    }
}

void TestPTRepo::GetActiveSeanceWhenTreatmentNotStarted() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 111.0, 222.0, 333.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));
        Seance active_seance = repo.GetActiveSeance(dossier);
        QVERIFY(calc::AlmostEqual(active_seance.GetDosePrescribed(), 111.0, 0.0001));
    }
    catch (std::exception& exc) {
        QString msg = "TestPTRepo::GetActiveSeanceWhenTreatmentNotStarted Exc thrown " + QString(exc.what());
        QFAIL(msg.toStdString().c_str());
    }
}

void TestPTRepo::GetActiveSeanceWhenTreatmentStarted() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 111.0, 222.0, 333.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));
        Treatment t = repo.GetTreatment(dossier);
        t.IncrementSeance();
        repo.UpdateTreatment(t);
        Seance active_seance = repo.GetActiveSeance(dossier);
        QVERIFY(calc::AlmostEqual(active_seance.GetDosePrescribed(), 222.0, 0.0001));
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::GetActiveSeanceWhenTreatmentNotStarted Exc thrown");
    }
}

void TestPTRepo::GetActiveSeanceWhenTreatmentFinished() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 111.0, 222.0, 333.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));
        Treatment t = repo.GetTreatment(dossier);
        t.IncrementSeance();
        t.IncrementSeance();
        t.IncrementSeance();
        repo.UpdateTreatment(t);
        try {
            repo.GetActiveSeance(dossier);
            QFAIL("TestPTRepo::GetActiveSeanceWhenTreatmentFinished Should have thrown");
        }
        catch (std::exception& exc) {}
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::GetActiveSeanceWhenTreatmentFinished Exc thrown");
    }
}

void TestPTRepo::GetActiveSeanceErrorCases() {
    // dossier does not exist
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        repo.GetActiveSeance(666);
        QFAIL("TestPTRepo::GetActiveSeanceErrorCases Should have thrown (no dossier)");
    }
    catch (std::exception& exc) {}
}

void TestPTRepo::GetSeanceRecord() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));

        try {
            repo.GetSeanceRecord(QDateTime::currentDateTime());
            QFAIL("TestPTRepo::GetSeanceRecord An exception should have been thrown, but wasnt");
        }
        catch (...) {}

        SeanceRecord r1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, r1);

        try {
            repo.GetSeanceRecord(r1.GetTimestamp().addSecs(1));
            QFAIL("TestPTRepo::GetSeanceRecord An exception should have been thrown, but wasnt");
        }
        catch(...) {}

        QCOMPARE(repo.GetSeanceRecord(r1.GetTimestamp()), r1);

        SeanceRecord r2 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, r2);
        QCOMPARE(repo.GetSeanceRecord(r2.GetTimestamp()), r2);
    }
    catch (...) {
        QFAIL("TestPTRepo::GetSeanceRecord An exception was thrown in the test");
    }
}

void TestPTRepo::GetSeanceRecordById() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));

        try {
            repo.GetSeanceRecord(132);
            QFAIL("TestPTRepo::GetSeanceRecord An exception should have been thrown, but wasnt");
        }
        catch (...) {}

        SeanceRecord r1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, r1);

        const int id1 = repo.GetSeanceRecordId(r1);
        QCOMPARE(repo.GetSeanceRecord(id1), r1);

        SeanceRecord r2 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, r2);

        const int id2 = repo.GetSeanceRecordId(r2);
        QCOMPARE(repo.GetSeanceRecord(id2), r2);
    }
    catch (...) {
        QFAIL("TestPTRepo::GetSeanceRecordById An exception was thrown in the test");
    }
}

void TestPTRepo::GetLastSeanceRecord_OneRecord() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));


        SeanceRecord r1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, r1);

        SeanceRecord last_record = repo.GetLastSeanceRecord(dossier);
        QCOMPARE(r1, last_record);
    }
    catch (...) {
        QFAIL("TestPTRepo::GetLastSeanceRecord_OneRecord An exception was thrown in the test");
    }
}

void TestPTRepo::GetLastSeanceRecord_MultipleRecords() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));

        SeanceRecord record_s2_r1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(2, record_s2_r1);

        SeanceRecord last_record_1 = repo.GetLastSeanceRecord(dossier);
        QCOMPARE(record_s2_r1, last_record_1);

        SeanceRecord record_s1_r1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, record_s1_r1);

        SeanceRecord last_record_2 = repo.GetLastSeanceRecord(dossier);
        QCOMPARE(record_s1_r1, last_record_2);

        SeanceRecord record_s2_r2 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(2, record_s2_r2);

        SeanceRecord last_record_3 = repo.GetLastSeanceRecord(dossier);
        QCOMPARE(record_s2_r2, last_record_3);

        QVERIFY(record_s1_r1 != record_s2_r1);
        QVERIFY(record_s2_r1 != record_s2_r2);
    }
    catch (...) {
        QFAIL("TestPTRepo::GetLastSeanceRecord_OneRecord An exception was thrown in the test");
    }
}

void TestPTRepo::GetLastSeanceRecord_ErrorCases() {
    PTRepo repo("unittest", dbHostName_, dbPort_,
                      dbDatabaseName_, dbUserName_,
                      dbPassword_);
    repo.Connect();

    try {
        repo.GetLastSeanceRecord(666);
        QFAIL("GetLastSeanceRecord_ErrorCases should have thrown 1");
    }
    catch(std::exception& exc) {}

    const int dossier(10001);
    std::vector<double> dose_pre { 1.0, 2.0 };
    repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));
    try {
        repo.GetLastSeanceRecord(dossier);
        QFAIL("GetLastSeanceRecord_ErrorCases should have thrown 2");
    }
    catch(std::exception& exc) {}
}

void TestPTRepo::GetSeanceRecordId() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));

        SeanceRecord record_1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, record_1);
        SeanceRecord record_2 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, record_2);
        SeanceRecord record_3 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(2, record_3);

        QCOMPARE(repo.GetSeanceRecordId(record_1), 1);
        QCOMPARE(repo.GetSeanceRecordId(record_2), 2);
        QCOMPARE(repo.GetSeanceRecordId(record_3), 3);


        SeanceRecord record_4 = SeanceRecord::GetRandom();
        // Deliberatly not saved to db
        try {
            repo.GetSeanceRecordId(record_4);
            QFAIL("GetSeanceRecordId should have thrown 1");
        }
        catch (std::exception& exc) {}

        repo.SaveSeanceRecord(2, record_3); // save same record again -> 2 records having the same timestamp
        try {
            repo.GetSeanceRecordId(record_3);
            QFAIL("GetSeanceRecordId should have thrown 2");
        }
        catch (std::exception& exc) {}
    }
    catch (std::exception& exc) {
        QFAIL("GetSeanceRecordId Exception thrown in test");
    }
}

void TestPTRepo::DeleteSeance() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier1(10001);
        repo.CreateTreatment(dossier1, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));
        const int dossier2(10002);
        repo.CreateTreatment(dossier2, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));
        repo.CreateSeance(dossier1, 1.0);
        repo.CreateSeance(dossier2, 11.0);
        repo.CreateSeance(dossier1, 2.0);
        repo.CreateSeance(dossier2, 12.0);
        repo.CreateSeance(dossier1, 3.0);
        repo.CreateSeance(dossier2, 13.0);
        repo.CreateSeance(dossier1, 4.0);

        QCOMPARE((int)repo.GetSeances(dossier1).size(), 4);
        QCOMPARE((int)repo.GetSeances(dossier2).size(), 3);

        repo.DeleteSeance(dossier1, 3); // id = 3 -> 2.0 GY seance
        std::vector<Seance> exp1_dossier1 {Seance(1, 1.0, std::list<SeanceRecord> ()),
                                           Seance(5, 3.0, std::list<SeanceRecord> ()),
                                           Seance(7, 4.0, std::list<SeanceRecord> ()) };
        std::vector<Seance> exp1_dossier2 {Seance(2, 11.0, std::list<SeanceRecord> ()),
                                           Seance(4, 12.0, std::list<SeanceRecord> ()),
                                           Seance(6, 13.0, std::list<SeanceRecord> ()) };
        QCOMPARE(exp1_dossier1, repo.GetSeances(dossier1));
        QCOMPARE(exp1_dossier2, repo.GetSeances(dossier2));

        repo.DeleteSeance(dossier2, 4); // id = 4 -> 12.0 GY seance
        std::vector<Seance> exp2_dossier2 {Seance(2, 11.0, std::list<SeanceRecord> ()),
                                           Seance(6, 13.0, std::list<SeanceRecord> ()) };
        QCOMPARE(exp1_dossier1, repo.GetSeances(dossier1));
        QCOMPARE(exp2_dossier2, repo.GetSeances(dossier2));

        try {
            repo.DeleteSeance(dossier1, 3); // doesnt exist any longer
            QFAIL("DeleteSeance should have thrown1");
        }
        catch (...) {}

        try {
            repo.DeleteSeance(dossier2, 1); // mis match dossier and seance id
            QFAIL("DeleteSeance should have thrown2");
        }
        catch (...) {}

        repo.CreateSeance(dossier1, 10.0);
        std::vector<Seance> exp2_dossier1 {Seance(1, 1.0, std::list<SeanceRecord> ()),
                                           Seance(5, 3.0, std::list<SeanceRecord> ()),
                                           Seance(7, 4.0, std::list<SeanceRecord> ()),
                                           Seance(8, 10.0, std::list<SeanceRecord> ()) };
        QCOMPARE(exp2_dossier1, repo.GetSeances(dossier1));
    }
    catch(...) {
        QFAIL("TestPTRepo::DeleteSeance An exception was thrown in the test");
    }
}

void TestPTRepo::UpdateDeliveredDose() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));
        SeanceRecord record1 = SeanceRecord::GetRandom();
        const int um_del = record1.GetTotalUMDelivered();
        const int um_del_updated = um_del + 66;
        repo.SaveSeanceRecord(1, record1);

        repo.UpdateDeliveredDose(record1.GetTimestamp(), um_del_updated);
        QCOMPARE(repo.GetSeanceRecord(record1.GetTimestamp()).GetTotalUMDelivered(), um_del_updated);

        try {
            repo.UpdateDeliveredDose(record1.GetTimestamp().addSecs(1), 376);
            QFAIL("TestPTRepo::UpdateDeliveredDose An exception was not thrown in the test, but should have");
        }
        catch (...) {}

        try {
            repo.UpdateDeliveredDose(record1.GetTimestamp(), -11);
            QFAIL("TestPTRepo::UpdateDeliveredDose An exception was not thrown in the test, but should have");
        }
        catch (...) {}

        repo.SaveSeanceRecord(1, record1);
        try {
            repo.UpdateDeliveredDose(record1.GetTimestamp(), 11);
            QFAIL("TestPTRepo::UpdateDeliveredDose An exception was not thrown in the test, but should have");
        }
        catch (...) {}
    }
    catch(...) {
        QFAIL("TestPTRepo:UpdateDeliveredDose An exception was thrown in the test");
    }
}

void TestPTRepo::UpdatePlannedDose() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        std::vector<double> dose_pre_1 { 1.0, 3.0, 2.567 };
        repo.CreateTreatment(10001, 10000, dose_pre_1, 10000, 10000, 10000, QString("t1"));
        std::vector<double> dose_pre_2 { 10.0, 30.0, 25.67 };
        repo.CreateTreatment(10002, 10000, dose_pre_2, 10000, 10000, 10000, QString("t1"));

        std::vector<Seance> seances_10001_before = repo.GetSeances(10001);
        std::vector<Seance> exp_seances_10001_before { Seance(1, 1.0, std::list<SeanceRecord>()),
                                                       Seance(2, 3.0, std::list<SeanceRecord>()),
                                                       Seance(3, 2.567, std::list<SeanceRecord>()) };
        QCOMPARE(seances_10001_before, exp_seances_10001_before);

        std::vector<Seance> seances_10002_before = repo.GetSeances(10002);
        std::vector<Seance> exp_seances_10002_before { Seance(4, 10.0, std::list<SeanceRecord>()),
                                                       Seance(5, 30.0, std::list<SeanceRecord>()),
                                                       Seance(6, 25.67, std::list<SeanceRecord>()) };

        // modify planned dose 1
        repo.UpdatePlannedDose(2, 10001, 33.0);
        std::vector<Seance> seances_10001_after = repo.GetSeances(10001);
        std::vector<Seance> exp_seances_10001_after { Seance(1, 1.0, std::list<SeanceRecord>()),
                                                      Seance(2, 33.0, std::list<SeanceRecord>()),
                                                      Seance(3, 2.567, std::list<SeanceRecord>()) };
        QCOMPARE(seances_10001_after, exp_seances_10001_after);
        QCOMPARE(seances_10002_before, exp_seances_10002_before);

        // modify planned dose 2
        repo.UpdatePlannedDose(4, 10002, 44.0);
        repo.UpdatePlannedDose(5, 10002, 55.0);
        std::vector<Seance> seances_10002_after = repo.GetSeances(10002);
        std::vector<Seance> exp_seances_10002_after { Seance(4, 44.0, std::list<SeanceRecord>()),
                                                      Seance(5, 55.0, std::list<SeanceRecord>()),
                                                      Seance(6, 25.67, std::list<SeanceRecord>()) };
        QCOMPARE(seances_10002_after, exp_seances_10002_after);
        QCOMPARE(seances_10001_after, exp_seances_10001_after);

        try {
            // dossier and seanceid does not correspond
            repo.UpdatePlannedDose(4, 10001, 66.0);
            QFAIL("TestPTRepo::UpdatePlannedDose An exception was not thrown in the test1");
        }
        catch (...) {}

        try {
            // negative dose
            repo.UpdatePlannedDose(4, 10002, -66.0);
            QFAIL("TestPTRepo::UpdatePlannedDose An exception was not thrown in the test2");
        }
        catch (...) {}

        try {
            // dossier does not exist
            repo.UpdatePlannedDose(4, 10003, 66.0);
            QFAIL("TestPTRepo::UpdatePlannedDose An exception was not thrown in the test3");
        }
        catch (...) {}

        try {
            // seanceid does not exist
            repo.UpdatePlannedDose(7, 10002, 66.0);
            QFAIL("TestPTRepo::UpdatePlannedDose An exception was not thrown in the test4");
        }
        catch (...) {}


    }
    catch(...) {
        QFAIL("TestPTRepo:UpdatePlannedDose An exception was thrown in the test");
    }
}

void TestPTRepo::UpdateEstimatedDose() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));
        SeanceRecord record1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, record1);
        const double est_dose = record1.GetDoseEstimated() + 1.0;

        repo.UpdateEstimatedDose(repo.GetSeanceRecordId(record1), est_dose);
        SeanceRecord retr1 = repo.GetSeanceRecord(record1.GetTimestamp());
        QCOMPARE(retr1.GetDoseEstimated(), est_dose);
        retr1.SetEstimatedDose(record1.GetDoseEstimated());
        QCOMPARE(retr1, record1);

        try {
            repo.UpdateEstimatedDose(repo.GetSeanceRecordId(record1), -10.0);
            QFAIL("TestPTRepo::UpdateEstimatedDose 1 An exception was not thrown in the test, but should have");
        }
        catch (...) {}

        try {
            repo.UpdateEstimatedDose(repo.GetSeanceRecordId(record1) + 100, 10.0);
            QFAIL("TestPTRepo::UpdateEstimatedDose 2 An exception was not thrown in the test, but should have");
        }
        catch (...) {}
    }
    catch(...) {
        QFAIL("TestPTRepo:UpdateEstimatedDose An exception was thrown in the test");
    }
}

void TestPTRepo::CreateTreatmentSuccessCases() {
    if (!InsertDegradeur(1, 800, true)) {
        QFAIL("TestPTRepo::CreateTreatmentSuccessCases Could not insert degradeur");
    }

    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int patient_id(10000);
        const std::vector<double> doses = {1.11, 2.22, 3.33};
        std::list<SeanceRecord> records;
        std::vector<Seance> seances = { Seance(1, doses[0], records),
                                        Seance(2, doses[1], records),
                                        Seance(3, doses[2], records) };

        // Two degradeurs
        const int dossier1(10001);
        repo.CreateTreatment(dossier1, patient_id, doses, 10000, 10000, 1, QString("t1"));

        Treatment tr1 = repo.GetTreatment(dossier1);
        QCOMPARE(tr1.GetDossier(), dossier1);
        QCOMPARE(tr1.GetPatient().GetId(), patient_id);
        QCOMPARE(tr1.TreatmentType(), QString("t1"));
        QCOMPARE(tr1.GetAllSeances(), seances);
        QCOMPARE(tr1.GetModulateur().id(), QString("10000"));
        QStringList ids1;
        ids1 << "10000" << "1";
        QCOMPARE(tr1.GetDegradeurSet().ids(), ids1);
        QVERIFY(calc::AlmostEqual(tr1.dref(), 1.37, 0.0001));

        // One degradeur
        const int dossier2(10002);
        repo.CreateTreatment(dossier2, patient_id, doses, 10000, 1, QString("t1"));
        std::vector<Seance> seances2 = { Seance(4, doses[0], records),
                                        Seance(5, doses[1], records),
                                        Seance(6, doses[2], records) };

        Treatment tr2 = repo.GetTreatment(dossier2);
        QCOMPARE(tr2.GetDossier(), dossier2);
        QCOMPARE(tr2.GetPatient().GetId(), patient_id);
        QCOMPARE(tr2.GetAllSeances(), seances2);
        QCOMPARE(tr2.GetModulateur().id(), QString("10000"));
        QStringList ids2;
        ids2 << "1";
        QCOMPARE(tr2.GetDegradeurSet().ids(), ids2);
        QVERIFY(calc::AlmostEqual(tr2.dref(), 1.37, 0.0001));

    }
    catch (...) {
        QFAIL("An exception was thrown in the test CreateTreatmentSuccessCase");
    }
}

void TestPTRepo::CreateTreatmentNonValidDegradeur() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        try {
            repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, -1, 10000, QString("t1"));
            QFAIL("TestDbRepostory::CreateTreatmentNonValidDegradeur should have thrown exception");
        }
        catch (...) {}
    }
    catch (...) {
        QFAIL("An exception was thrown in the test CreateTreatmentSuccessCase");
    }
}

void TestPTRepo::CreateTreatmentRollbackWhenFails() {
    // not allowed to create treatment when patient doesnt exist in db
    PTRepo repo("unittest", dbHostName_, dbPort_,
                      dbDatabaseName_, dbUserName_,
                      dbPassword_);
    repo.Connect();

    const int patient_id(999); // non existant
    const int dossier(10001);
    const std::vector<double> doses = {1.11, 2.22, 3.33};

    try {
        repo.CreateTreatment(dossier, patient_id, doses, 10000, 10000, 10000, QString("t1"));
        QFAIL("An exception was NOT thrown in the test CreateTreatmentRollbackWhenFails");
    }
    catch (...) {
        try {
        QVERIFY(!repo.DossierExist(dossier));
        QVERIFY(repo.GetSeances(dossier).empty());
        }
        catch(...) {
            QFAIL("TestPTRepo::CreateTreatmentRollbackWhenFails Exception thrown");
        }
    }
}

void TestPTRepo::CreateTreatmentThatAlreadyExistsFails() {
    const int dossier(10001);
    std::vector<double> doses = {1.11, 2.22, 3.33};
    std::list<SeanceRecord> records;
    std::vector<Seance> seances = { Seance(1, doses[0], records),
                                    Seance(2, doses[1], records),
                                    Seance(3, doses[2], records) };
    PTRepo repo("unittest", dbHostName_, dbPort_,
                      dbDatabaseName_, dbUserName_,
                      dbPassword_);
    try {
        repo.Connect();
        const int patient_id(10000);
        repo.CreateTreatment(dossier, patient_id, doses, 10000, 10000, 10000, QString("t1"));
        doses.push_back(4.44);
        repo.CreateTreatment(dossier, patient_id, doses,  10000, 10000, 10000, QString("t1")); // this should throw en exception
        QFAIL("An exception was NOT thrown in CreateTreatmentThatAlreadyExistsFails");
    }
    catch (...) {
        Treatment tr = repo.GetTreatment(dossier);
        QCOMPARE(tr.GetDossier(), dossier);
        QCOMPARE(tr.GetAllSeances(), seances);
    }
}

void TestPTRepo::CreateAndRetrieveTreatmentAllDosimetriesCorrectlyDone() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

         const int dossier(10001);
         const int patient_id(10000);
         const std::vector<double> dose_prescribed = { 2.22 };
         const bool reference(true);

         repo.CreateTreatment(dossier, patient_id, dose_prescribed, 10000, 10000, 10000, QString("t1"));
         std::list<DosimetryRecord> records;
         auto chambre = repo.GetChambre("FWT-849");
         Dosimetry patientDosimetry(QDateTime(QDate(2015, 2, 3), QTime(2, 2, 2)), !reference, false, 23.0, 1001, chambre, records, "");
         Dosimetry patientRefDosimetry(QDateTime(QDate(2015, 2, 3), QTime(1, 1, 1)), reference, false, 24.0, 1001, chambre, records, "");
         Dosimetry refDosimetryToday(QDateTime(QDate::currentDate()), reference, false, 25.0, 1001, chambre, records, "");
         repo.SaveDosimetry(dossier, patientDosimetry);
         repo.SaveDosimetry(10000, patientRefDosimetry);
         repo.SaveDosimetry(10000, refDosimetryToday);

         auto treatment = repo.GetTreatment(dossier);

         QCOMPARE(treatment.GetDossier(), dossier);
         QCOMPARE(treatment.GetActiveSeanceNumber(), (int)0);
         QCOMPARE((int)treatment.GetAllSeances().size(), 1);
         QCOMPARE(treatment.GetActiveSeance()->GetDosePrescribed(), dose_prescribed.at(0));
         QCOMPARE(treatment.GetPatientDosimetry(), patientDosimetry);
         QCOMPARE(treatment.GetPatientRefDosimetry(), patientRefDosimetry);
         QCOMPARE(treatment.GetRefDosimetryToday(), refDosimetryToday);
    }
    catch (...) {
        QFAIL("An exception was thrown in test CreateAndRetrieveTreatment");
    }
}

void TestPTRepo::CreateAndRetrieveTreatmentCorrectReferenceTaken() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

         const int dossier(10001);
         const int patient_id(10000);
         const std::vector<double> dose_prescribed = { 2.22 };
         const bool reference(true);

         repo.CreateTreatment(dossier, patient_id, dose_prescribed, 10000, 10000, 10000, QString("t1"));
         std::list<DosimetryRecord> records;
         auto chambre = repo.GetChambre("FWT-849");
         Dosimetry tdj1(QDateTime(QDate(2015, 2, 3), QTime(1, 1, 1)), reference, false, 23.0, 1001, chambre, records, "");
         Dosimetry tdj2(QDateTime(QDate(2015, 2, 3), QTime(3, 3, 3)), reference, false, 24.0, 1001, chambre, records, "");
         Dosimetry patientDosimetry(QDateTime(QDate(2015, 2, 3), QTime(2, 2, 2)), !reference, false, 23.0, 1001, chambre, records, "");
         Dosimetry tdjToday(QDateTime(QDate::currentDate()), reference, false, 25.0, 1001, chambre, records, "");
         repo.SaveDosimetry(dossier, patientDosimetry);
         repo.SaveDosimetry(10000, tdj1);
         repo.SaveDosimetry(10000, tdj2);
         repo.SaveDosimetry(10000, tdjToday);

         auto treatment = repo.GetTreatment(dossier);

         QCOMPARE(treatment.GetDossier(), dossier);
         QCOMPARE(treatment.GetActiveSeanceNumber(), (int)0);
         QCOMPARE((int)treatment.GetAllSeances().size(), 1);
         QCOMPARE(treatment.GetActiveSeance()->GetDosePrescribed(), dose_prescribed.at(0));
         QCOMPARE(treatment.GetPatientDosimetry(), patientDosimetry);
         QCOMPARE(treatment.GetPatientRefDosimetry(), tdj1);
         QCOMPARE(treatment.GetRefDosimetryToday(), tdjToday);
    }
    catch (...) {
        QFAIL("An exception was thrown in test CreateAndRetrieveTreatmentCorrectReferenceTaken");
    }
}

void TestPTRepo::CreateAndRetrieveTreatmentSomeDosimetriesDone() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

         const int dossier(10001);
         const int patient_id(10000);
         const std::vector<double> dose_prescribed = { 2.22 };

         repo.CreateTreatment(dossier, patient_id, dose_prescribed, 10000, 10000, 10000, QString("t1"));
         std::list<DosimetryRecord> records;
         auto chambre = repo.GetChambre("FWT-849");
         Dosimetry refDosimetryToday(QDateTime(QDate::currentDate()), true, false, 25.0, 1001, chambre, records, "");
         repo.SaveDosimetry(10000, refDosimetryToday);

         auto treatment = repo.GetTreatment(dossier);

         QCOMPARE(treatment.GetDossier(), dossier);
         QCOMPARE(treatment.GetActiveSeanceNumber(), (int)0);
         QCOMPARE((int)treatment.GetAllSeances().size(), 1);
         QCOMPARE(treatment.GetActiveSeance()->GetDosePrescribed(), dose_prescribed.at(0));
         QCOMPARE(treatment.GetPatientDosimetry().GetTimestamp().isValid(), false);
         QCOMPARE(treatment.GetPatientRefDosimetry().GetTimestamp().isValid(), false);
         QCOMPARE(treatment.GetRefDosimetryToday(), refDosimetryToday);
    }
    catch (...) {
        QFAIL("An exception was thrown in test CreateAndRetrieveTreatment");
    }
}

void TestPTRepo::UpdateTreatment() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        const int patient_id(10000);
        const std::vector<double> dose_prescribed = { 2.22 };
        repo.CreateTreatment(dossier, patient_id, dose_prescribed, 10000, 10000, 10000, QString("t1"));
        auto treatment = repo.GetTreatment(dossier);
        auto active_seance = treatment.GetActiveSeanceNumber();
        treatment.IncrementSeance();
        repo.UpdateTreatment(treatment);
        auto treatment_updated = repo.GetTreatment(dossier);
        QVERIFY(treatment_updated.GetActiveSeanceNumber() != active_seance);
    }
    catch (...) {
        QFAIL("An exception was thrown in test UpdateTreatment");
    }
}

void TestPTRepo::UpdateTreatmentType() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto types = repo.GetTreatmentTypes();
        if (types.size() < 2) {
            QFAIL("This test requires atleast 2 treatment types in the db");
        }
        QVERIFY(types.at(0).Name() != types.at(1).Name());

        const int dossier(10001);
        const int patient_id(10000);
        const std::vector<double> dose_prescribed = { 2.22 };
        repo.CreateTreatment(dossier, patient_id, dose_prescribed, 10000, 10000, 10000, types.at(0).Name());
        repo.UpdateTreatmentType(dossier, types.at(1).Name());

        Treatment t = repo.GetTreatment(dossier);
        QCOMPARE(t.TreatmentType(), types.at(1).Name());
    }
    catch (...) {
        QFAIL("An exception was thrown in test UpdateTreatment");
    }
}

void TestPTRepo::GetTreatmentType() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int patient_id(10000);
        const std::vector<double> dose_prescribed = { 2.22 };

        repo.CreateTreatment(10001, patient_id, dose_prescribed, 10000, 10000, 10000, QString("t1"));
        repo.CreateTreatment(10002, patient_id, dose_prescribed, 10000, 10000, 10000, QString("t2"));

        QCOMPARE(repo.GetTreatmentType(10001), QString("t1"));
        QCOMPARE(repo.GetTreatmentType(10002), QString("t2"));
    }
    catch (...) {
        QFAIL("An exception was thrown in test GetTreatmentType");
    }
}

void TestPTRepo::ChangeDegradeurSet() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int patient_id(10000);
        const std::vector<double> dose_prescribed = { 2.22 };

        const int dossier1(10001);
        repo.CreateTreatment(dossier1, patient_id, dose_prescribed, 10000, 20000, 30000, QString("t1"));
        DegradeurSet set_before1(repo.GetDegradeurSetForDossier(dossier1));
        QCOMPARE(set_before1.ids().at(0).toInt(), 20000);
        QCOMPARE(set_before1.ids().at(1).toInt(), 30000);
        repo.ChangeDegradeurSet(dossier1, 40000, 50000);
        DegradeurSet set_after1(repo.GetDegradeurSetForDossier(dossier1));
        QCOMPARE(set_after1.ids().at(0).toInt(), 40000);
        QCOMPARE(set_after1.ids().at(1).toInt(), 50000);

        const int dossier2(10002);
        repo.CreateTreatment(dossier2, patient_id, dose_prescribed, 10000, 20000, QString("t1"));
        repo.ChangeDegradeurSet(dossier2, 40000, 50000);
        DegradeurSet set2(repo.GetDegradeurSetForDossier(dossier2));
        QCOMPARE(set2.ids().at(0).toInt(), 40000);
        QCOMPARE(set2.ids().at(1).toInt(), 50000);

        const int dossier3(10003);
        repo.CreateTreatment(dossier3, patient_id, dose_prescribed, 10000, 20000, 30000, QString("t1"));
        repo.ChangeDegradeurSet(dossier3, 40000, -1);
        DegradeurSet set3(repo.GetDegradeurSetForDossier(dossier3));
        QCOMPARE(set3.ids().at(0).toInt(), 40000);
        QVERIFY(set3.ids().size() == 1);

        try {
            repo.ChangeDegradeurSet(dossier3, -1, 30000);
            QFAIL("ChangeDegradeurSet Should have thrown1");
        }
        catch (...) {}

        try {
            repo.ChangeDegradeurSet(99999, 40000, 50000);
            QFAIL("ChangeModulateur Should have thrown2");
        }
        catch (...) {}

    }
    catch (...) {
        QFAIL("An exception was thrown in test ChangeDegradeurSet");
    }
}

void TestPTRepo::ChangeModulateur() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int patient_id(10000);
        const std::vector<double> dose_prescribed = { 2.22 };

        const int dossier1(10001);
        repo.CreateTreatment(dossier1, patient_id, dose_prescribed, 10000, 20000, 30000, QString("t1"));
        QCOMPARE(repo.GetModulateurForDossier(dossier1).id().toInt(), 10000);
        repo.ChangeModulateur(dossier1, 10001);
        QCOMPARE(repo.GetModulateurForDossier(dossier1).id().toInt(), 10001);

        try {
            repo.ChangeModulateur(dossier1, -1);
            QFAIL("ChangeModulateur Should have thrown1");
        }
        catch (...) {}

        try {
            repo.ChangeModulateur(99999, 10001);
            QFAIL("ChangeModulateur Should have thrown2");
        }
        catch (...) {}
    }
    catch (...) {
        QFAIL("An exception was thrown in test ChangeModulateur");
    }
}

void TestPTRepo::AddSeanceToExistingTreatment() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        std::vector<double> pre_dose;
        pre_dose.push_back(1.1);
        pre_dose.push_back(2.2);
        pre_dose.push_back(3.3);
        repo.CreateTreatment(10001, 10000, pre_dose, 10000, 10000, 10000, QString("t1"));

        auto t1 = repo.GetTreatment(10001);
        QCOMPARE(t1.GetActiveSeanceNumber(), (int)0);

        repo.AddSeanceToExistingTreatment(10001, 4.4);
        auto t2 = repo.GetTreatment(10001);
        QCOMPARE(t2.GetActiveSeanceNumber(), (int)0);
        QCOMPARE((int)t2.GetAllSeances().size(), 4);

        t2.IncrementSeance();
        t2.IncrementSeance();
        t2.IncrementSeance();
        t2.IncrementSeance();
        repo.UpdateTreatment(t2);
        QCOMPARE(t2.GetActiveSeanceNumber(), (int)-1);

        repo.AddSeanceToExistingTreatment(10001, 5.5);
        auto t3 = repo.GetTreatment(10001);
        QCOMPARE(t3.GetActiveSeanceNumber(), (int)4);
        QCOMPARE(t3.GetActiveSeance()->GetDosePrescribed(), 5.5);

        try {
            repo.AddSeanceToExistingTreatment(10001, -5.5);
            QFAIL("TestPTRepo::AddSeanceToExistingTreatment An exception should have been thrown but wasnt");
        }
        catch (...) {}
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::AddSeanceToExistingTreatment Exception thrown during test");
    }
}

void TestPTRepo::GetPatients() {
    QSqlQuery query(QSqlDatabase::database(dbConnName_));
    if (!query.exec("INSERT INTO Patient VALUES (444, 'Abra', 'Kadabral');")) {
        qWarning() << "Query insert into patient failed: " << query.lastError();
    }
    query.clear();
    if (!query.exec("INSERT INTO Patient VALUES (555, 'Piff', 'Puff');")) {
        qWarning() << "Query insert into patient failed: " << query.lastError();
    }
    query.clear();

    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto patients1 = repo.GetPatients("Ab","");
        QVERIFY(patients1.size() == 1);
        QVERIFY(patients1.at(0).GetId() == 444);

        auto patients2 = repo.GetPatients("","Puff");
        QVERIFY(patients2.size() == 1);
        QVERIFY(patients2.at(0).GetId() == 555);

        auto patients3 = repo.GetPatients("","");
        QVERIFY(patients3.size() == 3);

        auto patients4 = repo.GetPatients("Gri","Gru");
        QVERIFY(patients4.size() == 0);
    }
    catch (...) {
        QFAIL("An exception was thrown in test GetPatients");
    }

}

void TestPTRepo::InsertAndRetrieveDoubles() {
    QSqlQuery query(QSqlDatabase::database(dbConnName_));

    query.exec("DROP TABLE IF EXISTS Doubles;");
    query.clear();

    if (!query.exec("CREATE TABLE Doubles(d1 REAL, d2 REAL, d3 REAL, d4 REAL, d5 REAL, PRIMARY KEY(d1));")) {
        qWarning() << "TestPTRepo::InsertAndRetrieveDoubles create table query failed";
    }
    query.clear();

    query.prepare("INSERT INTO Doubles(d1,d2,d3,d4,d5) VALUES(:d1,:d2,:d3,:d4,:d5)");
    query.bindValue(":d1", (double)19393493.0);
    query.bindValue(":d2", (double)1289282.39392003);
    query.bindValue(":d3", (double)12.39);
    query.bindValue(":d4", (double)12.39922928);
    query.bindValue(":d5", (double)1.1);
    if (!query.exec()) {
        qWarning() << QString("TestPTRepo::InsertAndRetrieveDoubles insert query failed ") << query.lastError();
    }
    query.clear();

    query.prepare("SELECT d1, d2, d3, d4, d5 FROM Doubles");
    if (!query.exec()) {
        qWarning() << "TestPTRepo::InsertAndRetrieveDoubles select query failed";
    }

    query.next();

    const double rel_precision(0.00001);
    QVERIFY(calc::AlmostEqual(query.value(0).toDouble(), 19393493.0, rel_precision));
    QVERIFY(calc::AlmostEqual(query.value(1).toDouble(), 1289282.39392003, rel_precision));
    QVERIFY(calc::AlmostEqual(query.value(2).toDouble(), 12.39, rel_precision));
    QVERIFY(calc::AlmostEqual(query.value(3).toDouble(), 12.39922928, rel_precision));
    QVERIFY(calc::AlmostEqual(query.value(4).toDouble(), 1.1, rel_precision));
}

void TestPTRepo::GetDegradeurSet() {
    if (!InsertDegradeur(1, 800, true)) {
        QFAIL("TestPTRepo::GetDegradeurForDossier Could not insert degradeur");
    }
    if (!InsertDegradeur(2, 1600, false)) {
        QFAIL("TestPTRepo::GetDegradeurForDossier Could not insert degradeur");
    }

    PTRepo repo("unittest", dbHostName_, dbPort_,
                      dbDatabaseName_, dbUserName_,
                      dbPassword_);
    repo.Connect();

    // Single degradeur
    repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 1, QString("t1"));
    QCOMPARE(repo.GetDegradeurSetForDossier(10001), DegradeurSet(Degradeur("1", 800, true)));

    // Two degradeurs
    repo.CreateTreatment(10002, 10000, std::vector<double>(), 10000, 1, 2, QString("t1"));
    QCOMPARE(repo.GetDegradeurSetForDossier(10002), DegradeurSet(Degradeur("1", 800, true),
                                                                 Degradeur("2", 1600, false)));

    // Takes for correct dossier
    QCOMPARE(repo.GetDegradeurSetForDossier(10001), DegradeurSet(Degradeur("1", 800, true)));
}

void TestPTRepo::GetBestDegradeurCombinations() {
    if (!InsertDegradeur(1, 1110, true)) {
        QFAIL("TestPTRepo::GetBestDegradeurCombinations Could not insert degradeur");
    }
    if (!InsertDegradeur(2, 2220, true)) {
        QFAIL("TestPTRepo::GetBestDegradeurCombinations Could not insert degradeur");
    }
    if (!InsertDegradeur(3, 3330, true)) {
        QFAIL("TestPTRepo::GetBestDegradeurCombinations Could not insert degradeur");
    }

    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int max_um_plexi_diff(110);

        auto sortedDegradeurs0 = repo.GetBestDegradeurCombinations(50000, max_um_plexi_diff);
        QVERIFY(sortedDegradeurs0.empty());

        auto sortedDegradeurs1 = repo.GetBestDegradeurCombinations(1100, max_um_plexi_diff);
        QCOMPARE((int)sortedDegradeurs1.size(), 1);
        DegradeurSet expDegSet1(Degradeur("1", 1110, true));
        QCOMPARE(sortedDegradeurs1.at(0), expDegSet1);

        auto sortedDegradeurs2 = repo.GetBestDegradeurCombinations(2200, max_um_plexi_diff);
        QCOMPARE((int)sortedDegradeurs2.size(), 1);
        DegradeurSet expDegSet2(Degradeur("2", 2220, true));
        QCOMPARE(sortedDegradeurs2.at(0), expDegSet2);

        auto sortedDegradeurs3 = repo.GetBestDegradeurCombinations(3300, max_um_plexi_diff);
        QCOMPARE((int)sortedDegradeurs3.size(), 2);
        DegradeurSet expDegSet3a(Degradeur("3", 3330, true));
        QCOMPARE(sortedDegradeurs3.at(0), expDegSet3a);
        DegradeurSet expDegSet3b (Degradeur("1", 1110, true), Degradeur("2", 2220, true));
        QCOMPARE(sortedDegradeurs3.at(1), expDegSet3b);

        auto sortedDegradeurs4 = repo.GetBestDegradeurCombinations(4400, max_um_plexi_diff);
        QCOMPARE((int)sortedDegradeurs4.size(), 1);
        DegradeurSet expDegSet4(Degradeur("1", 1110, true), Degradeur("3", 3330, true));
        QCOMPARE(sortedDegradeurs4.at(0), expDegSet4);

        auto sortedDegradeurs5 = repo.GetBestDegradeurCombinations(5500, max_um_plexi_diff);
        QCOMPARE((int)sortedDegradeurs5.size(), 1);
        DegradeurSet expDegSet5(Degradeur("2", 2220, true), Degradeur("3", 3330, true));
        QCOMPARE(sortedDegradeurs5.at(0), expDegSet5);
    }
    catch(...) {
        QFAIL("TestPTRepo::GetBestDegradeurCombination An exception was thrown in the test");
    }
}

void TestPTRepo::GetModulateursFullfillingCriteria() {
    if (!InsertModulateur(1, 5.0, 5.5, 8.0, 1000, 0, 1.0)) {
        QFAIL("TestPTRepo::GetModulateurs Could not insert modulateur");
    }
    if (!InsertModulateur(2, 5.1, 8.5, 15.0, 1000, 0, 1.0)) {
        QFAIL("TestPTRepo::GetModulateurs Could not insert modulateur");
    }
    if (!InsertModulateur(3, 8.0, 5.4, 10.0, 1000, 0, 1.0)) {
        QFAIL("TestPTRepo::GetModulateurs Could not insert modulateur");
    }
    if (!InsertModulateur(4, 2.0, 3.5, 12.0, 1000, 0, 1.0)) {
        QFAIL("TestPTRepo::GetModulateurs Could not insert modulateur");
    }

    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto modulateurs1 = repo.GetModulateurs(0.0, 10.0, 0.0, 10.0, 0.0, 20.0);
        QCOMPARE((int)modulateurs1.size(), 4);
        auto modulateurs2 = repo.GetModulateurs(5.1, 9.0, 0.0, 10.0, 0.0, 20.0);
        QCOMPARE((int)modulateurs2.size(), 2);
        QCOMPARE(modulateurs2.at(0).id(), QString("2"));
        QCOMPARE(modulateurs2.at(1).id(), QString("3"));
        auto modulateurs3 = repo.GetModulateurs(0.0, 10.0, 5.5, 10.0, 0.0, 20.0);
        QCOMPARE((int)modulateurs3.size(), 2);
        QCOMPARE(modulateurs3.at(0).id(), QString("1"));
        QCOMPARE(modulateurs3.at(1).id(), QString("2"));
        auto modulateurs4 = repo.GetModulateurs(0.0, 10.0, 0.0, 10.0, 10.0, 12.0);
        QCOMPARE((int)modulateurs4.size(), 2);
        QCOMPARE(modulateurs4.at(0).id(), QString("3"));
        QCOMPARE(modulateurs4.at(1).id(), QString("4"));
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::GetModulateurs An exception was thrown in the test");
    }
}

void TestPTRepo::GetModulateurById() {
    try {
        QSqlQuery query(QSqlDatabase::database(dbConnName_));
        if (!query.exec("INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, um_degradeur, um_plexi_weight, available,"
                        " opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod, decalage, conv_criteria)"
                        " VALUES(1,5.0,5.5,8.0,3000, '{\"(800, 0.3)\", \"(1600, 0.5)\", \"(2400, 0.44)\"}', true, 'Joel99', 'Shifted bragg', 4, 0.8, 20.0, 10.0, 0.4, 0.0001);")) {
            QFAIL("TestPTRepo::GetModulateurById Failed insert modulateur");
        }
        query.clear();

        if (!query.exec("INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, um_degradeur, um_plexi_weight, available,"
                        " opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod, decalage, conv_criteria)"
                        " VALUES(2,5.0,5.5,8.0,2000, '{\"(0, 1.0)\"}', true, 'Petter2017', 'MCNPX', 3, 0.8, 25.0, 15.0, 0.4, 0.0001);")) {
            QFAIL("TestPTRepo::GetModulateurById Failed insert modulateur");
        }
        query.clear();

        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        std::map<int, double> um_plexi_weight1;
        um_plexi_weight1[800] = 0.3;
        um_plexi_weight1[1600] = 0.5;
        um_plexi_weight1[2400] = 0.44;
        Modulateur exp_mod1(QString("1"), 5.0, 5.5, 8.0, 3000, um_plexi_weight1, true, "Joel99", "Shifted bragg", 4, 0.8, 20.0, 10.0, 0.4, 0.0001);
        auto mod1 = repo.GetModulateur(1);
        QCOMPARE(mod1, exp_mod1);

        std::map<int, double> um_plexi_weight2;
        um_plexi_weight2[0] = 1.0;
        Modulateur exp_mod2(QString("2"), 5.0, 5.5, 8.0, 2000, um_plexi_weight2, true, "Petter2017", "MCNPX", 3, 0.8, 25.0, 15.0, 0.4, 0.0001);
        auto mod2 = repo.GetModulateur(2);
        QCOMPARE(mod2, exp_mod2);

    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::GetModulateur An exception was thrown in the test");
    }
}

void TestPTRepo::SaveModulateurItems() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        std::map<int, double> weights1;
        weights1.emplace(0, 1.0);
        weights1.emplace(800, 0.422);
        weights1.emplace(1600, 0.233);
        Modulateur m1("123", 10.0, 12.0, 23.0, 800, weights1, true, "optalg", "dataset", 4, 0.8, 22.0, 11.0, 0.4, 0.0001);
        QString fraiseuse_prog("This is a modulateur fraiseuse program");

        repo.SaveModulateurItems(m1, fraiseuse_prog, false);
        Modulateur exp_m1("123", 0.0, 0.0, 0.0, 800, weights1, false, "optalg", "dataset", 4, 0.8, 22.0, 11.0, 0.4, 0.0001);
        Modulateur retr_m1(repo.GetModulateur(m1.id().toInt()));
        QCOMPARE(exp_m1, retr_m1);
        QCOMPARE(fraiseuse_prog, repo.GetTreatmentFraiseuseProgram(m1.id().toInt(), Modulateur::program_type));

        try {
             repo.SaveModulateurItems(m1, fraiseuse_prog, false);
             QFAIL("SaveModulateurItems should have thrown1");
        }
        catch (...) {}

        repo.SaveModulateurItems(m1, fraiseuse_prog, true);
    }
    catch (std::exception& exc) {
         QFAIL("TestPTRepo::SaveModulateurItems An exception was thrown in the test");
    }
}

void TestPTRepo::SaveModulateur() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        std::map<int, double> weights1;
        weights1.emplace(0, 1.0);
        weights1.emplace(800, 0.422);
        weights1.emplace(1600, 0.233);
        Modulateur m1("123", 10.0, 12.0, 23.0, 800, weights1, true, "optalg", "dataset", 4, 0.8, 22.0, 11.0, 0.4, 0.0001);

        repo.SaveModulateur(m1);
        Modulateur m1_retr(repo.GetModulateur(m1.id().toInt()));
        // measured parcours and mod is set to 0 when inserting a new modulateur, available is set to false
        Modulateur m1_exp("123", 0.0, 0.0, 0.0, 800, weights1, false, "optalg", "dataset", 4, 0.8, 22.0, 11.0, 0.4, 0.0001);

        QCOMPARE(m1_exp, m1_retr);

        try {
            repo.SaveModulateur(m1);
            QFAIL("TestPTRepo::SaveModulateur Test should have thrown1");
        }
        catch (...) {}

    }
    catch (std::exception& exc) {
         QFAIL("TestPTRepo::SaveModulateur An exception was thrown in the test");
    }
}

void TestPTRepo::UpdateModulateur() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        std::map<int, double> weights1;
        weights1.emplace(0, 1.0);
        weights1.emplace(800, 0.422);
        weights1.emplace(1600, 0.233);
        Modulateur m1("123", 10.0, 12.0, 23.0, 800, weights1, true, "optalg", "dataset", 4, 0.8, 22.0, 11.0, 0.4, 0.0001);

        std::map<int, double> weights2;
        weights2.emplace(0, 0.9);
        weights2.emplace(800, 0.322);
        weights2.emplace(1600, 0.133);
        weights2.emplace(2400, 0.033);
        Modulateur m2("123", 110.0, 112.0, 123.0, 1800, weights2, true, "optalg2", "dataset2", 14, 1.8, 122.0, 111.0, 1.4, 1.0001);

        repo.SaveModulateur(m1);
        repo.UpdateModulateur(m2);
        Modulateur exp_m2("123", 0.0, 0.0, 0.0, 1800, weights2, false, "optalg2", "dataset2", 14, 1.8, 122.0, 111.0, 1.4, 1.0001);
        Modulateur retr_m2(repo.GetModulateur(m1.id().toInt()));
        QCOMPARE(exp_m2, retr_m2);

        try {
            // update non-existing modulateur should throw
            Modulateur m3("666", 110.0, 112.0, 123.0, 1800, weights2, true, "optalg2", "dataset2", 14, 1.8, 122.0, 111.0, 1.4, 1.0001);
            repo.UpdateModulateur(m3);
            QFAIL("TestPTRepo::UpdateModulateur Test should have thrown1");
        }
        catch (...) {}

    }
    catch (std::exception& exc) {
         QFAIL("TestPTRepo::UpdateModulateur An exception was thrown in the test");
    }
}

void TestPTRepo::ModulateurExist() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        std::map<int, double> weights1;
        weights1.emplace(0, 1.0);
        weights1.emplace(800, 0.422);
        weights1.emplace(1600, 0.233);
        Modulateur m1("123", 10.0, 12.0, 23.0, 800, weights1, true, "optalg", "dataset", 4, 0.8, 22.0, 11.0, 0.4, 0.0001);
        repo.SaveModulateur(m1);

        QVERIFY(repo.ModulateurExist(m1.id().toInt()));
        QVERIFY(!repo.ModulateurExist(1234));
    }
    catch (std::exception& exc) {
         QFAIL("TestPTRepo::ModulateurExist An exception was thrown in the test");
    }
}

void TestPTRepo::GetModulateurByDossier() {
    if (!InsertModulateur(4, 10.0, 10.0, 10.0, 1000, 0, 1.0)) {
        QFAIL("TestPTRepo::GetModulateurByDossier Failed insert modulateur");
    }
    if (!InsertModulateur(2, 10.0, 10.0, 10.0, 1000, 0, 1.0)) {
        QFAIL("TestPTRepo::GetModulateurByDossier Failed insert modulateur");
    }

    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        repo.CreateTreatment(10001, 10000, std::vector<double>(), 2, 10000, 10000, QString("t1"));
        repo.CreateTreatment(10002, 10000, std::vector<double>(), 4, 10000, 10000, QString("t1"));

        QCOMPARE(repo.GetModulateurForDossier(10001).id(), QString("2"));
        QCOMPARE(repo.GetModulateurForDossier(10002).id(), QString("4"));
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::GetModulateur An exception was thrown in the test");
    }
}


void TestPTRepo::CreateAndRetrieveMCNPXData() {
    try {
        QSqlQuery query(QSqlDatabase::database(dbConnName_));
        if (!query.exec("INSERT INTO MCNPXDoseCurve(um_plexi, monitor_units, depth_dose_array)"
                        " VALUES(1600, 3.2, '{\"(1, 10)\", \"(3,11)\", \"(3, 12)\"}');")) {
            QFAIL("TestPTRepo::CreateAndRetrieveMCNPXPUM Could not insert data");
        }
        if (!query.exec("INSERT INTO MCNPXDoseCurve(um_plexi, monitor_units, depth_dose_array)"
                        " VALUES(800, 2.3, '{\"(1, 10)\", \"(2,11)\", \"(3, 12)\"}');")) {
            QFAIL("TestPTRepo::CreateAndRetrieveMCNPXUM Could not insert data");
        }

        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto mcnpx_data = repo.GetMCNPXData();

        std::vector<int> exp_um_plexi {800, 1600};
        std::vector<double> exp_monitor_units {2.3, 3.2};
        std::vector<DepthDose> depth_dose_vector1 { DepthDose(1, 10), DepthDose(2, 11), DepthDose(3, 12) };
        DepthDoseCurve exp_curve1(depth_dose_vector1);
        std::vector<DepthDose> depth_dose_vector2 { DepthDose(1, 10), DepthDose(3, 11), DepthDose(3, 12) };
        DepthDoseCurve exp_curve2(depth_dose_vector2);
        std::vector<DepthDoseCurve> exp_curves { exp_curve1, exp_curve2 };

        QCOMPARE(mcnpx_data.monitor_units(), exp_monitor_units);
        QCOMPARE(mcnpx_data.um_plexi() , exp_um_plexi);
        QCOMPARE(mcnpx_data.depth_dose_curves(), exp_curves);
    }
    catch (...) {
         QFAIL("TestPTRepo::CreateAndRetrieveMCNPXData An exception was thrown in the test");
    }
}

void TestPTRepo::SaveAndGetCollimator() {
    try {        
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        // std case
        Collimator coll1a(10000, Patient(10000, "REF",  "REF"), "((1.1,2),(3.4,10.1),(1.1,2))");
        repo.SaveCollimator(coll1a);
        Collimator coll2a(10000, Patient(10000, "REF",  "REF"), "((2.1,2),(3.4,10.1),(1.1,2))");
        repo.SaveCollimator(coll2a);
        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));
        Collimator coll3a(10001, Patient(10000, "REF",  "REF"), "((1.1,2),(3.4,10.1),(1.1,2))");
        repo.SaveCollimator(coll3a);

        auto coll_retr = repo.GetCollimator(10000);
        QVERIFY(coll2a.InternalEdges() == coll_retr.InternalEdges());
        QVERIFY(coll2a.Dossier() == coll_retr.Dossier());

        // wrong patient
        Collimator coll2(10000, Patient(3, "MUPP",  "REF"), "((1.1,2),(3.4,10.1),(1.1,2))");
        try {
            repo.SaveCollimator(coll2);
            QFAIL("TestPTRepo::SaveAndGetCollimator SaveCollimator should have thrown an exception since patient is wrong");
        }
        catch(...) {}

        // non existing dossier
        Collimator coll3(20000, Patient(10000, "REF",  "REF"), "((1.1,2),(3.4,10.1),(1.1,2))");
        try {
            repo.SaveCollimator(coll3);
            QFAIL("TestPTRepo::SaveAndGetCollimator SaveCollimator should have thrown an exception since dossier doesnt exist");
        }
        catch(...) {}

        // empty coordinates
        Collimator coll4(10000, Patient(10000, "REF",  "REF"), "()");
        try {
            repo.SaveCollimator(coll4);
            QFAIL("TestPTRepo::SaveAndGetCollimator SaveCollimator should have thrown an exception since no coordinates are given");
        }
        catch(...) {}

    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::SaveAndGetCollimator An exception was thrown in the test");
    }
}

void TestPTRepo::SaveAndGetCompensateur() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        double diameter_eye(22.0);
        double sclere_thickness(2.0);
        Eye eye(diameter_eye, sclere_thickness);
        QCPCurveData center_point(0, 14.14, 15.15);
        double bolus_thickness(2.3);
        double parcours(23.0);
        double parcours_error(0.34);
        double error_radius_scaling(0.23);
        double base_thickness(0.1);

        Compensateur compensateur1(10000, eye, center_point, bolus_thickness, parcours, parcours_error, error_radius_scaling, base_thickness);
        repo.SaveCompensateur(compensateur1);
        Compensateur compensateur2(10000, eye, center_point, bolus_thickness, parcours, parcours_error, error_radius_scaling + 0.01, base_thickness);
        repo.SaveCompensateur(compensateur2);
        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));
        Compensateur compensateur3(10001, eye, center_point, bolus_thickness, parcours, parcours_error, error_radius_scaling, base_thickness);
        repo.SaveCompensateur(compensateur3);

        Compensateur retr_compensateur(repo.GetCompensateur(10000));

        QCOMPARE(compensateur2, retr_compensateur);
        QVERIFY(compensateur1 != retr_compensateur);
        QVERIFY(compensateur3 != retr_compensateur);
    }
    catch (std::exception& exc) {
        QString message("TestPTRepo::SaveAndGetCompensateur An exception was thrown in the test: " + QString(exc.what()));
        QFAIL(message.toStdString().c_str());
    }
}

void TestPTRepo::SaveAndGetMiscFraiseuseProgram() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.SaveMiscFraiseuseProgram(QString("name1"), QString("HF"), QDateTime(QDate(2016,1,4)), QString("prog1"));
        repo.SaveMiscFraiseuseProgram(QString("name2"), QString("HF"), QDateTime(QDate(2016,5,4)), QString("prog2"));
        repo.SaveMiscFraiseuseProgram(QString("name1"), QString("DIVERS"), QDateTime(QDate(2017,5,4)), QString("prog3"));

        QString program10000(repo.GetMiscFraiseuseProgram(QString("name1"), QString("HF")));
        QCOMPARE(program10000, QString("prog1"));

        QString program10001(repo.GetMiscFraiseuseProgram(QString("name1"), QString("DIVERS")));
        QCOMPARE(program10001, QString("prog3"));

        try {
            repo.GetMiscFraiseuseProgram(QString("name1"), QString("rep3"));
            QFAIL("TestPTRepo::SaveAndGetMiscFraiseProgram An exception should have been thrown 1");
        }
        catch (...) {}

        try {
            repo.GetMiscFraiseuseProgram(QString("name3"), QString("HF"));
            QFAIL("TestPTRepo::SaveAndGetMiscFraiseProgram An exception should have been thrown 2");
        }
        catch (...) {}
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::SaveAndGetMiscFraiseuseProgram An exception was thrown in the test");
    }
}

void TestPTRepo::GetFraiseuseMiscPrograms() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        QDateTime d1(QDate(2016,1,4));
        QDateTime d2(QDate(2016,5,4));
        QDateTime d3(QDate(2017,5,4));
        repo.SaveMiscFraiseuseProgram(QString("name1"), QString("HF"), d1, QString("prog1"));
        repo.SaveMiscFraiseuseProgram(QString("name1"), QString("HF"), d2, QString("prog2"));
        repo.SaveMiscFraiseuseProgram(QString("name2"), QString("DIVERS"), d3, QString("prog3"));

        std::map<QDateTime, QString> progs1(repo.GetFraiseuseMiscPrograms(QString("HF")));
        QCOMPARE((int)progs1.size(), 1);
        QCOMPARE(progs1.at(d2), QString("name1"));

        std::map<QDateTime, QString> progs2(repo.GetFraiseuseMiscPrograms(QString("DIVERS")));
        QCOMPARE((int)progs2.size(), 1);
        QCOMPARE(progs2.at(d3), QString("name2"));

        try {
            repo.GetFraiseuseMiscPrograms(QString("NOTEXISTING"));
            QFAIL("TestPTRepo::GetFraiseusePrograms Should have thrown");
        }
        catch (...) {}
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::SaveAndGetMiscFraiseuseProgram An exception was thrown in the test");
    }
}

void TestPTRepo::SaveAndGetTreatmentFraiseuseProgram() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();
        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        repo.SaveTreatmentFraiseuseProgram(10000, Compensateur::program_type, QDateTime(QDate(2016,1,4)), QString("This is\na fraiseuse compensateur program\ndossier 10000\n"));
        repo.SaveTreatmentFraiseuseProgram(10000, Collimator::program_type, QDateTime(QDate(2016,5,4)), QString("This is\na fraiseuse collimateur program\ndossier 10000\n"));
        repo.SaveTreatmentFraiseuseProgram(10001, Compensateur::program_type, QDateTime(QDate(2017,5,4)), QString("This is\nyet another fraiseuse compensateur program\ndossier 10001\n"));

        QString comp10000(repo.GetTreatmentFraiseuseProgram(10000, Compensateur::program_type));
        QCOMPARE(comp10000, QString("This is\na fraiseuse compensateur program\ndossier 10000\n"));

        QString coll10000(repo.GetTreatmentFraiseuseProgram(10000, Collimator::program_type));
        QCOMPARE(coll10000, QString("This is\na fraiseuse collimateur program\ndossier 10000\n"));

        QString comp10001(repo.GetTreatmentFraiseuseProgram(10001, Compensateur::program_type));
        QCOMPARE(comp10001, QString("This is\nyet another fraiseuse compensateur program\ndossier 10001\n"));

        try {
            repo.GetTreatmentFraiseuseProgram(10002, Compensateur::program_type);
            QFAIL("TestPTRepo::SaveAndGetTreatmentFraiseProgram An exception should have been thrown");
        }
        catch (...) {}
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::SaveAndGetTreatmentFraiseuseProgram An exception was thrown in the test");
    }
}

void TestPTRepo::GetFraiseuseTreatmentPrograms() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.CreateTreatment(10001, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));
        QDateTime d1(QDate(2016,1,4));
        QDateTime d2(QDate(2016,5,4));
        QDateTime d3(QDate(2017,5,4));
        QDateTime d4(QDate(2017,6,4));
        repo.SaveTreatmentFraiseuseProgram(10000, Compensateur::program_type, d1, "prog1");
        repo.SaveTreatmentFraiseuseProgram(10000, Compensateur::program_type, d2, "prog2");
        repo.SaveTreatmentFraiseuseProgram(10001, Compensateur::program_type, d3, "prog3");
        repo.SaveTreatmentFraiseuseProgram(10001, Collimator::program_type, d4, "prog4");

        std::map<QDateTime, int> comp_progs(repo.GetFraiseuseTreatmentPrograms(Compensateur::program_type));
        QCOMPARE((int)comp_progs.size(), 2);
        QCOMPARE(comp_progs.at(d2), 10000);
        QCOMPARE(comp_progs.at(d3), 10001);

        std::map<QDateTime, int> coll_progs(repo.GetFraiseuseTreatmentPrograms(Collimator::program_type));
        QCOMPARE(coll_progs.at(d4), 10001);
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::GetTreatmentFraiseuseProgram An exception was thrown in the test");
    }
}

void TestPTRepo::SetAndGetDossierComments() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        QCOMPARE(repo.GetDossierComments(10000), QString(""));
        repo.SetDossierComments(10000, QString("hejda"));
        QCOMPARE(repo.GetDossierComments(10000), QString("hejda"));
    }
    catch (std::exception& exc) {
        QFAIL("SetAndGetDossierComments Exception thrown in test");
    }
}

void TestPTRepo::SetAndGetActiveTreatment() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.SetActiveTreatment(10000);
        QCOMPARE(repo.GetActiveTreatment(), 10000);

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0, 3.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));
        repo.SetActiveTreatment(10001);
        QCOMPARE(repo.GetActiveTreatment(), 10001);
    }
    catch (std::exception& exc) {
        QFAIL("SetAndGetActiveTreatment Exception thrown in test");
    }
}

void TestPTRepo::SetAndGetActiveTreatment_ErrorCases() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        try {
            repo.SetActiveTreatment(10001);
            QFAIL("SetAndGetActiveTreatment_ErrorCases Should have thrown 1");
        }
        catch (std::exception& exc) {}
    }
    catch (std::exception& exc) {
        QFAIL("SetAndGetActiveTreatment_ErrorCases Exception thrown in test");
    }
}

void TestPTRepo::SetAndGetActiveTreatmentStarted() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.SetActiveTreatment(10000);
        QCOMPARE(repo.IsActiveTreatmentStarted(), false);
        repo.SetActiveTreatmentStarted(true);
        QCOMPARE(repo.IsActiveTreatmentStarted(), true);

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0, 3.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));

        repo.SetActiveTreatment(10001);
        QCOMPARE(repo.GetActiveTreatment(), 10001);
        QCOMPARE(repo.IsActiveTreatmentStarted(), false);
        repo.SetActiveTreatmentStarted(true);
        QCOMPARE(repo.IsActiveTreatmentStarted(), true);
    }
    catch (std::exception& exc) {
        QFAIL("SetAndGetActiveTreatment Exception thrown in test");
    }
}

void TestPTRepo::SaveAndGetBaliseLevels() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto no_levels = repo.GetInstantaneousBaliseLevels(QDate(2017, 1, 1), QDate(2017, 1, 2));
        QVERIFY(no_levels.empty());

        BaliseLevel l1(QDateTime(QDate(2017, 1, 1), QTime(1, 2, 3)), 1.11);
        repo.SaveBaliseInstantaneousLevel(l1);
        auto one_level = repo.GetInstantaneousBaliseLevels(QDate(2017, 1, 1), QDate(2017, 1, 2));
        std::vector<BaliseLevel> exp_levels1 = { l1 };
        QCOMPARE(one_level, exp_levels1);

        BaliseLevel l2(QDateTime(QDate(2017, 1, 2), QTime(1, 2, 3)), 2.22);
        repo.SaveBaliseInstantaneousLevel(l2);
        std::vector<BaliseLevel> exp_levels2 = { l1, l2 };
        auto two_levels = repo.GetInstantaneousBaliseLevels(QDate(2017, 1, 1), QDate(2017, 1, 2));
        QCOMPARE(two_levels, exp_levels2);

        BaliseLevel l3(QDateTime(QDate(2017, 1, 3), QTime(1, 2, 3)), 3.33);
        repo.SaveBaliseInstantaneousLevel(l3);
        std::vector<BaliseLevel> exp_levels3 = { l2, l3 };
        auto two_levels2 = repo.GetInstantaneousBaliseLevels(QDate(2017, 1, 2), QDate(2017, 1, 3));
        QCOMPARE(two_levels2, exp_levels3);
    }
    catch (std::exception& exc) {
        QFAIL("SaveAndGetBaliselevels Exception thrown in test");
    }
}

void TestPTRepo::SaveAndGetBaliseLevels_ErrorCases() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        // inverted to from dates
        auto no_levels = repo.GetInstantaneousBaliseLevels(QDate(2017, 1, 1), QDate(2016, 1, 2));
        QVERIFY(no_levels.empty());
    }
    catch (std::exception& exc) {
        QFAIL("SaveAndGetBaliselevels_ErrorCases Exception thrown in test");
    }
}

void TestPTRepo::SaveAndGetBaliseRecord() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0, 3.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));

        SeanceRecord s1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, s1);
        SeanceRecord s2 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(2, s2);
        SeanceRecord s3 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(3, s3);

        BaliseRecord b1(QDateTime::currentDateTime(), dossier, 1, 2.012, 2.5, 3.0, 4.0, 13.0);
        repo.SaveBaliseRecord(b1);

        BaliseRecord b2(QDateTime::currentDateTime(), dossier, 2, 2.0, 3.1, 3.055, 4.0, 6.25);
        repo.SaveBaliseRecord(b2);

        BaliseRecord b3(QDateTime::currentDateTime(), dossier, 3, 2.0, 5.5, 3.550, 4.0, 5.625);
        repo.SaveBaliseRecord(b3);

        QCOMPARE(repo.GetBaliseRecord(s1), b1);
        QCOMPARE(repo.GetBaliseRecord(s2), b2);
        QCOMPARE(repo.GetBaliseRecord(s3), b3);
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::SaveAndGetBaliseRecord An exception was thrown in the test");
    }
}

void TestPTRepo::SaveAndGetBaliseRecord_ErrorCases() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));

        SeanceRecord s1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, s1);

        try {
            BaliseRecord b1(QDateTime::currentDateTime(), dossier, 111, 2.0, 2.5, 3.0, 4.0, 13.0);
            repo.SaveBaliseRecord(b1);
            QFAIL("Should not work since there is no seancerecord with id 111");
        }
        catch (std::exception& exc) {}
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::SaveAndGetBaliseRecord_ErrorCases An exception was thrown in the test");
    }
}

void TestPTRepo::GetBaliseRecords() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        const int dossier(10001);
        std::vector<double> dose_pre { 1.0, 2.0, 3.0 };
        repo.CreateTreatment(dossier, 10000, dose_pre, 10000, 10000, 10000, QString("t1"));

        SeanceRecord s1 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(1, s1);
        SeanceRecord s2 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(2, s2);
        SeanceRecord s3 = SeanceRecord::GetRandom();
        repo.SaveSeanceRecord(3, s3);

        BaliseRecord b1(QDateTime(QDate(2017, 1, 13)), dossier, 1, 1.371, 2.012, 3.0, 4.0, 13.0);
        repo.SaveBaliseRecord(b1);

        BaliseRecord b2(QDateTime(QDate(2017, 1, 14)), dossier, 2, 1.372, 2.0, 3.055, 4.0, 5.625);
        repo.SaveBaliseRecord(b2);

        BaliseRecord b3(QDateTime(QDate(2017, 1, 15)), dossier, 3, 1.373, 2.0, 3.550, 4.0, 2.875);
        repo.SaveBaliseRecord(b3);

        std::vector<BaliseRecord> empty_vector;
        std::vector<BaliseRecord> b1_vector;
        std::vector<BaliseRecord> b1b2_vector;
        std::vector<BaliseRecord> b1b2b3_vector;
        std::vector<BaliseRecord> b3_vector;
        b1_vector.push_back(b1);
        b1b2_vector.push_back(b1);
        b1b2_vector.push_back(b2);
        b1b2b3_vector.push_back(b1);
        b1b2b3_vector.push_back(b2);
        b1b2b3_vector.push_back(b3);
        b3_vector.push_back(b3);

        QCOMPARE(repo.GetBaliseRecords(QDate(2017, 1, 1), QDate(2017, 1, 2)), empty_vector);
        QCOMPARE(repo.GetBaliseRecords(QDate(2017, 1, 1), QDate(2017, 1, 13)), b1_vector);
        QCOMPARE(repo.GetBaliseRecords(QDate(2017, 1, 13), QDate(2017, 1, 14)), b1b2_vector);
        QCOMPARE(repo.GetBaliseRecords(QDate(2017, 1, 12), QDate(2017, 1, 16)), b1b2b3_vector);
        QCOMPARE(repo.GetBaliseRecords(QDate(2017, 1, 15), QDate(2017, 1, 16)), b3_vector);
    }
    catch (std::exception& exc) {
        QFAIL("TestPTRepo::SaveAndGetBaliseRecord An exception was thrown in the test");
    }
}

void TestPTRepo::SaveAndGetBaliseCalibration() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        try {
            repo.GetBaliseCalibration();
            QFAIL("SaveAndGetBaliseCalibration Should have thrown 1");
        } catch (std::exception& exc) {}

        BaliseCalibration calib1(QDateTime::currentDateTime(), 3333.33, 1.111e-7, -2.222, 5.634e-9, -3.222);
        repo.SaveBaliseCalibration(calib1);
        BaliseCalibration retr1 = repo.GetBaliseCalibration();
        QCOMPARE(retr1, calib1);

        BaliseCalibration calib2(QDateTime::currentDateTime(), 10.0, 20.0, -30.0, 40.0, -50.0);
        repo.SaveBaliseCalibration(calib2);
        BaliseCalibration retr2 = repo.GetBaliseCalibration();
        QCOMPARE(retr2, calib2);
        QVERIFY(retr2 != calib1);
    }
    catch (std::exception& exc) {
        QFAIL("SaveAndGetBaliseCalibration exception thrown in test");
    }
}

void TestPTRepo::GetBaliseConfiguration() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        repo.GetBaliseConfiguration();
        QVERIFY(true);
    }
    catch (std::exception& exc) {
        QFAIL("GetBaliseConfiguration exception thrown in test");
    }
}

void TestPTRepo::SaveAndGetXRayRecords() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        QCOMPARE(repo.GetXRayExposure(40000).first, 0);

        const int dossier(10001);
        repo.CreateTreatment(dossier, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        XRayRecord record1(QDateTime::currentDateTime(), dossier, 1.123E-5, 123.456);
        repo.SaveXRayRecord(record1);
        QCOMPARE(repo.GetXRayExposure(dossier).first, 1);
        QVERIFY(calc::AlmostEqual(repo.GetXRayExposure(dossier).second, 0.25*123.456*1.0E12, 0.01));
        QCOMPARE(repo.GetXRayRecords(dossier).at(0), record1);

        XRayRecord record2(QDateTime::currentDateTime(), dossier, 1.7E-5, 1.345);
        repo.SaveXRayRecord(record2);
        std::pair<int, double> xray_exposure = repo.GetXRayExposure(dossier);
        QCOMPARE(xray_exposure.first, 2);
        QVERIFY(calc::AlmostEqual(xray_exposure.second, 0.25*1.0E12*(123.456 + 1.345), 0.0001));
        QCOMPARE(repo.GetXRayRecords(dossier).at(0), record1);
        QCOMPARE(repo.GetXRayRecords(dossier).at(1), record2);
    }
    catch (std::exception& exc) {
        QFAIL("SaveAndGetXRayRecords exception thrown in test");
    }
}

void TestPTRepo::GetTreatmentTypes() {
    try {
        QSqlQuery query(QSqlDatabase::database(dbConnName_));
        if (!query.exec("INSERT INTO TreatmentType(name, dose_seance_std, dose_seance_boost) VALUES ('Rbra', '{2.123, 4.0, 55.001}', '{}');")) {
            qWarning() << "Query insert into patient failed: " << query.lastError();
            QFAIL("Failed setting up test");
        }
        query.clear();
        if (!query.exec("INSERT INTO TreatmentType(name, dose_seance_std, dose_seance_boost) VALUES ('Rrupp', '{55.001}', '{2.22, 19.111}');")) {
            qWarning() << "Query insert into patient failed: " << query.lastError();
            QFAIL("Failed setting up test");
        }
        query.clear();
        if (!query.exec("INSERT INTO TreatmentType(name, dose_seance_std, dose_seance_boost) VALUES ('Rambadabam', '{20.001, 10.0}', '{19.111}');")) {
            qWarning() << "Query insert into patient failed: " << query.lastError();
            QFAIL("Failed setting up test");
        }
        query.clear();


        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        std::vector<double> v1 {2.123, 4.0, 55.001};
        std::vector<double> v2 {};
        std::vector<double> v3 {55.001};
        std::vector<double> v4 {2.22, 19.111};
        std::vector<double> v5 {20.001, 10.0};
        std::vector<double> v6 {19.111};

        std::vector<TreatmentType> t = repo.GetTreatmentTypes();
        std::vector<TreatmentType> tfiltered;
        for (TreatmentType ti : t) {
            if (ti.Name().startsWith(QString("R"))) {
                tfiltered.push_back(ti);
            }
        }
        QCOMPARE(static_cast<int>(tfiltered.size()), 3);
        QCOMPARE(tfiltered.at(0), TreatmentType(QString("Rbra"), v1, v2));
        QCOMPARE(tfiltered.at(1), TreatmentType(QString("Rrupp"), v3, v4));
        QCOMPARE(tfiltered.at(2), TreatmentType(QString("Rambadabam"), v5, v6));
    }
    catch (...) {
        QFAIL("TestPTRepo::GetTreatmentTypes An exception was thrown in the test");
    }
}

void TestPTRepo::CheckReadWriteAccess() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);

        QCOMPARE(repo.CheckReadWriteAccess(), false);
        repo.Connect();
        QCOMPARE(repo.CheckReadWriteAccess(), true);
    }
    catch (std::exception& exc) {
        QFAIL("CheckReadWriteAccess exception thrown in test");
    }

}


void TestPTRepo::GetNonFinishedDossiers() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        std::vector<int> dossiers1 = repo.GetNonFinishedDossiers();

        QVERIFY(dossiers1.empty());

        const std::vector<double> doses = {1.11, 2.22, 3.33};
        std::list<SeanceRecord> records;
        std::vector<Seance> seances = { Seance(1, doses[0], records),
                                        Seance(2, doses[1], records),
                                        Seance(3, doses[2], records) };

        repo.CreateTreatment(1, 10000, doses, 10000, 10000, 20000, QString("t1"));
        repo.CreateTreatment(20, 10000, doses, 10000, 10000, 20000, QString("t1"));
        repo.CreateTreatment(999, 10000, doses, 10000, 10000, 20000, QString("t1"));

        std::vector<int> dossiers2 = repo.GetNonFinishedDossiers();
        QCOMPARE(static_cast<int>(dossiers2.size()), 3);
        QCOMPARE(dossiers2.at(0), 1);
        QCOMPARE(dossiers2.at(1), 20);
        QCOMPARE(dossiers2.at(2), 999);
    }
    catch (...) {
        QFAIL("An exception was thrown in the test GetNonFinishedDossiers");
    }
}

void TestPTRepo::GetBeamTransmission() {
    try {
        PTRepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto chambre = repo.GetChambre("FWT-849");

        repo.CreateTreatment(20000, 10000, std::vector<double>(), 10000, 10000, 10000, QString("t1"));

        // common
        std::vector<double> duration {1.1, 2.1, 3.1, 4.1};
        std::vector<int> um_del {4, 8, 12};
        std::vector<double> i_chambre1 {100.2, 50, 200.4, 66.6};
        std::vector<double> i_chambre2 {300.8, 44.5, 33};
        int um_prevu = 100;
        double charge = 1.0;
        double air_charge = 1.0;

        // 100, 50, 200, 100, 50
        DosimetryRecord record1(duration, i_chambre1, i_chambre2, 100.0, 50.0, um_prevu, um_del, charge, air_charge);
        DosimetryRecord record2(duration, i_chambre1, i_chambre2, 100.0, 25.0, um_prevu, um_del, charge, air_charge);
        DosimetryRecord record3(duration, i_chambre1, i_chambre2, 50.0, 50.0, um_prevu, um_del, charge, air_charge);
        DosimetryRecord record4(duration, i_chambre1, i_chambre2, 20.0, 10.0, um_prevu, um_del, charge, air_charge);
        DosimetryRecord record5(duration, i_chambre1, i_chambre2, 400.0, 100.0, um_prevu, um_del, charge, air_charge);


        // Dosi 1 (100, 0)
        std::list<DosimetryRecord> records1 = {record1};
        Dosimetry dosimetry1(QDateTime(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3))), true, false,
                            23.0, 1001,
                            chambre,
                            records1, "");
        repo.SaveDosimetry(10000, dosimetry1);

        // Dosi 2 (75, 25)
        std::list<DosimetryRecord> records2 = {record1, record2};
        Dosimetry dosimetry2(QDateTime(QDateTime(QDate(2015, 1, 2), QTime(1, 2, 3))), true, false,
                            23.0, 1001,
                            chambre,
                            records2, "");
        repo.SaveDosimetry(10000, dosimetry2);

        // Dosi 3 (80, 75.828754440516)
        std::list<DosimetryRecord> records3 = {record1, record2, record3, record4, record5};
        Dosimetry dosimetry3(QDateTime(QDateTime(QDate(2015, 1, 3), QTime(1, 2, 3))), true, false,
                            23.0, 1001,
                            chambre,
                            records3, "");
        repo.SaveDosimetry(10000, dosimetry3);

        // Dosi 4 (80, 75.828754440516)
        std::list<DosimetryRecord> records4 = {record1, record2, record3, record4, record5};
        Dosimetry dosimetry4(QDateTime(QDateTime(QDate(2015, 1, 4), QTime(1, 2, 3))), false, false,
                            23.0, 1001,
                            chambre,
                            records4, "");
        repo.SaveDosimetry(20000, dosimetry4);

        // Dosi 5 (150, 70.710678118655)
        std::list<DosimetryRecord> records5 = {record1, record2, record3, record3, record3};
        Dosimetry dosimetry5(QDateTime(QDateTime(QDate(2015, 1, 4), QTime(1, 2, 3))), true, false,
                            23.0, 1001,
                            chambre,
                            records5, "");
        repo.SaveDosimetry(10000, dosimetry5);

        auto bt0 = repo.GetBeamTransmissionSeries(QDate(2014, 1, 1), QDate(2014, 1, 1));

        auto bt1 = repo.GetBeamTransmissionSeries(QDate(2015, 1, 1), QDate(2015, 1, 1));
        QCOMPARE(bt1.time.size(), 1);
        QCOMPARE(bt1.error.size(), 1);
        QCOMPARE(bt1.value.size(), 1);
        QVERIFY(std::abs(bt1.value.at(0) - 100.0) < 0.0001);
        QVERIFY(std::abs(bt1.error.at(0) - 0.0) < 0.0001);
        QVERIFY(std::abs(bt1.time.at(0) - QDateTime(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3))).toTime_t()) < 0.0001);

        auto bt2 = repo.GetBeamTransmissionSeries(QDate(2015, 1, 2), QDate(2015, 1, 2));
        QCOMPARE(bt2.time.size(), 1);
        QCOMPARE(bt2.error.size(), 1);
        QCOMPARE(bt2.value.size(), 1);
        QVERIFY(std::abs(bt2.value.at(0) - 75.0) < 0.0001);
        QVERIFY(std::abs(bt2.error.at(0) - 35.355339059327) < 0.0001);
        QVERIFY(std::abs(bt2.time.at(0) - QDateTime(QDateTime(QDate(2015, 1, 2), QTime(1, 2, 3))).toTime_t()) < 0.0001);

        auto bt3 = repo.GetBeamTransmissionSeries(QDate(2015, 1, 3), QDate(2015, 1, 3));
        QCOMPARE(bt3.time.size(), 1);
        QCOMPARE(bt3.error.size(), 1);
        QCOMPARE(bt3.value.size(), 1);
        QVERIFY(std::abs(bt3.value.at(0) - 100.0) < 0.0001);
        QVERIFY(std::abs(bt3.error.at(0) - 61.237243569579) < 0.0001);
        QVERIFY(std::abs(bt3.time.at(0) - QDateTime(QDateTime(QDate(2015, 1, 3), QTime(1, 2, 3))).toTime_t()) < 0.0001);

        auto bt4 = repo.GetBeamTransmissionSeries(QDate(2015, 1, 4), QDate(2015, 1, 4));
        QCOMPARE(bt4.time.size(), 1);
        QCOMPARE(bt4.error.size(), 1);
        QCOMPARE(bt4.value.size(), 1);
        QVERIFY(std::abs(bt4.value.at(0) - 150.0) < 0.0001);
        QVERIFY(std::abs(bt4.error.at(0) - 70.710678118655) < 0.0001);
        QVERIFY(std::abs(bt4.time.at(0) - QDateTime(QDateTime(QDate(2015, 1, 4), QTime(1, 2, 3))).toTime_t()) < 0.0001);

        auto bt5 = repo.GetBeamTransmissionSeries(QDate(2015, 1, 1), QDate(2015, 1, 4));
        QCOMPARE(bt5.time.size(), 4);
        QCOMPARE(bt5.error.size(), 4);
        QCOMPARE(bt5.value.size(), 4);
        QVERIFY(std::abs(bt5.value.at(0) - 100.0) < 0.0001);
        QVERIFY(std::abs(bt5.error.at(0) - 0.0) < 0.0001);
        QVERIFY(std::abs(bt5.time.at(0) - QDateTime(QDateTime(QDate(2015, 1, 1), QTime(1, 2, 3))).toTime_t()) < 0.0001);
        QVERIFY(std::abs(bt5.value.at(3) - 150.0) < 0.0001);
        QVERIFY(std::abs(bt5.error.at(3) - 70.710678118655) < 0.0001);
        QVERIFY(std::abs(bt5.time.at(3) - QDateTime(QDateTime(QDate(2015, 1, 4), QTime(1, 2, 3))).toTime_t()) < 0.0001);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetBeamTransmission");
    }
}

