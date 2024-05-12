#include "TestQARepo.h"

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

#include "BeamProfile.h"


void TestQARepo::initTestCase() {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MQA.ini"), QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    dbHostName_ = settings.value("unittestdatabase/host", "unknown").toString();
    dbDatabaseName_ = settings.value("unittestdatabase/name", "unknown").toString();
    dbPort_ = settings.value("unittestdatabase/port", "0").toInt();
    dbUserName_ = settings.value("unittestdatabase/user", "unknown").toString();
    dbPassword_ = settings.value("unittestdatabase/password", "unknown").toString();
    dbConnName_ = "unittestSetup";
    if (dbHostName_ == "unknown") qCritical() << "TestQARepo::inittestCase Unknown db hostname";
    if (dbDatabaseName_ == "unknown") qCritical() << "TestQARepo::inittestCase Unknown db name";
    if (dbUserName_ == "unknown") qCritical() << "TestQARepo::inittestCase Unknown db username";
    if (dbPassword_ == "unknown") qCritical() << "TestQARepo::inittestCase Unknown db password";

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL7", dbConnName_);
    db.setHostName(dbHostName_);
    db.setDatabaseName(dbDatabaseName_);
    db.setPort(dbPort_);
    db.setUserName(dbUserName_);
    db.setPassword(dbPassword_);
    if (!db.open()) {
        qWarning() << "Dd could not be opened";
    }

    if (!LoadSqlResourceFile(":/scripts/createConstants.sql")) AbortUnitTests("createConstants");
    if (!LoadSqlResourceFile(":/scripts/createBeamMeasurement.sql")) AbortUnitTests("createBeamMeasurement");
}

void TestQARepo::AbortUnitTests(QString msg) {
    qCritical() << "TestQARepo Tests aborted: " << msg;
    cleanupTestCase();
    QFAIL(msg.toStdString().c_str());
}

bool TestQARepo::LoadSqlResourceFile(QString file) {
    QSqlQuery query(QSqlDatabase::database(dbConnName_));

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestQARepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    return query.exec(cmd);
}

void TestQARepo::cleanupTestCase() {
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

void TestQARepo::init() {
//    if (!LoadSqlResourceFile(":/scripts/setupUnitTests.sql")) {
//        AbortUnitTests("init");
//    }
}

void TestQARepo::cleanup() {
     QSqlQuery query(QSqlDatabase::database(dbConnName_));
     if (!query.exec("TRUNCATE MeasurementCurrents, MeasurementPoint, BeamProfile, BraggPeak, SOBP, Cube "
                     "RESTART IDENTITY")) {
         qWarning() << "Query truncate tables failed: " << query.lastError();
     }
     query.clear();
}

BeamProfile TestQARepo::GetBeamProfile(Axis axis, QDateTime timestamp, int smoothing, double noise) {
    return BeamProfile(QA_HARDWARE::SCANNER2D, axis, GetExampleProfile(), timestamp,
                       MeasurementCurrents(timestamp, 1.1, 2.2, 3.3, 4.4, 5.5), smoothing, noise);
}

BraggPeak TestQARepo::GetBraggPeak(QDateTime timestamp) {
    BraggPeak bp(DepthDoseMeasurement(QA_HARDWARE::WHEEL, GetMeasurementPoints(), timestamp, MeasurementCurrents(timestamp, 1.1, 2.2, 3.3, 4.4, 5.5), 0, 0.0),
                 2.0, 0.9, 30.7);

    return bp;
}

SOBP TestQARepo::GetSOBP(int dossier, QDateTime timestamp) {
    SOBP sobp(DepthDoseMeasurement(QA_HARDWARE::SCANNER3D, GetMeasurementPoints(), timestamp, MeasurementCurrents(timestamp, 1.1, 2.2, 3.3, 4.4, 5.5), 0, 0.0),
              dossier, 123, 10.123, "hejhej", 1.1, 30.7, 20.0, 19.0);
    return sobp;
}

std::vector<MeasurementPoint> TestQARepo::GetMeasurementPoints() {
    std::vector<MeasurementPoint> p;
    p.push_back(MeasurementPoint(Point(0, 0, 1), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 2), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 3), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 4), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 5), BeamSignal(0, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 7), BeamSignal(5, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 8), BeamSignal(6, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 9), BeamSignal(7, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 10), BeamSignal(8, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 11), BeamSignal(9, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 12), BeamSignal(10, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 13), BeamSignal(8, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 14), BeamSignal(6, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 15), BeamSignal(4, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 16), BeamSignal(2, 1)));
    p.push_back(MeasurementPoint(Point(0, 0, 17), BeamSignal(0, 1)));
    return p;
}

