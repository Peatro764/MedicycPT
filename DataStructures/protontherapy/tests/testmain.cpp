#include <QtTest>
#include "TestEye.h"
#include "TestCompensateur.h"
#include "TestDosimetry.h"
#include "TestDosimetryRecord.h"
#include "TestPatient.h"
#include "TestSeance.h"
#include "TestSeanceRecord.h"
#include "TestTreatment.h"
#include "TestUtil.h"
#include "TestDegradeur.h"
#include "TestModulateur.h"
#include "TestCollimateur.h"
#include "TestDepthDose.h"
#include "TestDepthDoseCurve.h"
#include "TestSOBPCurve.h"
#include "TestSOBPMaker.h"
#include "TestModulateurMaker.h"
#include "TestColorHandler.h"
#include "TestAlgorithms.h"
#include "TestTreatmentType.h"
#include "TestSeanceConfig.h"
#include "TestSeancePacket.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    TestEye testEye;
    TestCompensateur testCompensateur;
    TestSeance testSeance;
    TestSeanceRecord testSeanceRecord;
    TestDosimetry testDosimetry;
    TestDosimetryRecord testDosimetryRecord;
    TestTreatment testTreatment;
    TestUtil testUtil;
    TestDegradeur testDegradeur;
    TestModulateur testModulateur;
    TestCollimateur testCollimateur;
    TestDepthDose testDepthDose;
    TestDepthDoseCurve testDepthDoseCurve;
    TestSOBPCurve testSOBPCurve;
    TestSOBPMaker testSOBPMaker;
    TestModulateurMaker testModulateurMaker;
    TestColorHandler testColorHandler;
    TestAlgorithms testAlgorithms;
    TestTreatmentType testTreatmentType;
    TestSeancePacket testSeancePacket;
    TestSeanceConfig testSeanceConfig;

  return    QTest::qExec(&testEye, argc, argv) |
            QTest::qExec(&testCompensateur, argc, argv) |
            QTest::qExec(&testUtil, argc, argv) |
            QTest::qExec(&testDosimetry, argc, argv) |
            QTest::qExec(&testDosimetryRecord, argc, argv) |
            QTest::qExec(&testSeance, argc, argv) |
            QTest::qExec(&testSeanceRecord, argc, argv) |
            QTest::qExec(&testTreatment, argc, argv) |
            QTest::qExec(&testDegradeur, argc, argv) |
            QTest::qExec(&testModulateur, argc, argv) |
            QTest::qExec(&testCollimateur, argc, argv) |
            QTest::qExec(&testDepthDose, argc, argv) |
            QTest::qExec(&testDepthDoseCurve, argc, argv) |
            QTest::qExec(&testSOBPCurve, argc, argv) |
            QTest::qExec(&testSOBPMaker, argc, argv) |
            QTest::qExec(&testModulateurMaker, argc, argv) |
            QTest::qExec(&testColorHandler, argc, argv) |
            QTest::qExec(&testAlgorithms, argc, argv) |
            QTest::qExec(&testTreatmentType, argc, argv) |
            QTest::qExec(&testSeanceConfig, argc, argv) |
            QTest::qExec(&testSeancePacket, argc, argv);

}
