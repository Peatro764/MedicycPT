#include "TestCompensateur.h"
#include "Compensateur.h"
#include "Calc.h"
#include "Material.h"

using namespace QTest;

void TestCompensateur::Constructor() {
    try {
        // Standard case
        int dossier(123);
        double diameter(22.0);
        double sclere_thickness(2.0);
        Eye eye(diameter, sclere_thickness);
        QCPCurveData center_point(0, 2, 3);
        double bolus_thickness(1.22);
        double parcours_error(0.5);
        double parcours(22.0);
        double base_thickness(1.0);
        double error_scaling(1.0);

        Compensateur comp1(dossier, eye, center_point, bolus_thickness, parcours,
                           parcours_error, error_scaling, base_thickness);
        QCOMPARE(dossier, comp1.dossier());
        QCOMPARE(eye, comp1.eye());
        QCOMPARE(center_point.key, comp1.center_point().key);
        QCOMPARE(center_point.value, comp1.center_point().value);
        QCOMPARE(parcours, comp1.parcours());
        QCOMPARE(parcours_error, comp1.parcours_error());
        QCOMPARE(base_thickness, comp1.base_thickness());

        // Minimal parcours given
        double minimal_parcours(bolus_thickness + sclere_thickness + 0.05);
        Compensateur(dossier, eye, center_point, bolus_thickness, minimal_parcours,
                     parcours_error, error_scaling, base_thickness);

        // Parcours to small
        try {
            double toosmall_parcours(bolus_thickness + sclere_thickness - 0.05);
            Compensateur(dossier, eye, center_point, bolus_thickness, toosmall_parcours,
                         parcours_error, error_scaling, base_thickness);
            QFAIL("TestCompensateur::Constructor Should have thrown 1");
        }
        catch (...) {}
    }
    catch (std::exception& exc) {
        QString msg("TestCompensateur::Constructor Exception thrown in test: " + QString(exc.what()));
        QFAIL(msg.toStdString().c_str());
    }
}

void TestCompensateur::Comparisons() {
    try {
        int dossier(123);
        double diameter(22.0);
        double sclere_thickness(2.0);
        Eye eye(diameter, sclere_thickness);
        QCPCurveData center_point(0, 2, 3);
        double bolus_thickness(1.22);
        double parcours_error(3.33);
        double error_scaling(1.0);
        double parcours(22.22);
        double base_thickness(1.0);

        Compensateur comp1(dossier, eye, center_point, bolus_thickness, parcours,
                           parcours_error, error_scaling, base_thickness);
        Compensateur comp2(dossier, eye, center_point, bolus_thickness + 1.0, parcours,
                           parcours_error, error_scaling, base_thickness);

        QCOMPARE(comp1, comp1);
        QVERIFY(comp1 != comp2);
    }
    catch (std::exception& exc) {
        QString msg("TestCompensateur::Comparisons Exception thrown in test: " + QString(exc.what()));
        QFAIL(msg.toStdString().c_str());
    }
}

void TestCompensateur::CorrectBaseWidth() {
    try {
        int dossier(123);
        double diameter(22.0);
        double sclere_thickness(2.0);
        Eye eye(diameter, sclere_thickness);
        QCPCurveData center_point(0, 2, 3);
        double bolus_thickness(1.22);
        double parcours_error(1.0);
        double error_scaling(1.0);
        double parcours(22.22);
        double base_thickness(1.0);

        Compensateur comp1(dossier, eye, center_point, bolus_thickness, parcours,
                           parcours_error, error_scaling, base_thickness);
        QCPCurveDataContainer theo_curve(comp1.HalfTheoCurve());
        QVERIFY(calc::AlmostEqual(theo_curve.at(theo_curve.size() - 1)->mainKey(), diameter / 2.0 - sclere_thickness, 0.001));
    }
    catch (std::exception& exc) {
        QString msg("TestCompensateur::CorrectBaseWidth Exception thrown in test: " + QString(exc.what()));
        QFAIL(msg.toStdString().c_str());
    }
}

void TestCompensateur::CorrectPlexiglasColumnHeight() {
    try {
        int dossier(123);
        double diameter(22.0);
        double sclere_thickness(2.0);
        Eye eye(diameter, sclere_thickness);
        QCPCurveData center_point(0, 2, 3);
        double bolus_thickness(1.22);
        double parcours_error(1.0);
        double error_scaling(1.0);
        double parcours(22.22);
        double base_thickness(1.0);

        Compensateur comp1(dossier, eye, center_point, bolus_thickness, parcours,
                           parcours_error, error_scaling, base_thickness);
        QCPCurveDataContainer theo_curve(comp1.HalfTheoCurve());
        QCPCurveDataContainer step_curve(comp1.HalfStepCurve());
        QCPCurveDataContainer fraiseuse_curve(comp1.FraiseuseFormat());

        const double plexiglas_height(material::Tissue2Plexiglas(parcours - bolus_thickness - sclere_thickness));
        QVERIFY(calc::AlmostEqual(theo_curve.at(0)->mainValue(), plexiglas_height, 0.001));
        QVERIFY(calc::AlmostEqual(step_curve.at(0)->mainValue(), plexiglas_height, 0.001));
        QVERIFY(calc::AlmostEqual(fraiseuse_curve.at(0)->mainValue(), plexiglas_height, 0.001));
        QVERIFY(calc::AlmostEqual(comp1.mm_plexiglas(0.0), plexiglas_height, 0.001));
    }
    catch (std::exception& exc) {
        QString msg("TestCompensateur::CorrectPlexiglasColumnHeight Exception thrown in test: " + QString(exc.what()));
        QFAIL(msg.toStdString().c_str());
    }
}

