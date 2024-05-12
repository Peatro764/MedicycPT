#include "TestSOBPMaker.h"

#include "SOBPMaker.h"
#include "Calc.h"

#include <vector>

void TestSOBPMaker::initTestCase() {}

void TestSOBPMaker::cleanupTestCase() {}

void TestSOBPMaker::init() {}

void TestSOBPMaker::cleanup() {}

void TestSOBPMaker::GetSOBP_NoInterpolation_NoDegradeur() {
    try {
        std::vector<DepthDose> depth_doses1 {DepthDose(1, 10.0), DepthDose(2, 20.0), DepthDose(3, 30.0), DepthDose(4, 40.0)};
        std::vector<DepthDose> depth_doses2 {DepthDose(1, 100.0), DepthDose(2, 200.0), DepthDose(3, 300.0), DepthDose(4, 400.0)};
        std::vector<DepthDose> depth_doses3 {DepthDose(1, 1000.0), DepthDose(2, 2000.0), DepthDose(3, 3000.0), DepthDose(4, 4000.0)};
        DepthDoseCurve depth_dose_curve1(depth_doses1);
        DepthDoseCurve depth_dose_curve2(depth_doses2);
        DepthDoseCurve depth_dose_curve3(depth_doses3);
        std::vector<DepthDoseCurve> depth_dose_curves {depth_dose_curve1, depth_dose_curve2, depth_dose_curve3};
        std::vector<double> monitor_units {100, 200, 300};
        std::vector<int> um_plexi {0, 800, 1600};
        std::vector<double> weights {1.0, 0.1, 0.01};

        SOBPCurve exp_sobp_curve(depth_dose_curves, monitor_units, um_plexi, weights, false);

        const int um_mod_deg(0);
        std::map<int, double> um_plexi_weight;
        for (int idx = 0; idx < (int)weights.size(); ++idx) {
            um_plexi_weight[um_plexi[idx]] = weights[idx];
        }
        Modulateur mod(QString("1"), 1.0, 1.0, 1.0, um_mod_deg, um_plexi_weight, true);
        DegradeurSet deg_set(Degradeur(QString("1"), 0, true));

        SOBPMaker maker(depth_dose_curves, monitor_units, um_plexi);
        SOBPCurve sobp_curve(maker.GetSOBP(mod, deg_set));
        QCOMPARE(sobp_curve, exp_sobp_curve);
    }
    catch (std::exception& exc) {
        qWarning() << "TestSOBPMaker::GetSOBPWithInterpolation_NoDegradeur " + QString(exc.what());
        QFAIL("TestSOBPMaker::GetSOBP_WithInterpolation_NoDegradeur Exception thrown in test");
    }
}

void TestSOBPMaker::GetSOBP_NoInterpolation_ExternalDegradeur() {
    try {
        std::vector<DepthDose> depth_doses1 {DepthDose(1, 10.0), DepthDose(2, 20.0), DepthDose(3, 30.0), DepthDose(4, 40.0)};
        std::vector<DepthDose> depth_doses2 {DepthDose(1, 100.0), DepthDose(2, 200.0), DepthDose(3, 300.0), DepthDose(4, 400.0)};
        std::vector<DepthDose> depth_doses3 {DepthDose(1, 1000.0), DepthDose(2, 2000.0), DepthDose(3, 3000.0), DepthDose(4, 4000.0)};
        DepthDoseCurve depth_dose_curve1(depth_doses1);
        DepthDoseCurve depth_dose_curve2(depth_doses2);
        DepthDoseCurve depth_dose_curve3(depth_doses3);
        std::vector<DepthDoseCurve> depth_dose_curves {depth_dose_curve1, depth_dose_curve2, depth_dose_curve3};
        std::vector<double> monitor_units {100, 200, 300};
        std::vector<int> um_plexi {800, 1600, 2400};
        std::vector<int> um_mod_plexi {0, 800, 1600};
        std::vector<double> weights {1.0, 0.1, 0.01};

        SOBPCurve exp_sobp_curve(depth_dose_curves, monitor_units, um_plexi, weights, false);

        const int um_mod_deg(0);
        std::map<int, double> um_plexi_weight;
        for (int idx = 0; idx < (int)weights.size(); ++idx) {
            um_plexi_weight[um_mod_plexi[idx]] = weights[idx];
        }
        Modulateur mod(QString("1"), 1.0, 1.0, 1.0, um_mod_deg, um_plexi_weight, true);
        DegradeurSet deg_set(Degradeur(QString("1"), 800, true));

        SOBPMaker maker(depth_dose_curves, monitor_units, um_plexi);
        SOBPCurve sobp_curve(maker.GetSOBP(mod, deg_set));
        QCOMPARE(sobp_curve, exp_sobp_curve);
    }
    catch (std::exception& exc) {
        qWarning() << "TestSOBPMaker::GetSOBPWithInterpolation_ExternalDegradeur " + QString(exc.what());
        QFAIL("TestSOBPMaker::GetSOBP_WithInterpolation_ExternalDegradeur Exception thrown in test");
    }
}

