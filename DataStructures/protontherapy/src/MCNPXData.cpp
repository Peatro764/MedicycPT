#include "MCNPXData.h"

#include <stdexcept>

MCNPXData::MCNPXData(const std::vector<DepthDoseCurve>& depth_dose_curves,
          const std::vector<int>& um_plexi,
          const std::vector<double>& monitor_units) : depth_dose_curves_(depth_dose_curves),
    um_plexi_(um_plexi),
    monitor_units_(monitor_units) {
    if (um_plexi.size() != depth_dose_curves.size() ||
            depth_dose_curves.size() != monitor_units.size()) {
        throw std::runtime_error("MCNPXData cannot be created with different size vectors");
    }
}



um_plexi_depthDoseCurve MCNPXData::UmPlexiDepthDoseCurves() const {
    um_plexi_depthDoseCurve curves;
    for (std::size_t idx = 0; idx < um_plexi_.size(); ++idx) {
        curves.emplace(um_plexi_.at(idx), depth_dose_curves_.at(idx));
    }
    return curves;
}
