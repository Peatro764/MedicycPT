#include "SOBPMaker.h"

#include "Calc.h"
#include "Util.h"

SOBPMaker::SOBPMaker(const std::vector<DepthDoseCurve> &depth_dose_curves,
                     const std::vector<double> &monitor_units,
                     const std::vector<int> &um_plexi)
{
    const auto N(um_plexi.size());
    if (depth_dose_curves.size() != N ||
            monitor_units.size() != N) {
        throw std::runtime_error("Cannot construct a SOBPMaker with vectors of different size");
    }

    for (std::size_t idx = 0; idx < N; ++idx) {
        raw_um_plexi_depthDoseCurve_[um_plexi[idx]] = depth_dose_curves[idx];
        raw_um_plexi_monitor_units_[um_plexi[idx]] = monitor_units[idx];
    }
}

SOBPCurve SOBPMaker::GetSOBP(const Modulateur &mod, const DegradeurSet &deg_set) const {
    auto um_plexi_vector(GetUMPlexiVector(mod, deg_set));
    auto weight_vector(GetWeightVector(mod));
    auto monitor_units_vector(GetMonitorUnitsVector(um_plexi_vector));
    auto depth_dose_curve_vector(GetDepthDoseCurves(um_plexi_vector));
    bool extrapolated(*(um_plexi_vector.rbegin()) > raw_um_plexi_monitor_units_.rbegin()->first);
    auto sobp(SOBPCurve(depth_dose_curve_vector,
                        monitor_units_vector,
                        um_plexi_vector,
                        weight_vector,
                        extrapolated));
    return sobp;
}

std::vector<int> SOBPMaker::GetUMPlexiVector(const Modulateur &mod, const DegradeurSet &deg_set) const {
    std::vector<int> um_plexi_vector;
    for (auto it = mod.um_plexi_weight().begin(); it != mod.um_plexi_weight().end(); ++it) {
      um_plexi_vector.push_back(deg_set.um_plexi_total() + mod.RangeShift() + it->first);
    }
    return um_plexi_vector;
}

std::vector<double> SOBPMaker::GetWeightVector(const Modulateur &mod) const {
    std::vector<double> weight_vector;
    for (auto it = mod.um_plexi_weight().begin(); it != mod.um_plexi_weight().end(); ++it) {
      weight_vector.push_back(it->second);
    }
    return weight_vector;
}

std::vector<double> SOBPMaker::GetMonitorUnitsVector(const std::vector<int> &um_plexi_vector) const {
    std::vector<double> monitor_units_vector;
    for (auto um_plexi : um_plexi_vector) {
        if (um_plexi <= raw_um_plexi_monitor_units_.rbegin()->first) {
            monitor_units_vector.push_back(util::InterpolateMonitorUnits(um_plexi, raw_um_plexi_monitor_units_));
        } else {
            monitor_units_vector.push_back(raw_um_plexi_monitor_units_.rbegin()->second);
        }
    }
    return monitor_units_vector;
}

std::vector<DepthDoseCurve> SOBPMaker::GetDepthDoseCurves(const std::vector<int>& um_plexi_vector) const {
    std::vector<DepthDoseCurve> depth_dose_curves;
    for (auto um_plexi : um_plexi_vector) {
        if (um_plexi <= raw_um_plexi_depthDoseCurve_.rbegin()->first) {
            depth_dose_curves.push_back(util::InterpolateDepthDoseCurve(um_plexi, raw_um_plexi_depthDoseCurve_));
        } else {
            depth_dose_curves.push_back(raw_um_plexi_depthDoseCurve_.rbegin()->second);
        }
    }
    return depth_dose_curves;
}



