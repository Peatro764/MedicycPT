#ifndef SOBPCURVE_H
#define SOBPCURVE_H

#include <vector>
#include <QMap>

#include "DepthDoseCurve.h"

class SOBPCurve
{
public:
    SOBPCurve();
    SOBPCurve(const std::vector<DepthDoseCurve>& depth_dose_curves,
              const std::vector<double>& monitor_units,
              const std::vector<int>& um_plexi,
              const std::vector<double>& weight,
              const bool& extrapolated);
    double ZHalf() const;
    double DMUZHalf() const;
    double DMU(const double& depth) const;
    DepthDoseCurve Curve() const { return sobp_curve_; }
    std::vector<DepthDoseCurve> IndividualWeightedCurves();
    bool Extrapolated() const { return extrapolated_; }

private:
    DepthDoseCurve CalculateCurve() const;
    std::vector<DepthDoseCurve> depth_dose_curves_;
    std::vector<double> monitor_units_;
    std::vector<int> um_plexi_;
    std::vector<double> weight_;
    DepthDoseCurve sobp_curve_;
    bool extrapolated_;
};

bool operator==(const SOBPCurve& c1, const SOBPCurve& c2);
bool operator!=(const SOBPCurve& c1, const SOBPCurve& c2);

#endif // SOBPCURVE_H
