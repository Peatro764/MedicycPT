#include "TestPatient.h"
#include "Patient.h"

using namespace QTest;

void TestPatient::Getters() {
    Patient patient(33, "petter", "hofverberg");
    QCOMPARE(patient.GetId(), 33);
    QCOMPARE(patient.GetFirstName(), QString("petter"));
    QCOMPARE(patient.GetLastName(), QString("hofverberg"));
}