std::vector<MeasurementPoint> TestQARepo::GetExampleProfile() {
    std::vector<MeasurementPoint> points { MeasurementPoint(Point(1, 1, 0), BeamSignal(0, 1)),
                MeasurementPoint(Point(2, 2, 0), BeamSignal(2, 1)),
                MeasurementPoint(Point(3, 3, 0), BeamSignal(3, 1)),
                MeasurementPoint(Point(4, 4, 0), BeamSignal(4, 1)),
                MeasurementPoint(Point(5, 5, 0), BeamSignal(4, 1)),
                MeasurementPoint(Point(6, 6, 0), BeamSignal(4, 1)),
                MeasurementPoint(Point(7, 7, 0), BeamSignal(5, 1)),
                MeasurementPoint(Point(8, 8, 0), BeamSignal(4, 1)),
                MeasurementPoint(Point(9, 9, 0), BeamSignal(3, 1)),
                MeasurementPoint(Point(10, 10, 0), BeamSignal(2, 1)),
                MeasurementPoint(Point(11, 11, 0), BeamSignal(1, 1)),
                MeasurementPoint(Point(12, 12, 0), BeamSignal(0, 1)) };
    return points;
}

void TestQARepo::SaveAndGetBeamProfile() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        QDateTime timestamp = QDateTime::currentDateTime();
        Axis axis = Axis::X;

        // some measurement points
        timestamp = timestamp.addDays(1);
        BeamProfile profile_nonempty(QA_HARDWARE::SCANNER2D, axis, GetExampleProfile(), timestamp,
                            MeasurementCurrents(timestamp, 1.1, 2.2, 3.3, 4.4, 5.5), 6, 0.0);
        repo.SaveBeamProfile(profile_nonempty);
        BeamProfile read_profile_nonempty = repo.GetBeamProfile(axis, timestamp);
        QCOMPARE(profile_nonempty, read_profile_nonempty);

        // timestamp does not exist
        try {
            repo.GetBeamProfile(axis, timestamp.addDays(2));
            QFAIL("SaveAndGetBeamProfile should have thrown 1");
        }
        catch(...){}

        // axis does not exist for timestamp
        try {
            repo.GetBeamProfile(Axis::Y, timestamp);
            QFAIL("SaveAndGetBeamProfile should have thrown 2");
        }
        catch(...){}

    }
    catch (std::exception& exc) {
        qDebug() << "SaveAndGetBeamProfile Exception thrown: " << exc.what();
        QFAIL("SaveAndGetBeamProfile exception thrown in test");
    }
}

//void TestQARepo::SaveAndGetBeamProfilesForSingleDate() {
//    try {
//        std::vector<BeamProfilePoint> points { BeamProfilePoint(1.0, 0.3, 1.5, 0.1), BeamProfilePoint(2.0, 0.31, 1.51, 0.11) };
//        BeamProfile p1(points, Axis::X, 1, 70.0, 35.5, 40.0, 45.0, QDateTime(QDate(2017, 2, 4)));
//        BeamProfile p2(points, Axis::Y, 2, 70.0, 35.5, 40.0, 45.0, QDateTime(QDate(2017, 2, 4)));
//        BeamProfile p3(points, Axis::X, 1, 72.0, 35.5, 40.0, 45.0, QDateTime(QDate(2017, 2, 4)));
//        BeamProfile p4(points, Axis::Y, 1, 70.0, 35.5, 40.0, 47.0, QDateTime(QDate(2017, 2, 5)));
//        BeamProfile p5(std::vector<BeamProfilePoint>(), Axis::Y, 1, 70.0, 35.5, 40.0, 47.0, QDateTime(QDate(2017, 2, 6)));

