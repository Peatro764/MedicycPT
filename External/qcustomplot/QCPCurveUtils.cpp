#include "QCPCurveUtils.h"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <QDebug>

#include "qcustomplot.h"

namespace qcpcurveutils {

QCPCurveDataContainer Discretize(const QCPCurveDataContainer& curve, const double& max_err, const double& err_key_scaling) {
    if (curve.size() < 4) {
        throw std::runtime_error("Cannot construct discretized curve from less than 4 elements");
    }
    if (err_key_scaling > 1.0 || err_key_scaling < 0.001) {
        throw std::runtime_error("Error key scaling out of bounds, cannot construct discretized curve");
    }
    const double r((curve.constEnd() - 1)->mainKey());
    QCPCurveDataContainer discretized;
    discretized.add(*(curve.constBegin()));
    for (auto it = (curve.constBegin() + 1); it != (curve.constEnd() - 1); ++it) {
        if (fabs(discretized.at(discretized.size() - 1)->mainValue() - (it + 1)->mainValue())
                > max_err * (err_key_scaling * (1.0 - it->mainKey() / r) + it->mainKey() / r)) {
            discretized.add(*it);
        }
    }
    discretized.add(*(curve.constEnd() - 1));
    return discretized;
}

std::vector<double> ValuesInKeyRange(const QCPCurveDataContainer& curve, int idx1, int idx2) {
    std::vector<double> values;
    const int lower_idx = std::max(0, idx1 < idx2 ? idx1 : idx2);
    const int upper_idx = std::min(curve.size() - 1, idx1 > idx2 ? idx1 : idx2);
    if (idx1 == idx2) return values;
    if (lower_idx >= curve.size()) return values;
    if (upper_idx < 0) return values;
    for (int i = lower_idx; i <= upper_idx; ++i) {
        values.push_back(curve.at(i)->mainValue());
    }
    return values;
}

void ScaleValueAxis(QCPCurveDataContainer &curve, const double &scale_factor) {
    if (!curve.size()) { return; }
    std::for_each(curve.begin(), curve.end(),
                  [&scale_factor](QCPCurveData &p) { p.value = p.value * scale_factor; } );
}

void SetMaxValue(QCPCurveDataContainer &curve, const double &max_value) {
    if (!curve.size()) { return; }
    const double add_value(max_value - MaxValue(curve));
    std::for_each(curve.begin(), curve.end(), [&](QCPCurveData &p) { p.value += add_value; });
}

QCPCurveDataContainer CutAt(QCPCurveDataContainer& curve, const double &cut_level, bool keep_above) {
    QCPCurveDataContainer cutted;
    if (!curve.size()) { return cutted; }
    for (auto it = curve.constBegin(); it != curve.constEnd(); ++it) {
        if ((it->mainValue() >= cut_level && keep_above) || (it->mainValue() <= cut_level && !keep_above)) {
            cutted.add(*it);
        }
    }
    return cutted;
}

QCPCurveDataContainer StepCurve(const QCPCurveDataContainer& curve, const double& min_step_height) {
    if (curve.size() < 2) {
        throw std::runtime_error("Cannot construct step curve from less than 2 elements");
    }
    QCPCurveDataContainer step_curve;
    step_curve.add(QCPCurveData(curve.at(0)->sortKey(), curve.at(0)->mainKey(), std::max(curve.at(0)->mainValue(), min_step_height)));
    bool step_above(true);
    for (int idx = 1; idx < curve.size(); ++idx) {
        if (step_above) {
            step_curve.add(QCPCurveData(curve.at(idx)->sortKey(), curve.at(idx)->mainKey(), std::max(curve.at(idx - 1)->mainValue(), min_step_height)));
        } else {
            if (idx < (curve.size() - 1)) {
                step_curve.add(QCPCurveData(curve.at(idx)->sortKey(), curve.at(idx - 1)->mainKey(), std::max(curve.at(idx)->mainValue(), min_step_height)));
            } else {
                step_curve.add(QCPCurveData(curve.at(idx)->sortKey(), curve.at(idx - 1)->mainKey(), curve.at(idx)->mainValue()));
            }
        }
        step_above = !step_above;
    }
    if (!step_above) {
        step_curve.add(*curve.at(curve.size() - 1));
    }
    return step_curve;
}

double MinValue(QCPCurveDataContainer& data) {
    bool found(false);
    const double min(data.valueRange(found).lower);
    if (!found) { throw std::runtime_error("Did not find lower range in curve data"); }
    return min;
}

double MaxValue(QCPCurveDataContainer& data) {
    bool found(false);
    const double max(data.valueRange(found).upper);
    if (!found) { throw std::runtime_error("Did not find upper range in curve data"); }
    return max;
}

QCPCurveData MaxElement(QCPCurveDataContainer& data) {
    return MaxElement(data, 0, data.size() - 1);
}

QCPCurveData MaxElement(QCPCurveDataContainer& data, int start_idx, int end_idx) {
    if (data.size() == 0) {
        throw std::runtime_error("Cannot get max element of empty QCPCurveDataContainer");
    }
    if (start_idx < 0 || start_idx > (data.size() - 1) || end_idx < start_idx || end_idx > (data.size() - 1)) {
        throw std::runtime_error("Idx out of range");
    }
    QCPCurveData max_element = *data.at(start_idx);
    for (int idx = start_idx; idx <= end_idx; ++idx) {
        if (data.at(idx)->mainValue() > max_element.mainValue()) max_element = *data.at(idx);
    }
    return max_element;
}

QCPCurveData MinElement(QCPCurveDataContainer& data) {
    return MinElement(data, 0, data.size() - 1);
}

QCPCurveData MinElement(QCPCurveDataContainer& data, int start_idx, int end_idx) {
    if (data.size() == 0) {
        throw std::runtime_error("Cannot get min element of empty QCPCurveDataContainer");
    }
    if (start_idx < 0 || start_idx > (data.size() - 1) || end_idx < start_idx || end_idx > (data.size() - 1)) {
        throw std::runtime_error("Idx out of range");
    }
    QCPCurveData min_element = *data.at(start_idx);
    for (int idx = start_idx; idx <= end_idx; ++idx) {
        if (data.at(idx)->mainValue() < min_element.mainValue()) min_element = *data.at(idx);
    }
    return min_element;
}

QCPCurveDataContainer Smooth(const QCPCurveDataContainer& data, int elements) {
    // elements = #elements to smooth with on each side of the element in question.
    // Ex: elements = 2 -> use 5 elements in total (2 on each side + element in question)
    if (elements < 1) {
        return data;
    }
    QCPCurveDataContainer smoothed_curve;
    for (int idx = 0; idx < data.size(); ++idx) {
        double smoothed_value(0.0);
        const int min_idx = std::max(0, idx - elements);
        const int max_idx = std::min(data.size() - 1, idx + elements);
        for (int s_idx = min_idx; s_idx <= max_idx ; ++s_idx) {
            smoothed_value += data.at(s_idx)->mainValue();
        }
        smoothed_curve.add(QCPCurveData(data.at(idx)->sortKey(), data.at(idx)->mainKey(), smoothed_value / static_cast<double>(max_idx - min_idx + 1)));
    }
    return smoothed_curve;
}

double ValueAtKey(const QCPCurveDataContainer& data, double key) {
    auto InInterval = [&] (const QCPCurveData& d1, const QCPCurveData& d2) -> bool { return (key >= d1.mainKey() && key <= d2.mainKey()); };
    auto it = std::adjacent_find(data.constBegin(), data.constEnd(), InInterval);
    if (it == data.constEnd()) {
        qWarning() << "QCPCurveUtils::ValueAtKey Key [" << key << "] not in interval [" << data.at(0)->mainKey() << "," << data.at(data.size() - 1)->mainKey() << "]";
        throw std::runtime_error("Key not in data interval");
    }
    const QCPCurveData d1 = *it;
    const QCPCurveData d2 = *(++it);
    const double interpolated_value(d1.mainValue() + ((d2.mainValue() - d1.mainValue()) * ((key - d1.mainKey()) / (d2.mainKey() - d1.mainKey()))));
    return interpolated_value;
}

double LeftMostKeyForValue(const QCPCurveDataContainer& data, double value) {
    auto InInterval = [&] (const QCPCurveData& d1, const QCPCurveData& d2) -> bool { return ((value >= d1.mainValue() && value <= d2.mainValue()) ||
                                                                                             (value <= d1.mainValue() && value >= d2.mainValue())); };
    auto it = std::adjacent_find(data.constBegin(), data.constEnd(), InInterval);
    if (it == data.constEnd()) {
        qWarning() << "QCPCurveUtils::LeftMostKeyForValue Value [" << value << "] not in interval";
        throw std::runtime_error("Beam profile value not in data interval");
    }
    const QCPCurveData d1 = *it;
    const QCPCurveData d2 = *(++it);
    if (AlmostEqual(d1.mainKey(), d2.mainKey(), 0.0001)) {
        return (d1.mainKey() + d2.mainKey()) / 2.0;
    } else {
        const double k = (d1.mainValue() - d2.mainValue()) / (d1.mainKey() - d2.mainKey());
        const double interpolated_value =  (1.0 / k) * (value + k * d2.mainKey() - d2.mainValue());
        return interpolated_value;
    }
}

double RightMostKeyForValue(const QCPCurveDataContainer& data, double value) {
    int idx_1 = -1;
    int idx_2 = -1;
    for (int idx = (data.size() - 1); idx > 0; --idx) {
        if ((value >= data.at(idx)->mainValue() && value <= data.at(idx - 1)->mainValue()) ||
            (value <= data.at(idx)->mainValue() && value >= data.at(idx - 1)->mainValue())) {
            idx_1 = idx;
            idx_2 = idx - 1;
            break;
        }
    }
    if (idx_1 < 0 || idx_2 < 0) {
        qWarning() << "QCPCurveUtils::RightMostKeyForValue Value [" << value << "] not in interval";
        throw std::runtime_error("Value not in data interval");
    }
    const QCPCurveData d1 = *data.at(idx_1);
    const QCPCurveData d2 = *data.at(idx_2);
    if (AlmostEqual(d1.mainKey(), d2.mainKey(), 0.0001)) {
        return (d1.mainKey() + d2.mainKey()) / 2.0;
    } else {
        const double k = (d1.mainValue() - d2.mainValue()) / (d1.mainKey() - d2.mainKey());
        const double interpolated_value =  (1.0 / k) * (value + k * d2.mainKey() - d2.mainValue());
        return interpolated_value;
    }
}
bool AlmostEqual(double a, double b, double maxRelDiff = 0.001) {
    double diff = fabs(a - b);
    a = fabs(a);
    b = fabs(b);
    double largest = (b > a) ? b : a;
    return (diff <= largest * maxRelDiff);
}
}

bool operator==(const QCPCurveDataContainer &c1, const QCPCurveDataContainer &c2) {
    if (c1.size() != c2.size()) {
        return false;
    }
    for (int idx = 0; idx < c1.size(); ++idx) {
        if (*c1.at(idx) != *c2.at(idx)) {
            return false;
        }
    }
    return true;
}

bool operator!=(const QCPCurveDataContainer &c1, const QCPCurveDataContainer &c2) {
    return !(c1 == c2);
}

bool operator==(const QCPCurveData &c1, const QCPCurveData &c2) {
    return (qcpcurveutils::AlmostEqual(c1.key, c2.key, 0.0001) &&
            qcpcurveutils::AlmostEqual(c1.value, c2.value, 0.0001));
}

bool operator!=(const QCPCurveData &c1, const QCPCurveData &c2) {
    return !(c1 == c2);
}