void TestSOBPMaker::GetSOBP_NoInterpolation_InternalPlusExternalDegradeur() {
    try {
        std::vector<DepthDose> depth_doses1 {DepthDose(1, 10.0), DepthDose(2, 20.0), DepthDose(3, 30.0), DepthDose(4, 40.0)};
        std::vector<DepthDose> depth_doses2 {DepthDose(1, 100.0), DepthDose(2, 200.0), DepthDose(3, 300.0), DepthDose(4, 400.0)};
        std::vector<DepthDose> depth_doses3 {DepthDose(1, 1000.0), DepthDose(2, 2000.0), DepthDose(3, 3000.0), DepthDose(4, 4000.0)};
        DepthDoseCurve depth_dose_curve1(depth_doses1);
        DepthDoseCurve depth_dose_curve2(depth_doses2);
        DepthDoseCurve depth_dose_curve3(depth_doses3);
        std::vector<DepthDoseCurve> depth_dose_curves {depth_dose_curve1, depth_dose_curve2, depth_dose_curve3};
        std::vector<double> monitor_units {100, 200, 300};
        std::vector<int> um_plexi {1000, 1800, 2600};
        std::vector<int> um_mod_plexi {0, 800, 1600};
        std::vector<double> weights {1.0, 0.1, 0.01};

        SOBPCurve exp_sobp_curve(depth_dose_curves, monitor_units, um_plexi, weights, false);

        const int um_mod_deg(200);
        std::map<int, double> um_plexi_weight;
        for (int idx = 0; idx < (int)weights.size(); ++idx) {
            um_plexi_weight[um_mod_plexi[idx]] = weights[idx];
        }
        Modulateur mod(QString("1"), 1.0, 1.0, 1.0, um_mod_deg, um_plexi_weight, true);
        DegradeurSet deg_set(Degradeur(QString("1"), 800, true));

        SOBPMaker maker(depth_dose_curves, monitor_units, um_plexi);
        SOBPCurve sobp_curve(maker.GetSOBP(mod, deg_set));
        QCOMPARE(sobp_curve, exp_sobp_curve);
    }
    catch (std::exception& exc) {
        qWarning() << "TestSOBPMaker::GetSOBPWithInterpolation_InternalPlusExternalDegradeur " + QString(exc.what());
        QFAIL("TestSOBPMaker::GetSOBP_WithInterpolation_InternalPlusExternalDegradeur Exception thrown in test");
    }
}

void TestSOBPMaker::GetSOBP_WithInterpolation() {
    // Note: In this test a range shifter is attached to the modulator (plus a normal range shifter)
    try {
        std::vector<DepthDose> depth_doses1 {DepthDose(1, 10.0), DepthDose(2, 20.0), DepthDose(3, 30.0), DepthDose(4, 40.0)};
        std::vector<DepthDose> depth_doses2 {DepthDose(1, 20.0), DepthDose(2, 30.0), DepthDose(3, 40.0), DepthDose(4, 50.0)};
        std::vector<DepthDose> depth_doses3 {DepthDose(1, 30.0), DepthDose(2, 40.0), DepthDose(3, 50.0), DepthDose(4, 60.0)};
        DepthDoseCurve depth_dose_curve1(depth_doses1);
        DepthDoseCurve depth_dose_curve2(depth_doses2);
        DepthDoseCurve depth_dose_curve3(depth_doses3);
        std::vector<DepthDoseCurve> depth_dose_curves {depth_dose_curve1, depth_dose_curve2, depth_dose_curve3};
        std::vector<double> monitor_units {100.0, 200.0, 300.0};
        std::vector<int> um_plexi {800, 1600, 2400};

        std::vector<double> weights {1.0, 0.5};
        std::vector<DepthDose> exp_depth_doses1 {DepthDose(1, 15.0), DepthDose(2, 25.0), DepthDose(3, 35.0), DepthDose(4, 45.0)};
        std::vector<DepthDose> exp_depth_doses2 {DepthDose(1, 25.0), DepthDose(2, 35.0), DepthDose(3, 45.0), DepthDose(4, 55.0)};
        DepthDoseCurve exp_depth_dose_curve1(exp_depth_doses1);
        DepthDoseCurve exp_depth_dose_curve2(exp_depth_doses2);
        std::vector<DepthDoseCurve> exp_depth_dose_curves {exp_depth_dose_curve1, exp_depth_dose_curve2};
        std::vector<double> exp_monitor_units { 150.0, 250.0 };
        std::vector<int> exp_um_plexi {1200, 2000};
        SOBPCurve exp_sobp_curve(exp_depth_dose_curves, exp_monitor_units, exp_um_plexi, weights, false);

        const int um_mod_deg(1000);
        std::map<int, double> um_plexi_weight;
        um_plexi_weight[0] = weights[0];
        um_plexi_weight[800] = weights[1];
        Modulateur mod(QString("1"), 1.0, 1.0, 1.0, um_mod_deg, um_plexi_weight, true);
        DegradeurSet deg_set(Degradeur(QString("1"), 200, true));

        SOBPMaker maker(depth_dose_curves, monitor_units, um_plexi);
        SOBPCurve sobp_curve(maker.GetSOBP(mod, deg_set));
        QCOMPARE(sobp_curve, exp_sobp_curve);
    }
    catch (std::exception& exc) {
        qWarning() << "TestSOBPMaker::GetSOBPWithInterpolation " + QString(exc.what());
        QFAIL("TestSOBPMaker::GetSOBP_WithInterpolation Exception thrown in test");
    }
}

