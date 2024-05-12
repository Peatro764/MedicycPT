#ifndef SOBPMAKER_H
#define SOBPMAKER_H

#include <map>

#include "DepthDoseCurve.h"
#include "SOBPCurve.h"
#include "Modulateur.h"
#include "Degradeur.h"

typedef std::map<int, double> um_plexi_weight;
typedef std::map<int, DepthDoseCurve> um_plexi_depthDoseCurve;
typedef std::map<int, double> um_plexi_monitor_units;

class SOBPMaker
{
public:
    SOBPMaker(const std::vector<DepthDoseCurve>& depth_dose_curves,
              const std::vector<double>& monitor_units,
              const std::vector<int>& um_plexi);

    SOBPCurve GetSOBP(const Modulateur &mod, const DegradeurSet& deg_set) const;

private:
    std::vector<int> GetUMPlexiVector(const Modulateur& mod, const DegradeurSet& deg_set) const;
    std::vector<double> GetWeightVector(const Modulateur& mod) const;
    std::vector<double> GetMonitorUnitsVector(const std::vector<int>& um_plexi_vector) const;
    std::vector<DepthDoseCurve> GetDepthDoseCurves(const std::vector<int>& um_plexi_vector) const;

    um_plexi_depthDoseCurve raw_um_plexi_depthDoseCurve_;
    um_plexi_monitor_units raw_um_plexi_monitor_units_;
};


#endif // SOBPMAKER_H
