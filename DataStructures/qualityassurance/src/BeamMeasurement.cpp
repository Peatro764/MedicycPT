#include "BeamMeasurement.h"

#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <math.h>

#include "QCPCurveUtils.h"
#include "Calc.h"

BeamMeasurement::BeamMeasurement() : hardware_(QA_HARDWARE::UNK) {}

BeamMeasurement::BeamMeasurement(QA_HARDWARE hardware) : hardware_(hardware) {}

BeamMeasurement::BeamMeasurement(QA_HARDWARE hardware, const std::vector<MeasurementPoint>& points, double noise) :
    hardware_(hardware), points_(points), signal_noise_(noise)
{}

BeamMeasurement::BeamMeasurement(QA_HARDWARE hardware,
                                 const std::vector<MeasurementPoint>& points,
                                 QDateTime timestamp,
                                 const MeasurementCurrents& current,
                                 double signal)
    : hardware_(hardware), points_(points), timestamp_(timestamp), currents_(current), signal_noise_(signal) {}

void BeamMeasurement::Sort(Axis axis) {
    auto SortPos = [&] (const MeasurementPoint& p1, const MeasurementPoint& p2) -> bool { return (p1.point().value(axis) < p2.point().value(axis)); };
    std::sort(points_.begin(), points_.end(), SortPos);
}

void BeamMeasurement::SetSignalNoise(double noise) {
    signal_noise_ = noise;
}

MeasurementPoint BeamMeasurement::Max() const {
    if (points_.empty()) {
        qWarning() << "BeamProfile::Max Trying to take max value from empty BeamProfile";
        throw std::runtime_error("Trying to take max value from empty BeamProfile");
    }
    auto comp = [&] (const MeasurementPoint& p1, const MeasurementPoint& p2) -> bool { return (p1.signal().value(signal_noise_) < p2.signal().value(signal_noise_)); };
    return *std::max_element(points_.begin(), points_.end(), comp);
}

std::vector<MeasurementPoint> BeamMeasurement::GetScaledPoints(Axis axis, double factor) const {
    std::vector<MeasurementPoint> points;
    for (auto& p : points_) {
        points.push_back(p.ScaleAxis(axis, factor));
    }
    return points;
}

std::vector<MeasurementPoint> BeamMeasurement::GetScaledIntensity(double scale_to) const {
    const MeasurementPoint max = Max();
    const double scale_factor = scale_to / max.intensity(signal_noise_);
    std::vector<MeasurementPoint> points;
    for (auto& p : points_) {
        points.push_back(p.ScaleIntensity(signal_noise_, scale_factor));
    }
    return points;
}

std::vector<MeasurementPoint> BeamMeasurement::GetTranslated(Axis axis, double mm) const {
    std::vector<MeasurementPoint> points;
    for (auto& p : points_) {
        points.push_back(p.TranslateAxis(axis, mm));
    }
    return points;
}

std::vector<double> BeamMeasurement::GetSignalValues() const {
    std::vector<double> signal;
    for (auto& p : points_) {
        signal.push_back(p.intensity(signal_noise_));
    }
    return signal;
}

std::vector<double> BeamMeasurement::GetAxisValues(Axis axis) const {
    std::vector<double> pos;
    for (auto &p : points_) {
        pos.push_back(p.pos(axis));
    }
    return pos;
}

QCPCurveDataContainer BeamMeasurement::GetIntensityCurve(Axis axis) const {
    QCPCurveDataContainer intensity_curve;
    int t(0);
    for (MeasurementPoint p : points_) {
        intensity_curve.add(QCPCurveData(t++, p.pos(axis), p.intensity(signal_noise_)));
    }
    return intensity_curve;
}

QCPCurveDataContainer BeamMeasurement::GetDifferentalIntensityCurve(Axis axis) const {
    QCPCurveDataContainer diff_curve;
    MeasurementPoint previous = points_.at(0);
    int t(0);
    for (MeasurementPoint p: points_) {
        diff_curve.add(QCPCurveData(t++, 0.5 * (p.pos(axis) + previous.pos(axis)), p.intensity(signal_noise_) - previous.intensity(signal_noise_)));
        previous = p;
    }
    return diff_curve;
}

QCPCurveDataContainer BeamMeasurement::GetChambreCurve(Axis axis) const {
    QCPCurveDataContainer chambre_curve;
    int t(0);
    for (MeasurementPoint p : points_) {
        chambre_curve.add(QCPCurveData(t++, p.pos(axis), p.signal().reference()));
    }
    return chambre_curve;
}

QCPCurveDataContainer BeamMeasurement::GetSignalNoiseCurve(Axis axis) const {
    QCPCurveDataContainer noise_curve;
    int t(0);
    for (MeasurementPoint p : points_) {
        noise_curve.add(QCPCurveData(t++, p.pos(axis), signal_noise_));
    }
    return noise_curve;
}

void BeamMeasurement::operator<<(const MeasurementPoint& p) {
    points_.push_back(p);
}

double BeamMeasurement::PenumbraForValue(QCPCurveDataContainer curve, double max_value) const {
    try {
        const double value_10 = 0.10 * max_value;
        const double value_90 = 0.90 * max_value;
        const double pos_10 = qcpcurveutils::RightMostKeyForValue(curve, value_10);
        const double pos_90 = qcpcurveutils::RightMostKeyForValue(curve, value_90);
        qDebug() << "BeamMeasurement::PenumbraForValue Value 10 " << QString::number(value_10, 'f', 3)
                 << " value 90 " << QString::number(value_90, 'f', 3)
                 << " pos 10 " << QString::number(pos_10, 'f', 3)
                 << " pos 90 " << QString::number(pos_90, 'f', 3);
        if (pos_10 <= pos_90) {
            qWarning() << "BeamMeasurement::Penumbra Could not derive a valid penumbra";
            throw std::runtime_error("Could not derive a valid penumbra");
        }
        return (pos_10 - pos_90);
    }
    catch (std::exception& exc) {
        qWarning() << "BeamMeasurement::Penumbra Exception thrown " << exc.what();
        return 0;
    }
}

double BeamMeasurement::GetMaxCurrentDiode() const {
    if (points_.empty()) {
        return 0.0;
    }
    auto comp = [&] (const MeasurementPoint& p1, const MeasurementPoint& p2) -> bool { return (p1.signal().raw() < p2.signal().raw()); };
    return (std::max_element(points_.begin(), points_.end(), comp))->signal().raw();
}

double BeamMeasurement::AveragePosition(Axis axis) const {
    if (points_.empty()) {
        return 0.0;
    }
    double sum(0.0);
    for (auto p : points_) {
        sum += p.pos(axis);
    }
    return (sum / static_cast<double>(points_.size()));
}

bool operator==(const BeamMeasurement& b1, const BeamMeasurement& b2) {
    return (b1.hardware_ == b2.hardware_&&
            b1.points_ == b2.points_ &&
            b1.timestamp_ == b2.timestamp_ &&
            b1.currents_ == b2.currents_);
}

bool operator!=(const BeamMeasurement& p1, const BeamMeasurement& p2) {
    return !(p1 == p2);
}
