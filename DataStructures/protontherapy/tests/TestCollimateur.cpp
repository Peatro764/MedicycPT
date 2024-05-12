#include "TestCollimateur.h"
#include "Collimator.h"

using namespace QTest;

void TestCollimateur::Constructors() {
    std::vector<Coordinate> coords {Coordinate(1,2), Coordinate(2,3)};
    Collimator coll1(10000, Patient(1, "first", "last"), coords);
    QCOMPARE(coll1.Dossier(), 10000);
    QCOMPARE(coll1.patient(), Patient(1, "first", "last"));
    QCOMPARE(coll1.InternalEdges(), coords);

    Collimator coll2(10000, Patient(1, "first", "last"), "((1,2),(2,3))");
    QCOMPARE(coll2.Dossier(), 10000);
    QCOMPARE(coll2.patient(), Patient(1, "first", "last"));
    QCOMPARE(coll2.InternalEdges(), coords);
}

void TestCollimateur::InternalEdgesPathFormat() {
    std::vector<Coordinate> coords;
    Patient patient(1, "first", "last");
    Collimator coll1(10000, patient, coords);
    QCOMPARE(coll1.InternalEdgesPathFormat(), QString("()"));

    coords.push_back(Coordinate(1,2));
    Collimator coll2(10000, patient, coords);
    QCOMPARE(coll2.InternalEdgesPathFormat(), QString("((1.0000,2.0000))"));

    coords.push_back(Coordinate(2,3));
    Collimator coll3(10000, patient, coords);
    QCOMPARE(coll3.InternalEdgesPathFormat(), QString("((1.0000,2.0000),(2.0000,3.0000))"));

    QCOMPARE(Collimator(10000, patient, QString("()")).InternalEdgesPathFormat(), QString("()"));
    QCOMPARE(Collimator(10000, patient, QString("")).InternalEdgesPathFormat(), QString("()"));
    QCOMPARE(Collimator(10000, patient, QString("(q,b)")).InternalEdgesPathFormat(), QString("()"));
    QCOMPARE(Collimator(10000, patient, QString("(1,2)")).InternalEdgesPathFormat(), QString("((1.0000,2.0000))"));
    QCOMPARE(Collimator(10000, patient, QString("((1,2))")).InternalEdgesPathFormat(), QString("((1.0000,2.0000))"));
    QCOMPARE(Collimator(10000, patient, QString("((1,2),(3,4))")).InternalEdgesPathFormat(), QString("((1.0000,2.0000),(3.0000,4.0000))"));
    QCOMPARE(Collimator(10000, patient, QString("((1,2),(c,4))")).InternalEdgesPathFormat(), QString("()"));
}

void TestCollimateur::SetDossier() {
    std::vector<Coordinate> coords;
    Patient patient(1, "first", "last");
    Collimator coll1(10000, patient, coords);
    QCOMPARE(coll1.Dossier(), 10000);
    coll1.SetDossier(20000);
    QCOMPARE(coll1.Dossier(), 20000);
}

void TestCollimateur::SetLastName() {
    std::vector<Coordinate> coords;
    Patient patient(1, "first", "last");
    Collimator coll1(10000, patient, coords);
    QCOMPARE(coll1.patient().GetLastName(), QString("last"));
    coll1.SetLastName("last2");
    QCOMPARE(coll1.patient().GetLastName(), QString("last2"));
}

void TestCollimateur::SetFirstName() {
    std::vector<Coordinate> coords;
    Patient patient(1, "first", "last");
    Collimator coll1(10000, patient, coords);
    QCOMPARE(coll1.patient().GetFirstName(), QString("first"));
    coll1.SetFirstName("first2");
    QCOMPARE(coll1.patient().GetFirstName(), QString("first2"));
}

void TestCollimateur::IsValid() {
    const std::vector<Coordinate> coordsOk { Coordinate(1,2) };
    const Patient patientOk(1, "first", "last");
    const int dossierOk(10000);

    const std::vector<Coordinate> coordsNotOk;
    const Patient patientNotOk;
    const int dossierNotOk(0);

    QCOMPARE(Collimator(dossierOk, patientOk, coordsOk).IsValid(), true);
    QCOMPARE(Collimator(dossierNotOk, patientOk, coordsOk).IsValid(), false);
    QCOMPARE(Collimator(dossierOk, patientNotOk, coordsOk).IsValid(), false);
    QCOMPARE(Collimator(dossierOk, patientOk, coordsNotOk).IsValid(), false);
}

void TestCollimateur::ReadEyePlanFile() {
    QFile file(":/data/EyePlanCollimator.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QFAIL("Failed open eyeplan file");
    }

    QTextStream stream(&file);
    Collimator collimator(stream);

    QCOMPARE(collimator.Dossier(), 19225);
    QCOMPARE(collimator.patient().GetFirstName(), QString("Bruno"));
    QCOMPARE(collimator.patient().GetLastName(), QString("MINI"));

    QCOMPARE(collimator.InternalEdgesPathFormat(),
             QString("((-4.6350,-0.1232),(-4.5947,0.8267),(-3.6459,4.0133),(-2.0478,5.5437),(-1.1067,5.9630),(0.0120,5.7953),(1.6989,5.0406),(3.2793,3.6569),(4.3980,1.8330),(5.0550,-0.4311),(4.9307,-2.2550),(4.1317,-4.1628),(2.3915,-5.7142),(-0.1833,-6.1544),(-2.0478,-5.5255),(-3.6282,-4.2676),(-4.2319,-3.3452),(-4.6581,-3.0098),(-4.9955,-2.1083),(-4.6350,-0.1232))"));
}
