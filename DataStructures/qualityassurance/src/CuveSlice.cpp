#include "CuveSlice.h"

#include "Calc.h"

CuveSlice::CuveSlice() {}

void CuveSlice::Add(double x, double y, const BeamSignal& signal) {
    x_.push_back(x);
    y_.push_back(y);
    signal_.push_back(signal);
}

void CuveSlice::Clear() {
    x_.clear();
    y_.clear();
    signal_.clear();
}

double CuveSlice::IntegratedSignal(const Range &x_range,
                                   const Range &y_range,
                                   double noise) const {
    double integrated_signal(0.0);
    const int n(x_.size());
    for (int idx = 0; idx < n; ++idx) {
        if (x_range.IsInside(x_.at(idx)) &&
            y_range.IsInside(y_.at(idx))) {
            integrated_signal +=  signal_.at(idx).value(noise);
        }
    }
    return integrated_signal;
}

double CuveSlice::AverageSignal(const Range &x_range,
                                const Range &y_range,
                                double noise) const {
    double integrated_signal(0.0);
    int n(0);
    const int nmb(x_.size());
    for (int idx = 0; idx < nmb; ++idx) {
        if (x_range.IsInside(x_.at(idx)) && y_range.IsInside(y_.at(idx))) {
            integrated_signal += signal_.at(idx).value(noise);
            ++n;
        }
    }
    if (n) {
        return (integrated_signal / static_cast<double>(n));
    } else {
        return 0;
    }
}

QCPColorMapData CuveSlice::GetColorMapData() const {
    return GetColorMapData(x_bins_, y_bins_, x_range_, y_range_, 0.0);
}

QCPColorMapData CuveSlice::GetColorMapData(int x_bins, int y_bins,
                                           Range x_range, Range y_range,
                                           double noise) const {
    QCPColorMapData map(x_bins, y_bins, x_range.AsQCPRange(), y_range.AsQCPRange());
    for (size_t idx = 0; idx < x_.size(); ++idx) {
        map.setData(x_.at(idx), y_.at(idx), signal_.at(idx).value(noise));
    }
    return map;
}

bool operator==(const CuveSlice& c1, const CuveSlice& c2) {
    return (calc::AlmostEqual(c1.x_, c2.x_, 0.001) &&
            calc::AlmostEqual(c1.y_, c2.y_, 0.001) &&
            c1.signal_ == c2.signal_);
}

bool operator!=(const CuveSlice& c1, const CuveSlice& c2) {
    return !(c1 == c2);
}
