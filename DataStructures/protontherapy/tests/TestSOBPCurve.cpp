#include "TestSOBPCurve.h"

#include "SOBPCurve.h"
#include "Calc.h"

#include <vector>

void TestSOBPCurve::initTestCase() {}

void TestSOBPCurve::cleanupTestCase() {}

void TestSOBPCurve::init() {}

void TestSOBPCurve::cleanup() {}

void TestSOBPCurve::Constructor() {
    DepthDose small(5.0, 4.4);
    DepthDose medium(10, 1.1);
    DepthDose large(14.0, 3.3);
    std::vector<DepthDose> depth_doses { small, medium, large };
    std::vector<DepthDoseCurve> curves { 1.1 * DepthDoseCurve(depth_doses), 1.3 * DepthDoseCurve(depth_doses), 1.8 * DepthDoseCurve(depth_doses),
                                  0.5 * DepthDoseCurve(depth_doses), 1.2 * DepthDoseCurve(depth_doses), 1.5 * DepthDoseCurve(depth_doses) };
    std::vector<double> monitor_units { 100.0, 110.0, 218.0, 145.0, 33.0, 47.0 };
    std::vector<int> um_plexi { 1000, 1800, 2600, 3400, 4200, 5000 };
    std::vector<double> weight {0.1, 0.5, 0.2, 0.7, 0.3, 0.5};
    SOBPCurve sobp(curves, monitor_units, um_plexi, weight, false);
    auto sobp_curve = sobp.Curve();
    std::vector<DepthDose> exp_depth_doses { DepthDose(5.0, 0.046620123), DepthDose(10.0, 0.011655031), DepthDose(14.0, 0.034965094) };
    QCOMPARE(sobp_curve, DepthDoseCurve(exp_depth_doses));
}


void TestSOBPCurve::Curve() {

}
