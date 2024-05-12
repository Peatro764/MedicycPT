#include "SOBPCurve.h"

#include <numeric>
#include <QDebug>

SOBPCurve::SOBPCurve() {}

SOBPCurve::SOBPCurve(const std::vector<DepthDoseCurve> &depth_dose_curves,
                     const std::vector<double> &monitor_units,
                     const std::vector<int> &um_plexi,
                     const std::vector<double> &weight,
                     const bool& extrapolated) :
    depth_dose_curves_(depth_dose_curves),
    monitor_units_(monitor_units),
    um_plexi_(um_plexi),
    weight_(weight),
    extrapolated_(extrapolated) {

    const auto N(um_plexi.size());
    if (depth_dose_curves.size() != N ||
            monitor_units.size() != N ||
            weight.size() != N) {
        throw std::runtime_error("Cannot create SOBPCurve from different size containers");
    }

    sobp_curve_ = CalculateCurve();
}

double SOBPCurve::ZHalf() const {
    auto max(sobp_curve_.Max());
    auto lower_range(sobp_curve_.LowerRange(0.95 * max.dose()));
    auto upper_range(sobp_curve_.UpperRange(0.95 * max.dose()));
    if (lower_range.depth() > max.depth()) {
        lower_range = max;
    }
    if (upper_range.depth() < max.depth()) {
        upper_range = max;
    }
    return 0.5 * (lower_range.depth() + upper_range.depth());
}

double SOBPCurve::DMUZHalf() const {
    return sobp_curve_.Dose(ZHalf()).dose();
}

double SOBPCurve::DMU(const double& depth) const {
    return sobp_curve_.Dose(depth).dose();
}

DepthDoseCurve SOBPCurve::CalculateCurve() const {
    const double sum_weights(std::accumulate(weight_.begin(), weight_.end(), 0.0));
    const double weighted_monitor_units(std::inner_product(weight_.begin(), weight_.end(), monitor_units_.begin(), 0.0) / sum_weights);
    DepthDoseCurve weighted_depthdosecurve(std::inner_product(weight_.begin(), weight_.end(), depth_dose_curves_.begin(), DepthDoseCurve()) / sum_weights);
    DepthDoseCurve sobp(weighted_depthdosecurve / weighted_monitor_units);
    return sobp;
}

std::vector<DepthDoseCurve> SOBPCurve::IndividualWeightedCurves() {
   const double sum_weights(std::accumulate(weight_.begin(), weight_.end(), 0.0));
   const double weighted_monitor_units(std::inner_product(weight_.begin(), weight_.end(), monitor_units_.begin(), 0.0) / sum_weights);
   std::vector<DepthDoseCurve> curves(depth_dose_curves_);
   for (int idx = 0; idx < (int)curves.size(); ++idx) {
       curves.at(idx) = (weight_.at(idx) / sum_weights) * (1.0 / weighted_monitor_units) * curves.at(idx);
   }
   return curves;
}

bool operator==(const SOBPCurve& c1, const SOBPCurve& c2) {
    return (c1.Curve() == c2.Curve());
}

bool operator!=(const SOBPCurve& c1, const SOBPCurve& c2) {
    return !(c1 == c2);
}
