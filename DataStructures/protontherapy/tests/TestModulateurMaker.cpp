#include "TestModulateurMaker.h"

#include "ModulateurMaker.h"
#include "Calc.h"
#include "Material.h"

#include <vector>

void TestModulateurMaker::initTestCase() {}

void TestModulateurMaker::cleanupTestCase() {}

void TestModulateurMaker::init() {}

void TestModulateurMaker::cleanup() {}

std::vector<DepthDoseCurve> TestModulateurMaker::GetDepthDoseCurves(int nmb) {
    DepthDose d1(0.0, 0.0);
    DepthDose d2(0.5, 1.1);
    DepthDose d3(1.0, 3.3);
    DepthDose d4(1.5, 4.3);
    DepthDose d5(2.0, 7.3);
    DepthDose d6(2.5, 9.3);
    DepthDose d7(3.0, 4.3);
    DepthDose d8(3.5, 0.0);

    std::vector<DepthDose> depth_doses { d1, d2, d3, d4, d5, d6, d7, d8 };
    std::vector<DepthDoseCurve> curves;
    for (int i = 0; i < nmb; ++i) {
        curves.push_back(DepthDoseCurve(depth_doses));
    }
    return curves;
}

void TestModulateurMaker::GetRequiredUmPlexiStepsNoDegradeur() {
    auto um_plexi_steps1(ModulateurMaker::GetRequiredUmPlexiSteps(30.0, 2.0, 30.0, 0.0, 0.8));
    std::vector<int> exp_plexi_steps1 { 0, 800, 1600 };
    QCOMPARE(um_plexi_steps1, exp_plexi_steps1);

    auto um_plexi_steps2(ModulateurMaker::GetRequiredUmPlexiSteps(30.0, 1.0, 30.0, 0.0, 0.8));
    std::vector<int> exp_plexi_steps2 { 0, 800};
    QCOMPARE(um_plexi_steps2, exp_plexi_steps2);

    auto um_plexi_steps3(ModulateurMaker::GetRequiredUmPlexiSteps(30.0, 0.3, 30.0, 0.0, 0.8));
    std::vector<int> exp_plexi_steps3 { 0 };
    QCOMPARE(um_plexi_steps3, exp_plexi_steps3);

    auto um_plexi_steps4(ModulateurMaker::GetRequiredUmPlexiSteps(30.0, 30.0, 30.0, 0.0, 0.8));
    std::vector<int> exp_plexi_steps4;
    for (int step = 0; step < 34; ++step)
        exp_plexi_steps4.push_back(step * 800);
    QCOMPARE(um_plexi_steps4, exp_plexi_steps4);
}

void TestModulateurMaker::GetRequiredUmPlexiStepsWithDegradeur() {
    auto um_plexi_steps1(ModulateurMaker::GetRequiredUmPlexiSteps(material::Plexiglas2Tissue(5.0), material::Plexiglas2Tissue(2.0),
                                                                  material::Plexiglas2Tissue(10.0), 0.0, 0.8));
    std::vector<int> exp_plexi_steps1 { 5000, 5800, 6600, 7400};
    QCOMPARE(um_plexi_steps1, exp_plexi_steps1);

    auto um_plexi_steps2(ModulateurMaker::GetRequiredUmPlexiSteps(material::Plexiglas2Tissue(2.0), material::Plexiglas2Tissue(2.0),
                                                       material::Plexiglas2Tissue(10.0), 0.0, 0.8));
    std::vector<int> exp_plexi_steps2 { 8000, 8800, 9600, 10400 };
    QCOMPARE(um_plexi_steps2, exp_plexi_steps2);
}

void TestModulateurMaker::GetRequiredUmPlexiStepsWithDegradeurAndDecalage() {
    auto um_plexi_steps1(ModulateurMaker::GetRequiredUmPlexiSteps(material::Plexiglas2Tissue(5.0), material::Plexiglas2Tissue(2.0),
                                                                  material::Plexiglas2Tissue(10.0), 0.4, 0.8));
    std::vector<int> exp_plexi_steps1 { 4600, 5400, 6200, 7000 };
    QCOMPARE(um_plexi_steps1, exp_plexi_steps1);
}

void TestModulateurMaker::GetSectorWeights() {
//    auto depth_dose_curves(GetDepthDoseCurves(6));
//    std::vector<int> um_plexi {0, 800, 1600, 2400, 3200, 4000};
//    ModulateurMaker maker(depth_dose_curves, um_plexi);

//    std::vector<int> plexi_steps { 0, 800, 1600 };
//    auto weights(maker.GetSectorWeights(plexi_steps, 0.0));
}

void TestModulateurMaker::GetWeightedCurves()  {
    DepthDose d1(0.0, 0.0);
    DepthDose d2(0.5, 1.1);

    std::vector<DepthDose> depth_doses { d1, d2 };
    std::vector<DepthDose> weighted_depth_doses { 2.0 * d1, 2.0 * d2 };

    um_plexi_depthDoseCurve curves;
    curves.emplace(0, DepthDoseCurve(depth_doses));
    curves.emplace(800, DepthDoseCurve(depth_doses));

    um_plexi_depthDoseCurve exp_curves;
    exp_curves.emplace(0, DepthDoseCurve(depth_doses));
    exp_curves.emplace(800, DepthDoseCurve(weighted_depth_doses));

    um_plexi_weight weights;
    weights.emplace(0, 1.0);
    weights.emplace(800, 2.0);

    ModulateurMaker maker(curves);
    um_plexi_depthDoseCurve weighted_curves(maker.GetWeightedCurves(weights));
    QCOMPARE(exp_curves, weighted_curves);
}

void TestModulateurMaker::GetSumOfWeightedCurves()  {
    DepthDose d1(0.0, 0.0);
    DepthDose d2(0.5, 1.1);
    std::vector<DepthDose> depth_doses { d1, d2 };
    um_plexi_weight weights;
    weights.emplace(0, 1.0);
    weights.emplace(800, 2.0);

    um_plexi_depthDoseCurve curves;
    curves.emplace(0, DepthDoseCurve(depth_doses));
    curves.emplace(800, DepthDoseCurve(depth_doses));

    std::vector<DepthDose> sum_weighted_depth_doses { d1, 2.0 * d2 + d2 };
    DepthDoseCurve exp_sum_of_weighted_curves(sum_weighted_depth_doses);

    ModulateurMaker maker(curves);
    DepthDoseCurve sum_of_weighted_curves(maker.GetSumOfWeightedCurves(weights));
    QCOMPARE(exp_sum_of_weighted_curves, sum_of_weighted_curves);
}

void TestModulateurMaker::IterateWeights()  {
    DepthDose d1(0.0, 0.0);
    DepthDose d2(0.5, 1.1);
    std::vector<DepthDose> depth_doses { d1, d2 };
    um_plexi_depthDoseCurve curves;
    curves.emplace(0, DepthDoseCurve(depth_doses));
    curves.emplace(800, DepthDoseCurve(depth_doses));
    ModulateurMaker maker(curves);

    um_plexi_weight weights1;
    weights1.emplace(800, 1.0);
    weights1.emplace(1600, 0.5);
    try {
        // different um plexi in curves and weights
        maker.IterateWeights(weights1);
        QFAIL("Having different weights in depthdosecurves and weights should make the funciton throw");
    }
    catch (...) {}

    um_plexi_weight weights2;
    weights2.emplace(0, 1.0);
    try {
        // different number of um plexi in curves and weights
        maker.IterateWeights(weights2);
        QFAIL("Having different number of weights in depthdosecurves and weights should make the funciton throw");
    }
    catch (...) {}

}