//        DbRepository repo("unittest", dbHostName_,
//                          dbDatabaseName_, dbUserName_,
//                          dbPassword_);
//        repo.Connect();
//        repo.SaveBeamProfile(p1, QDateTime(QDate(2017, 2, 4)));
//        repo.SaveBeamProfile(p2, QDateTime(QDate(2017, 2, 4)));
//        repo.SaveBeamProfile(p3, QDateTime(QDate(2017, 2, 4)));
//        repo.SaveBeamProfile(p4, QDateTime(QDate(2017, 2, 5)));
//        repo.SaveBeamProfile(p5, QDateTime(QDate(2017, 2, 6)));

//        std::vector<BeamProfile> x_profiles = repo.GetBeamProfiles(Axis::X, QDate(2017, 2, 4));
//        QCOMPARE(static_cast<int>(x_profiles.size()), 2);
//        QCOMPARE(x_profiles.at(0), p1);
//        QCOMPARE(x_profiles.at(1), p3);

//        std::vector<BeamProfile> y_profiles = repo.GetBeamProfiles(Axis::Y, QDate(2017, 2, 4));
//        QCOMPARE(static_cast<int>(y_profiles.size()), 1);
//        QCOMPARE(y_profiles.at(0), p2);

//        std::vector<BeamProfile> y_profiles2 = repo.GetBeamProfiles(Axis::Y, QDate(2017, 2, 5));
//        QCOMPARE(static_cast<int>(y_profiles2.size()), 1);
//        QCOMPARE(y_profiles2.at(0), p4);

//        std::vector<BeamProfile> y_profiles3 = repo.GetBeamProfiles(Axis::Y, QDate(2017, 2, 6));
//        QCOMPARE(static_cast<int>(y_profiles3.size()), 1);
//        QCOMPARE(y_profiles3.at(0), p5);

//        std::vector<BeamProfile> y_profiles4 = repo.GetBeamProfiles(Axis::Y, QDate(2017, 2, 1));
//        QVERIFY(y_profiles4.empty());

//        std::vector<BeamProfile> x_profiles2 = repo.GetBeamProfiles(Axis::X, QDate(2017, 2, 5));
//        QVERIFY(x_profiles2.empty());

//    }
//    catch (std::exception& exc) {
//        qDebug() << "SaveAndGetBeamProfile Exception thrown: " << exc.what();
//        QFAIL("SaveAndGetBeamProfile exception thrown in test");
//    }
//}

