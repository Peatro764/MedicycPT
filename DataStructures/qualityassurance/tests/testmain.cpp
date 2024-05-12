#include <QtTest>
#include "TestBeamMeasurement.h"
#include "TestBeamProfile.h"
#include "TestCuveCube.h"
#include "TestBraggPeak.h"
#include "TestSOBP.h"
#include "TestDepthDoseMeasurement.h"
#include "TestMeasurementPoint.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    TestBeamMeasurement testBeamMeasurement;
    TestBeamProfile testBeamProfile;
    TestCuveCube testCuveCube;
    TestBraggPeak testBraggPeak;
    TestSOBP testSOBP;
    TestMeasurementPoint testMeasurementPoint;
    TestDepthDoseMeasurement testDepthDoseMeasurement;

    return  QTest::qExec(&testBeamProfile, argc, argv) |
            QTest::qExec(&testBeamMeasurement, argc, argv) |
            QTest::qExec(&testCuveCube, argc, argv) |
            QTest::qExec(&testBraggPeak, argc, argv) |
            QTest::qExec(&testMeasurementPoint, argc, argv) |
            QTest::qExec(&testSOBP, argc, argv) |
            QTest::qExec(&testMeasurementPoint, argc, argv) |
            QTest::qExec(&testDepthDoseMeasurement, argc, argv);
}
