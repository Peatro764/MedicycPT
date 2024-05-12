#include "TestAlgorithms.h"

#include <iostream>
#include <QDateTime>

#include "Algorithms.h"
#include "Calc.h"

void TestAlgorithms::initTestCase() {
}

void TestAlgorithms::cleanupTestCase() {}

void TestAlgorithms::init() {}

void TestAlgorithms::cleanup() {}

void TestAlgorithms::TestCalculateIntegratedDose() {
    const double charge(4.786e-10);
    const double Debit2H_alpha(2.316e-7);
    const double Debit2H_beta(-1.277);
    const double svPerCo(7474.54);
    const double debit(0.87397);

    const double dose = algorithms::CalculateIntegratedDose(Debit2H_alpha, Debit2H_beta, svPerCo, charge, debit);
    QVERIFY(calc::AlmostEqual(dose, 13.00525, 0.0001));
}

void TestAlgorithms::TestCalculateDebit() {
    const double tdj(1.33114);
    const double ref(1.37);
    const double charge(7.85e-10);
    const double H2Debit_alpha(5.63204e-9);
    const double H2Debit_beta(-0.787305);
    const double dose(13.0);

    const double debit = algorithms::CalculateDebit(H2Debit_alpha, H2Debit_beta, charge, dose, ref, tdj);
    QVERIFY(calc::AlmostEqual(debit, 0.625451, 0.0001)); //0.61144123983146426);
}

void TestAlgorithms::TestCalculateHoverD() {
    const double tdj(1.33114);
    const double ref(1.37);
    const double charge(7.85e-10);
    const double dose(13.0);

    const double hoverd = algorithms::CalculateHoverD(charge, dose, ref, tdj);
    QCOMPARE(hoverd, 6.2147424821523717e-11);
}