void TestQARepo::GetBeamProfileSeries() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto b1_X = GetBeamProfile(Axis::X, QDateTime(QDate(2017, 10, 1)), 1, 0.0);
        auto b1_Y = GetBeamProfile(Axis::Y, QDateTime(QDate(2017, 10, 1)), 2, 0.0);

        auto b2_X = GetBeamProfile(Axis::X, QDateTime(QDate(2017, 10, 2)), 1, 0.0);
        auto b2_Y = GetBeamProfile(Axis::Y, QDateTime(QDate(2017, 10, 2), QTime(1, 2) ), 2, 0.0);
        auto b3_X = GetBeamProfile(Axis::X, QDateTime(QDate(2017, 10, 2)), 1, 0.0);

        auto b4_X = GetBeamProfile(Axis::X, QDateTime(QDate(2017, 10, 3)), 2, 0.0);
        auto b4_Y = GetBeamProfile(Axis::Y, QDateTime(QDate(2017, 10, 3)), 1, 0.0);

        auto b5_X = GetBeamProfile(Axis::X, QDateTime(QDate(2017, 10, 4)), 2, 0.0);
        auto b5_Y = GetBeamProfile(Axis::Y, QDateTime(QDate(2017, 10, 4)), 1, 0.0);

        repo.SaveBeamProfile(b2_X);
        repo.SaveBeamProfile(b1_X);
        repo.SaveBeamProfile(b3_X);
        repo.SaveBeamProfile(b4_X);
        repo.SaveBeamProfile(b5_X);
        repo.SaveBeamProfile(b2_Y);
        repo.SaveBeamProfile(b1_Y);
        repo.SaveBeamProfile(b4_Y);
        repo.SaveBeamProfile(b5_Y);

        BeamProfileSeries s1 = repo.GetBeamProfileSeries(Axis::X, QDate(2017, 10, 1), QDate(2017, 10, 1));
        QCOMPARE(s1.N(), 1);

        BeamProfileSeries s2 = repo.GetBeamProfileSeries(Axis::Y, QDate(2017, 10, 1), QDate(2017, 10, 1));
        QCOMPARE(s2.N(), 1);

        BeamProfileSeries s3 = repo.GetBeamProfileSeries(Axis::X, QDate(2017, 10, 1), QDate(2017, 11, 1));
        QCOMPARE(s3.N(), 5);

        BeamProfileSeries s5 = repo.GetBeamProfileSeries(Axis::X, QDate(2017, 10, 1), QDate(2017, 10, 2));
        QCOMPARE(s5.N(), 3);

        BeamProfileSeries s6 = repo.GetBeamProfileSeries(Axis::Y, QDate(2017, 10, 1), QDate(2017, 10, 2));
        QCOMPARE(s6.N(), 2);
    }
    catch (std::exception& exc) {
        qDebug() << "GetBeamProfileSeries Exception thrown: " << exc.what();
        QFAIL("GetBeamProfileSeries exception thrown in test");
    }
}

void TestQARepo::GetLastBeamProfile() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        QDateTime currentDay = QDateTime::currentDateTime();

        auto b0_X = GetBeamProfile(Axis::X, currentDay.addDays(-1), 1, 0.0);
        auto b1_X = GetBeamProfile(Axis::X, currentDay.addSecs(-100), 1, 0.0);
        auto b2_X = GetBeamProfile(Axis::X, currentDay.addSecs(-1000), 2, 0.0);
        auto b1_Y = GetBeamProfile(Axis::Y, currentDay, 3, 0.0);
        auto b2_Y = GetBeamProfile(Axis::Y, currentDay.addSecs(-50), 2, 0.0);

        repo.SaveBeamProfile(b2_X);
        repo.SaveBeamProfile(b1_X);
        repo.SaveBeamProfile(b0_X);
        repo.SaveBeamProfile(b1_Y);
        repo.SaveBeamProfile(b2_Y);

        auto bX = repo.GetLastBeamProfile(Axis::X);
        QCOMPARE(bX, b1_X);

        auto bY = repo.GetLastBeamProfile(Axis::Y);
        QCOMPARE(bY, b1_Y);

    }
    catch (std::exception& exc) {
        qDebug() << "GetBeamProfileSeries Exception thrown: " << exc.what();
        QFAIL("GetBeamProfileSeries exception thrown in test");
    }
}


