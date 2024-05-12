#include "TestTreatmentType.h"

#include "TreatmentType.h"
#include "Calc.h"

void TestTreatmentType::initTestCase() {}

void TestTreatmentType::cleanupTestCase() {}

void TestTreatmentType::init() {}

void TestTreatmentType::cleanup() {}

void TestTreatmentType::Comparisons() {
    std::vector<double> v1;
    std::vector<double> v2 {1.0, 4.0 };
    std::vector<double> v3 {2.0, 4.0, 5.0 };

    TreatmentType t1("ttt", v2, v1);
    TreatmentType t2("tt", v2, v1);
    TreatmentType t3("ttt", v3, v1);
    TreatmentType t4("ttt", v2, v3);

    QCOMPARE(t1, t1);
    QVERIFY(t1 != t2);
    QVERIFY(t3 != t2);
    QVERIFY(t3 != t4);
}

void TestTreatmentType::Constructor() {
    std::vector<double> v1;
    std::vector<double> v2 {1.0, 4.0 };
    std::vector<double> v3 {2.0, 4.0, 5.0 };

    TreatmentType t1("ttt", v2, v1);
    QCOMPARE(t1.Name(), QString("ttt"));
}

void TestTreatmentType::SeanceDoses() {
    std::vector<double> v1;
    std::vector<double> v2 {1.0, 4.0 };
    std::vector<double> v3 {2.0, 4.0, 5.0 };

    TreatmentType t1("ttt", v2, v1);
    TreatmentType t2("ttt", v3, v2);

    std::vector<double> exp1 { 1.0, 4.0 };
    std::vector<double> exp2 { 2.0, 4.0, 5.0, 1.0, 4.0 };

    QCOMPARE(t1.SeanceDoses(), exp1);
    QCOMPARE(t2.SeanceDoses(), exp2);
}

void TestTreatmentType::Dose() {
    std::vector<double> v1;
    std::vector<double> v2 {1.0, 4.0 };
    std::vector<double> v3 {2.0, 4.0, 5.0 };

    TreatmentType t1("ttt", v2, v1);
    TreatmentType t2("ttt", v3, v2);

    QVERIFY(calc::AlmostEqual(t1.TotalDose(), 5.0, 0.0001));
    QVERIFY(calc::AlmostEqual(t1.StandardDose(), 5.0, 0.0001));
    QVERIFY(calc::AlmostEqual(t1.BoostDose(), 0.0, 0.0001));

    QVERIFY(calc::AlmostEqual(t2.TotalDose(), 16.0, 0.0001));
    QVERIFY(calc::AlmostEqual(t2.StandardDose(), 11.0, 0.0001));
    QVERIFY(calc::AlmostEqual(t2.BoostDose(), 5.0, 0.0001));
}

void TestTreatmentType::DoseEBR() {
    std::vector<double> v1;
    std::vector<double> v2 {1.0, 4.0 };
    std::vector<double> v3 {2.0, 4.0, 5.0 };

    TreatmentType t1("ttt", v2, v1);
    TreatmentType t2("ttt", v3, v2);

    QVERIFY(calc::AlmostEqual(t1.TotalDoseEBR(), 5.0 * 1.1, 0.0001));
    QVERIFY(calc::AlmostEqual(t1.StandardDoseEBR(), 5.0 * 1.1, 0.0001));
    QVERIFY(calc::AlmostEqual(t1.BoostDoseEBR(), 0.0, 0.0001));

    QVERIFY(calc::AlmostEqual(t2.TotalDoseEBR(), 16.0 * 1.1, 0.0001));
    QVERIFY(calc::AlmostEqual(t2.StandardDoseEBR(), 11.0 * 1.1, 0.0001));
    QVERIFY(calc::AlmostEqual(t2.BoostDoseEBR(), 5.0 * 1.1, 0.0001));
}

void TestTreatmentType::AddSeance() {
    std::vector<double> v1 {1.0, 4.0 };
    std::vector<double> v2;
    TreatmentType t1("ttt", v1, v2);
    t1.AddStandard(3.0);

    std::vector<double> vExp { 1.0, 4.0, 3.0 };
    QCOMPARE(t1.SeanceDoses(), vExp);
}

void TestTreatmentType::AddBoost() {
    std::vector<double> v1 {1.0, 4.0 };
    std::vector<double> v2;
    TreatmentType t1("ttt", v1, v2);
    t1.AddBoost(3.0);
    t1.AddStandard(5.0);

    std::vector<double> vExp { 1.0, 4.0, 5.0, 3.0 };
    QCOMPARE(t1.SeanceDoses(), vExp);
}

void TestTreatmentType::StandardSummary() {
    std::vector<double> v1;
    std::vector<double> v2 {1.0, 4.0 };

    TreatmentType t1("ttt", v2, v1);
    QCOMPARE(t1.StandardSummary(), QString("(1.00, 4.00) Gy"));
}

void TestTreatmentType::BoostSummary() {
    std::vector<double> v1 {4.0, 2.0 };
    std::vector<double> v2 {1.0, 4.0 };

    TreatmentType t1("ttt", v1, v2);
    QCOMPARE(t1.BoostSummary(), QString("(1.00, 4.00) Gy"));
}

void TestTreatmentType::TotalSummary() {
    std::vector<double> v1 {4.0, 2.0 };
    std::vector<double> v2 {1.0, 4.0 };

    TreatmentType t1("ttt", v1, v2);
    QCOMPARE(t1.TotalSummary(), QString("11.00 Gy (12.10 GyEBR)"));
}


