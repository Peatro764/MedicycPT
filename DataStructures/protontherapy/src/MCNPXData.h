#ifndef MCNPXDATA_H
#define MCNPXDATA_H

#include "DepthDoseCurve.h"

#include <vector>
#include <map>

typedef std::map<int, DepthDoseCurve> um_plexi_depthDoseCurve;

class MCNPXData
{
public:
    MCNPXData(const std::vector<DepthDoseCurve>& depth_dose_curves,
              const std::vector<int>& um_plexi,
              const std::vector<double>& monitor_units);
    std::vector<DepthDoseCurve> depth_dose_curves() const { return depth_dose_curves_; }
    std::vector<int> um_plexi() const { return um_plexi_; }
    std::vector<double> monitor_units() const { return monitor_units_; }
    um_plexi_depthDoseCurve UmPlexiDepthDoseCurves() const;

private:
    const std::vector<DepthDoseCurve> depth_dose_curves_;
    const std::vector<int> um_plexi_;
    const std::vector<double> monitor_units_;
};

#endif // MCNPXDATA_H