void TestQARepo::SaveAndGetBraggPeak() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        // some measurement points
        BraggPeak braggpeak1 = GetBraggPeak(QDateTime::currentDateTime());
        BraggPeak braggpeak2 = GetBraggPeak(QDateTime::currentDateTime().addSecs(100));
        BraggPeak braggpeak3 = GetBraggPeak(QDateTime::currentDateTime().addSecs(-100));

        repo.SaveBraggPeak(braggpeak1);
        repo.SaveBraggPeak(braggpeak2);
        repo.SaveBraggPeak(braggpeak3);

        BraggPeak read_braggpeak1 = repo.GetBraggPeak(braggpeak1.depth_dose().GetTimestamp());
        BraggPeak read_braggpeak2 = repo.GetBraggPeak(braggpeak2.depth_dose().GetTimestamp());
        BraggPeak read_braggpeak3 = repo.GetBraggPeak(braggpeak3.depth_dose().GetTimestamp());

        QCOMPARE(braggpeak1, read_braggpeak1);
        QCOMPARE(braggpeak2, read_braggpeak2);
        QCOMPARE(braggpeak3, read_braggpeak3);
        QVERIFY(braggpeak1 != braggpeak2);
        QVERIFY(braggpeak2 != braggpeak3);

        // timestamp does not exist
        try {
            repo.GetBraggPeak(QDateTime::currentDateTime().addYears(1));
            QFAIL("SaveAndGetBraggPeak should have thrown 1");
        }
        catch(...){}
    }
    catch (std::exception& exc) {
        qDebug() << "SaveAndGetBraggPeak Exception thrown: " << exc.what();
        QFAIL("SaveAndGetBraggPeak exception thrown in test");
    }
}

void TestQARepo::SaveAndGetSOBP() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        // some measurement points
        SOBP sobp1(GetSOBP(10000, QDateTime::currentDateTime()));
        SOBP sobp2(GetSOBP(0, QDateTime::currentDateTime().addSecs(100)));
        SOBP sobp3(GetSOBP(10000, QDateTime::currentDateTime().addSecs(-100)));

        repo.SaveSOBP(sobp1);
        repo.SaveSOBP(sobp2);
        repo.SaveSOBP(sobp3);

        SOBP read_sobp1 = repo.GetSOBP(sobp1.depth_dose().GetTimestamp());
        SOBP read_sobp2 = repo.GetSOBP(sobp2.depth_dose().GetTimestamp());
        SOBP read_sobp3 = repo.GetSOBP(sobp3.depth_dose().GetTimestamp());

        QCOMPARE(sobp1, read_sobp1);
        QCOMPARE(sobp2, read_sobp2);
        QCOMPARE(sobp3, read_sobp3);
        QVERIFY(sobp1 != sobp2);
        QVERIFY(sobp2 != sobp3);

        // timestamp does not exist
        try {
            repo.GetSOBP(QDateTime::currentDateTime().addYears(1));
            QFAIL("SaveAndGetSOBP should have thrown 1");
        }
        catch(...){}
    }
    catch (std::exception& exc) {
        qDebug() << "SaveAndGetSOBP Exception thrown: " << exc.what();
        QFAIL("SaveAndGetSOBP exception thrown in test");
    }
}