void TestSOBPMaker::GetSOBP_WithExtrapolation() {
    try {
        std::vector<DepthDose> depth_doses1 {DepthDose(1, 10.0), DepthDose(2, 20.0), DepthDose(3, 30.0), DepthDose(4, 40.0)};
        std::vector<DepthDose> depth_doses2 {DepthDose(1, 20.0), DepthDose(2, 30.0), DepthDose(3, 40.0), DepthDose(4, 50.0)};
        std::vector<DepthDose> depth_doses3 {DepthDose(1, 30.0), DepthDose(2, 40.0), DepthDose(3, 50.0), DepthDose(4, 60.0)};
        DepthDoseCurve depth_dose_curve1(depth_doses1);
        DepthDoseCurve depth_dose_curve2(depth_doses2);
        DepthDoseCurve depth_dose_curve3(depth_doses3);
        std::vector<DepthDoseCurve> depth_dose_curves {depth_dose_curve1, depth_dose_curve2, depth_dose_curve3};
        std::vector<double> monitor_units {100.0, 200.0, 300.0};
        std::vector<int> um_plexi {800, 1600, 2400};

        std::vector<double> weights {1.0, 0.5};
        std::vector<DepthDose> exp_depth_doses1 {DepthDose(1, 25.0), DepthDose(2, 35.0), DepthDose(3, 45.0), DepthDose(4, 55.0)};
        std::vector<DepthDose> exp_depth_doses2 {DepthDose(1, 30.0), DepthDose(2, 40.0), DepthDose(3, 50.0), DepthDose(4, 60.0)};
        DepthDoseCurve exp_depth_dose_curve1(exp_depth_doses1);
        DepthDoseCurve exp_depth_dose_curve2(exp_depth_doses2);
        std::vector<DepthDoseCurve> exp_depth_dose_curves {exp_depth_dose_curve1, exp_depth_dose_curve2};
        std::vector<double> exp_monitor_units { 250.0, 300.0 };
        std::vector<int> exp_um_plexi {2000, 2800};
        SOBPCurve exp_sobp_curve(exp_depth_dose_curves, exp_monitor_units, exp_um_plexi, weights, true);

        const int um_mod_deg(1800);
        std::map<int, double> um_plexi_weight;
        um_plexi_weight[0] = weights[0];
        um_plexi_weight[800] = weights[1];
        Modulateur mod(QString("1"), 1.0, 1.0, 1.0, um_mod_deg, um_plexi_weight, true);
        DegradeurSet deg_set(Degradeur(QString("1"), 200, true));

        SOBPMaker maker(depth_dose_curves, monitor_units, um_plexi);
        SOBPCurve sobp_curve(maker.GetSOBP(mod, deg_set));
        QCOMPARE(sobp_curve, exp_sobp_curve);
    }
    catch (std::exception& exc) {
        qWarning() << "TestSOBPMaker::GetSOBPWithExtrapolation " + QString(exc.what());
        QFAIL("TestSOBPMaker::GetSOBP_WithExtrapolation Exception thrown in test");
    }
}
