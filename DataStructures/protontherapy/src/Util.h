#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <QStringList>
#include <map>

#include "Degradeur.h"
#include "DepthDose.h"
#include "DepthDoseCurve.h"
#include "Modulateur.h"
#include "Degradeur.h"
#include "Compensateur.h"

namespace util {
    double InterpolateMonitorUnits(const int& um_plexi, const std::map<int, double>& map);
    double ExtrapolateMonitorUnits(const int& um_plexi, const std::map<int, double>& map);
    DepthDoseCurve InterpolateDepthDoseCurve(const int& um_plexi, const std::map<int, DepthDoseCurve> &map);
    DepthDoseCurve ExtrapolateDepthDoseCurve(const int& um_plexi, const std::map<int, DepthDoseCurve> &map);
    double DebitDebroca(const double& ref_debit, const Modulateur& mod, const DegradeurSet& deg_set);
    bool GetStripperCurrentAndSeanceDuration(double debit, double dose_gy, double desired_duration, double *i_stripper, double *actual_duration);
    double GetStripperCurrent(const double& debit, const double& dose_gy, const double& seance_duration);
    double GetSeanceDuration(const double& debit, const double& dose_gy, const double& stripper_current);
    int GetUMCorrection(const double& debit, const double& i_stripper);
}

#endif // UTIL_H