void TestQARepo::SaveAndGetCube() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        std::map<Axis, AxisConfig> configs;
        configs[Axis::X] = AxisConfig(2, 0.3, 0.1);
        configs[Axis::Y] = AxisConfig(3, 0.4, 0.2);
        configs[Axis::Z] = AxisConfig(4, 0.5, 0.3);
        double noise = 0.03;

        std::vector<MeasurementPoint> points;
        points.push_back(MeasurementPoint(Point(0.3, 0.4, 0.5), BeamSignal(7, 1)));
        points.push_back(MeasurementPoint(Point(0.3, 0.4, 0.8), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.3, 0.4, 1.1), BeamSignal(8, 1)));
        points.push_back(MeasurementPoint(Point(0.3, 0.4, 1.4), BeamSignal(0, 0)));

        points.push_back(MeasurementPoint(Point(0.3, 0.6, 0.5), BeamSignal(4, 1)));
        points.push_back(MeasurementPoint(Point(0.3, 0.6, 0.8), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.3, 0.6, 1.1), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.3, 0.6, 1.4), BeamSignal(0, 0)));

        points.push_back(MeasurementPoint(Point(0.3, 0.8, 0.5), BeamSignal(6, 1)));
        points.push_back(MeasurementPoint(Point(0.3, 0.8, 0.8), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.3, 0.8, 1.1), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.3, 0.8, 1.4), BeamSignal(0, 0)));

        points.push_back(MeasurementPoint(Point(0.4, 0.4, 0.5), BeamSignal(2, 1)));
        points.push_back(MeasurementPoint(Point(0.4, 0.4, 0.8), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.4, 0.4, 1.1), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.4, 0.4, 1.4), BeamSignal(0, 0)));

        points.push_back(MeasurementPoint(Point(0.4, 0.6, 0.5), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.4, 0.6, 0.8), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.4, 0.6, 1.1), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.4, 0.6, 1.4), BeamSignal(0, 0)));

        points.push_back(MeasurementPoint(Point(0.4, 0.8, 0.5), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.4, 0.8, 0.8), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.4, 0.8, 1.1), BeamSignal(0, 0)));
        points.push_back(MeasurementPoint(Point(0.4, 0.8, 1.4), BeamSignal(9, 1)));

        CuveCube cube(QDateTime::currentDateTime(), QString("SSS"), configs, points, noise);

        repo.SaveCube(cube);
        CuveCube retr_cube = repo.GetCube(cube.timestamp());
        QCOMPARE(cube, retr_cube);
    }
    catch(std::exception& exc) {
        QFAIL("SaveAndGetCube failed");
    }
}

void TestQARepo::SaveAndGetCubeKeys() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        std::map<Axis, AxisConfig> configs1;
        configs1[Axis::X] = AxisConfig(2, 0.3, 0.1);
        configs1[Axis::Y] = AxisConfig(3, 0.4, 0.2);
        configs1[Axis::Z] = AxisConfig(4, 0.5, 0.3);
        std::vector<MeasurementPoint> points1;
        points1.push_back(MeasurementPoint(Point(0.3, 0.4, 0.5), BeamSignal(7, 1)));
        QDateTime timestamp1 = QDateTime::currentDateTime().addDays(-1);
        CuveCube cube1(timestamp1, QString("SSS1"), configs1, points1, 0.0);
        repo.SaveCube(cube1);

        std::map<Axis, AxisConfig> configs2;
        configs2[Axis::X] = AxisConfig(3, 0.3, 0.1);
        configs2[Axis::Y] = AxisConfig(4, 0.4, 0.2);
        configs2[Axis::Z] = AxisConfig(5, 0.5, 0.3);
        std::vector<MeasurementPoint> points2;
        points2.push_back(MeasurementPoint(Point(0.3, 0.4, 0.5), BeamSignal(8, 1)));
        QDateTime timestamp2 = QDateTime::currentDateTime();
        CuveCube cube2(timestamp2, QString("SSS2"), configs2, points2, 0.0);
        repo.SaveCube(cube2);

        std::map<Axis, AxisConfig> configs3;
        configs3[Axis::X] = AxisConfig(4, 0.3, 0.1);
        configs3[Axis::Y] = AxisConfig(5, 0.4, 0.2);
        configs3[Axis::Z] = AxisConfig(6, 0.5, 0.3);
        std::vector<MeasurementPoint> points3;
        points3.push_back(MeasurementPoint(Point(0.3, 0.4, 0.5), BeamSignal(9, 1)));
        QDateTime timestamp3 = QDateTime::currentDateTime();
        CuveCube cube3(timestamp3, QString("SSS3"), configs3, points3, 0.0);
        repo.SaveCube(cube3);

        std::map<QDateTime, QString> keys = repo.GetCubeKeys();
        QCOMPARE(keys.at(timestamp1), QString("SSS1"));
        QCOMPARE(keys.at(timestamp2), QString("SSS2"));
        QCOMPARE(keys.at(timestamp3), QString("SSS3"));
        QCOMPARE((int)keys.size(), 3);

    }
    catch(std::exception& exc) {
        QFAIL("GetCubeKeys failed");
    }
}