void TestCompensateur::CorrectTheoreticalCurveHeight() {
    try {
        int dossier(123);
        double diameter(22.0);
        double sclere_thickness(2.0);
        Eye eye(diameter, sclere_thickness);
        QCPCurveData center_point(0, 2, 3);
        double bolus_thickness(1.22);
        double parcours_error(1.0);
        double error_scaling(1.0);
        double parcours(22.22);
        double base_thickness(1.0);

        Compensateur comp1(dossier, eye, center_point, bolus_thickness, parcours,
                           parcours_error, error_scaling, base_thickness);
        QCPCurveDataContainer theo_curve(comp1.HalfTheoCurve());

        for (auto it = theo_curve.constBegin(); it < (theo_curve.constEnd() - 1); ++it) {
            double mm_tissue(bolus_thickness + sclere_thickness + eye.sclere_radius() - std::sqrt(std::pow(eye.sclere_radius(), 2.0) - std::pow(it->mainKey(), 2.0)));
            double mm_plexiglas(material::Tissue2Plexiglas(parcours - mm_tissue));
            QVERIFY(calc::AlmostEqual(it->mainValue(), mm_plexiglas, 0.001));
        }
    }
    catch (std::exception& exc) {
        QString msg("TestCompensateur::CorrectTheoreticalCurveHeight Exception thrown in test: " + QString(exc.what()));
        QFAIL(msg.toStdString().c_str());
    }
}

void TestCompensateur::StepCurveWithinError() {
    try {
        int dossier(123);
        double diameter(22.0);
        double sclere_thickness(2.0);
        Eye eye(diameter, sclere_thickness);
        QCPCurveData center_point(0, 2, 3);
        double bolus_thickness(1.22);
        double parcours_error(0.3);
        double error_scaling(1.0);
        double parcours(22.22);
        double base_thickness(0.01);

        // normal case
        Compensateur comp1(dossier, eye, center_point, bolus_thickness, parcours,
                           parcours_error, error_scaling, base_thickness);
        QCPCurveDataContainer theo_curve1(comp1.HalfTheoCurve());
        for (auto it = theo_curve1.constBegin(); it < (theo_curve1.constEnd() - 1); ++it) {
            QVERIFY(std::fabs(it->mainValue() - comp1.mm_plexiglas(it->mainKey())) < parcours_error);
        }

        // minimal parcours error
        double minimal_parcours_error(0.1);
        Compensateur comp2(dossier, eye, center_point, bolus_thickness, parcours,
                           minimal_parcours_error, error_scaling, base_thickness);
        QCPCurveDataContainer theo_curve2(comp2.HalfTheoCurve());
        for (auto it = theo_curve2.constBegin(); it < (theo_curve2.constEnd() - 1); ++it) {
            QVERIFY(std::fabs(it->mainValue() - comp2.mm_plexiglas(it->mainKey())) < minimal_parcours_error);
        }

        // minimal parcours
         double minimal_parcours(bolus_thickness + sclere_thickness + eye.sclere_radius() + 0.1);
         Compensateur comp3(dossier, eye, center_point, bolus_thickness,
                            minimal_parcours, parcours_error, error_scaling, base_thickness);
         QCPCurveDataContainer theo_curve3(comp3.HalfTheoCurve());
         for (auto it = theo_curve3.constBegin(); it < (theo_curve3.constEnd() - 1); ++it) {
                 QVERIFY(std::fabs(it->mainValue() - comp3.mm_plexiglas(it->mainKey())) <= parcours_error);
         }
    }
    catch (std::exception& exc) {
        QString msg("TestCompensateur::StepCurveWithinError Exception thrown in test: " + QString(exc.what()));
        QFAIL(msg.toStdString().c_str());
    }
}

void TestCompensateur::FraiseuseFormat() {
    try {
        int dossier(123);
        double diameter(22.0);
        double sclere_thickness(2.0);
        Eye eye(diameter, sclere_thickness);
        QCPCurveData center_point(0, 2, 3);
        double bolus_thickness(1.22);
        double parcours_error(0.3);
        double error_scaling(1.0);
        double parcours(22.22);
        double base_thickness(1.0);

        Compensateur comp1(dossier, eye, center_point, bolus_thickness, parcours,
                           parcours_error, error_scaling, base_thickness);
        QCPCurveDataContainer step_curve1(comp1.HalfStepCurve());
        QCPCurveDataContainer fraiseuse_curve1(comp1.FraiseuseFormat());
        for (int idx = 0; idx < fraiseuse_curve1.size(); ++idx) {
            QVERIFY(calc::AlmostEqual(fraiseuse_curve1.at(idx)->mainKey(), step_curve1.at(idx * 2)->mainKey(), 0.001));
            QVERIFY(calc::AlmostEqual(fraiseuse_curve1.at(idx)->mainValue(), step_curve1.at(idx * 2)->mainValue(), 0.001));
        }
    }
    catch (std::exception& exc) {
        QString msg("TestCompensateur::StepCurveWithinError Exception thrown in test: " + QString(exc.what()));
        QFAIL(msg.toStdString().c_str());
    }
}
