#include "Util.h"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <QDebug>

namespace util {
    double InterpolateMonitorUnits(const int& um_plexi, const std::map<int, double>& map) {
        if (map.empty()) {
            throw std::runtime_error("No monitor values available");
        }
        if (um_plexi < map.begin()->first || um_plexi > map.rbegin()->first) {
            throw std::runtime_error("No monitor values available for plexiglas thickness " + QString::number(um_plexi / 1000, 'f', 1).toStdString() + " mm");
        }
        auto it_find(map.find(um_plexi));
        if (it_find != map.end()) {
            return it_find->second;
        }

        auto lower = map.lower_bound(um_plexi) == map.begin() ? map.begin() : --(map.lower_bound(um_plexi));
        auto upper = map.upper_bound(um_plexi) == map.end() ? --map.end() :  map.upper_bound(um_plexi);
        if (upper == lower) {
            return lower->second;
        }

       return (lower->second + ((double(um_plexi - lower->first) / double(upper->first - lower->first))) * (upper->second - lower->second));
    }

    double ExtrapolateMonitorUnits(const int& um_plexi, const std::map<int, double>& map) {
        if (map.size() < 2) {
            throw std::runtime_error("No enough data to extrapolate");
        }
        if (um_plexi <= map.rbegin()->first) {
            throw std::runtime_error("Not allowed to extrapolate values smaller or equal than max plexiglas thickness");
        }
        if (um_plexi > map.rbegin()->first*1.10) {
            throw std::runtime_error("Not allowed to extrapolate plexiglas thickness more than 10%");
        }

        auto next_to_last(++map.rbegin());
        auto last(map.rbegin());

        const double extrapolated((next_to_last->second + ((double(um_plexi - next_to_last->first)) / (double(last->first - next_to_last->first))) *
                                   (last->second - next_to_last->second)));

        return std::max(extrapolated, 0.0);
    }

    DepthDoseCurve InterpolateDepthDoseCurve(const int& um_plexi, const std::map<int, DepthDoseCurve> &map) {
        if (map.empty()) {
            throw std::runtime_error("No depth dose curves available");
        }
        if (um_plexi < map.begin()->first || um_plexi > map.rbegin()->first) {
            throw std::runtime_error("No depth dose curves available for plexiglas thickness " + QString::number(um_plexi / 1000, 'f', 1).toStdString() + " mm");
        }
        auto it_find(map.find(um_plexi));
        if (it_find != map.end()) {
            return it_find->second;
        }

        auto lower = map.lower_bound(um_plexi) == map.begin() ? map.begin() : --(map.lower_bound(um_plexi));
        auto upper = map.upper_bound(um_plexi) == map.end() ? --map.end() :  map.upper_bound(um_plexi);
        if (upper == lower) {
            return lower->second;
        }

       return (lower->second + ((double(um_plexi - lower->first) / double(upper->first - lower->first))) * (upper->second - lower->second));
    }

    DepthDoseCurve ExtrapolateDepthDoseCurve(const int& um_plexi, const std::map<int, DepthDoseCurve> &map) {
        if (map.size() < 2) {
            throw std::runtime_error("No enough data to extrapolate depth dose curve");
        }
        if (um_plexi <= map.rbegin()->first) {
            throw std::runtime_error("Not allowed to extrapolate values smaller or equal than max plexiglas thickness");
        }
        if (um_plexi > map.rbegin()->first*1.10) {
            throw std::runtime_error("Not allowed to extrapolate plexiglas thickness more than 10%");
        }

        auto next_to_last(++map.rbegin());
        auto last(map.rbegin());

        const DepthDoseCurve extrapolated((next_to_last->second + ((double(um_plexi - next_to_last->first)) / (double(last->first - next_to_last->first))) *
                                   (last->second - next_to_last->second)));

        return extrapolated.CapAtZero();
    }


    double DebitDebroca(const double& ref_debit, const Modulateur& mod, const DegradeurSet& deg_set) {
        const double n_modulator_steps(mod.steps());
        const double deg_thickness_mm(((double)(deg_set.um_plexi_total() + mod.RangeShift())) / 1000.0);
        const double debit(ref_debit * 0.991 * (-0.7847 * log(1.1115 * deg_thickness_mm +
                                                              (n_modulator_steps - 1.0) * (0.8 * 1.18 / 1.05) * 0.825 * (1.0 - 0.0008 * (n_modulator_steps - 1.0) *  (0.8 * 1.18 / 1.05))) + 3.0209));
        return debit;
    }

    bool GetStripperCurrentAndSeanceDuration(double debit, double dose_gy, double desired_duration, double *i_stripper, double *actual_duration) {
        const double min_i_stripper = 20.0;
        const double max_i_stripper = 400.0;
        const double i_stripper_nominal = GetStripperCurrent(debit, dose_gy, desired_duration);
        if (i_stripper_nominal > max_i_stripper) {
            *i_stripper = max_i_stripper;
            *actual_duration = GetSeanceDuration(debit, dose_gy, max_i_stripper);
            return false;
        } else if (i_stripper_nominal < min_i_stripper) {
            *i_stripper = min_i_stripper;
            *actual_duration = GetSeanceDuration(debit, dose_gy, min_i_stripper);
            return false;
        } else {
            *i_stripper = i_stripper_nominal;
            *actual_duration = desired_duration;
            return true;
        }
    }

    double GetStripperCurrent(const double& debit, const double& dose_gy, const double& seance_duration) {
        if (debit < 0.1 ||
            dose_gy < 0.01 ||
            seance_duration < 0.1) {
              qWarning() << "Util::GetStripperCurrent Non valid input parameters, returning zero. Debit = "
                         << QString::number(debit, 'f', 3) << " dose_gy = " << QString::number(dose_gy, 'f', 3)
                         << " seance duration = " << QString::number(seance_duration, 'f', 3);
              return 0.0;
        }
        return (190.95 * std::pow(debit, -1.546) *  dose_gy / seance_duration);
    }

    double GetSeanceDuration(const double& debit, const double& dose_gy, const double& stripper_current) {
        if (debit < 0.1 ||
            dose_gy < 0.01 ||
            stripper_current < 1.0) {
              qWarning() << "Util::GetSeanceDuration Non valid input parameters, returning zero. Debit = "
                         << QString::number(debit, 'f', 3) << " dose_gy = " << QString::number(dose_gy, 'f', 3)
                         << " stripper_current = " << QString::number(stripper_current, 'f', 3);

              return 0.0;
        }
        return (190.95 * std::pow(debit, -1.546) * dose_gy / stripper_current);
    }

    int GetUMCorrection(const double& debit, const double& i_stripper) {
            if (debit < 0.01 || i_stripper < 0.01) {
                qWarning() << "Util::GetUMCorrection Non valid input parameters, returning zero. Debit = "
                           << QString::number(debit, 'f', 3) << " i_stripper = " << QString::number(i_stripper, 'f', 3);

                return 0;
            }
            const double delta_um(round(i_stripper * 0.093648 * pow(debit, 0.3896)));
            return static_cast<int>(std::round(delta_um));
     }

}