void TestQARepo::GetBraggPeakSeries() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto b1 = GetBraggPeak(QDateTime(QDate(2017, 10, 1)));
        auto b2 = GetBraggPeak(QDateTime(QDate(2017, 10, 2), QTime(1, 1)));
        auto b3 = GetBraggPeak(QDateTime(QDate(2017, 10, 2)));
        auto b4 = GetBraggPeak(QDateTime(QDate(2017, 10, 3)));
        auto b5 = GetBraggPeak(QDateTime(QDate(2017, 10, 4)));

        repo.SaveBraggPeak(b1);
        repo.SaveBraggPeak(b2);
        repo.SaveBraggPeak(b3);
        repo.SaveBraggPeak(b4);
        repo.SaveBraggPeak(b5);

        BraggPeakSeries s1 = repo.GetBraggPeakSeries(QDate(2017, 10, 1), QDate(2017, 10, 1));
        QCOMPARE(s1.N(), 1);

        BraggPeakSeries s2 = repo.GetBraggPeakSeries(QDate(2017, 10, 1), QDate(2017, 10, 5));
        QCOMPARE(s2.N(), 5);

        BraggPeakSeries s3 = repo.GetBraggPeakSeries(QDate(2017, 10, 1), QDate(2017, 10, 2));
        QCOMPARE(s3.N(), 3);
    }
    catch (std::exception& exc) {
        qDebug() << "GetBraggPeakSeries Exception thrown: " << exc.what();
        QFAIL("GetBraggPeakSeries exception thrown in test");
    }
}


void TestQARepo::GetSOBPSeries() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        auto s1 = GetSOBP(10000, QDateTime(QDate(2017, 10, 1)));
        auto s2 = GetSOBP(10000, QDateTime(QDate(2017, 10, 2), QTime(1, 1)));
        auto s3 = GetSOBP(10000, QDateTime(QDate(2017, 10, 2)));
        auto s4 = GetSOBP(10000, QDateTime(QDate(2017, 10, 3)));
        auto s5 = GetSOBP(10000, QDateTime(QDate(2017, 10, 4)));

        repo.SaveSOBP(s1);
        repo.SaveSOBP(s2);
        repo.SaveSOBP(s3);
        repo.SaveSOBP(s4);
        repo.SaveSOBP(s5);

        SOBPSeries serie1 = repo.GetSOBPSeries(123, QDate(2017, 10, 1), QDate(2017, 10, 1));
        QCOMPARE(serie1.N(), 1);

        SOBPSeries serie2 = repo.GetSOBPSeries(123, QDate(2017, 10, 1), QDate(2017, 10, 5));
        QCOMPARE(serie2.N(), 5);

        SOBPSeries serie3 = repo.GetSOBPSeries(123, QDate(2017, 10, 1), QDate(2017, 10, 2));
        QCOMPARE(serie3.N(), 3);
    }
    catch (std::exception& exc) {
        qDebug() << "GetSOBPSeries Exception thrown: " << exc.what();
        QFAIL("GetSOBPSeries exception thrown in test");
    }
}

void TestQARepo::GetLastBraggPeak() {
    try {
        QARepo repo("unittest", dbHostName_, dbPort_,
                          dbDatabaseName_, dbUserName_,
                          dbPassword_);
        repo.Connect();

        QDateTime now = QDateTime::currentDateTime();
        auto b1 = GetBraggPeak(now.addSecs(-200));
        auto b2 = GetBraggPeak(now);
        auto b3 = GetBraggPeak(now.addDays(-1));

        repo.SaveBraggPeak(b1);
        repo.SaveBraggPeak(b2);
        repo.SaveBraggPeak(b3);

        auto b = repo.GetLastBraggPeak();
        QCOMPARE(b, b2);

     }
    catch (std::exception& exc) {
        qDebug() << "GetLastBraggPeak Exception thrown: " << exc.what();
        QFAIL("GetLastBraggPeak exception thrown in test");
    }
}
