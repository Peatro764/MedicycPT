#include "BeamProfile.h"

#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <math.h>
#include "Calc.h"

#include "QCPCurveUtils.h"

BeamProfile::BeamProfile()
    : BeamMeasurement(QA_HARDWARE::UNK), axis_(Axis::UNK) {}

BeamProfile::BeamProfile(QA_HARDWARE hardware, Axis axis) :
    BeamMeasurement(hardware), axis_(axis)
{}

BeamProfile::BeamProfile(QA_HARDWARE hardware, Axis axis, const std::vector<MeasurementPoint> &points, QDateTime timestamp,
                         MeasurementCurrents currents, int smoothing_elements, double noise)
    : BeamMeasurement(hardware, points, timestamp, currents, noise), axis_(axis),
      smoothing_elements_(smoothing_elements) {}

BeamProfileResults BeamProfile::GetResults() const {
    QCPCurveDataContainer intensity_curve = this->GetIntensityCurve(axis_);
    QCPCurveDataContainer diff_intensity_curve = this->GetDifferentalIntensityCurve(axis_);

    double centre_pos = this->Centre(diff_intensity_curve);
    qDebug() << "BeamProfile::GetResults Centre " << centre_pos;
    double w50 = this->Width(intensity_curve, centre_pos, 0.50);
    qDebug() << "BeamProfile::GetResults W50 " << w50;
    double w90 = this->Width(intensity_curve, centre_pos, 0.90);
    qDebug() << "BeamProfile::GetResults W90 " << w90;
    double w95 = this->Width(intensity_curve, centre_pos, 0.95);
    qDebug() << "BeamProfile::GetResults W95 " << w95;
    const double centre_value = qcpcurveutils::ValueAtKey(intensity_curve, centre_pos);
    double penumbra = PenumbraForValue(intensity_curve, centre_value);
    qDebug() << "BeamProfile::GetResults Penumbra " << penumbra;
    double flatness = this->Flatness();
    qDebug() << "BeamProfile::GetResults Flatness " << flatness;

    return BeamProfileResults(axis_, this->GetTimestamp(), centre_pos, w95, w90, w50, penumbra, flatness);
}

double BeamProfile::Centre() const {
    return Centre(this->GetDifferentalIntensityCurve(axis_));
}

double BeamProfile::Centre(QCPCurveDataContainer diff_curve) const {
    auto smoothed_curve = qcpcurveutils::Smooth(diff_curve, smoothing_elements_);
    QCPCurveData max = qcpcurveutils::MaxElement(smoothed_curve);
    QCPCurveData min = qcpcurveutils::MinElement(smoothed_curve);
    if (max.mainKey() >= min.mainKey()) {
        qDebug() << "BeamProfile::Centre diff max to the right of diff min";
        throw std::runtime_error("Could not find two maxima in differential intensity spectrum");
    }
    return (max.mainKey() + min.mainKey()) / 2.0;
}

double BeamProfile::Width(double percentage_of_max) const {
    QCPCurveDataContainer diff_intensity_curve = this->GetDifferentalIntensityCurve(axis_);
    double centre = this->Centre(diff_intensity_curve);
    QCPCurveDataContainer intensity_curve = this->GetIntensityCurve(axis_);

    return this->Width(intensity_curve, centre, percentage_of_max);
}

double BeamProfile::Flatness() const {
    QCPCurveDataContainer intensity_curve = this->GetIntensityCurve(axis_);
    QCPCurveData max = qcpcurveutils::MaxElement(intensity_curve);
    QCPCurveDataContainer diff_intensity_curve = this->GetDifferentalIntensityCurve(axis_);
    double centre_pos = this->Centre(diff_intensity_curve);
    return (100.0 * this->LinearSlope(intensity_curve, centre_pos, 0.925) * this->Width(0.925) / max.value);
}

double BeamProfile::LinearSlope(QCPCurveDataContainer curve, double centre, double percentage_of_max) const {
    const double centre_value = qcpcurveutils::ValueAtKey(curve, centre);
    const double threshold_value = percentage_of_max * centre_value;
    QCPCurveDataContainer above_threshold = qcpcurveutils::CutAt(curve, threshold_value, true);
    std::vector<double> intensity;
    std::vector<double> position;
    for (auto it = above_threshold.constBegin(); it != above_threshold.constEnd(); ++it) {
        intensity.push_back(it->mainValue());
        position.push_back(it->mainKey());
    }
    if (intensity.empty()) {
        throw std::runtime_error("Curve empty");
    }
    std::pair<double, double> linear_fit = calc::LinearFit(position, intensity);
    return linear_fit.second;
}

double BeamProfile::Width(QCPCurveDataContainer curve, double centre, double percentage_of_max) const {
    try {
        const double centre_value = qcpcurveutils::ValueAtKey(curve, centre);
        const double threshold_value = percentage_of_max * centre_value;
        const double left_pos = qcpcurveutils::LeftMostKeyForValue(curve, threshold_value);
        const double right_pos = qcpcurveutils::RightMostKeyForValue(curve, threshold_value);
        if (left_pos >= right_pos) {
            qWarning() << "BeamProfile::Width Could not find width for given percentage of max";
            std::runtime_error("Could not find width for given percentage of max");
        }
        return (right_pos - left_pos);
    }
    catch (std::exception& exc) {
        qWarning() << "BeamProfile::Width Exception thrown " << exc.what();
        return 0;
    }
}

double BeamProfile::Penumbra() const {
    QCPCurveDataContainer diff_intensity_curve = this->GetDifferentalIntensityCurve(axis_);
    const double centre = this->Centre(diff_intensity_curve);
    QCPCurveDataContainer intensity_curve = this->GetIntensityCurve(axis_);
    const double centre_value = qcpcurveutils::ValueAtKey(intensity_curve, centre);
    return PenumbraForValue(intensity_curve, centre_value);
}

bool BeamProfile::ResultsValid() const {
    try {
        BeamProfileResults results = this->GetResults();
        qDebug() << "BeamProfile::ResultsValid: " << std::isfinite(results.centre()) << " "
                                                  << std::isfinite(results.penumbra()) << " "
                                                  << std::isfinite(results.width_50()) << " "
                                                  << std::isfinite(results.width_90()) << " "
                                                  << std::isfinite(results.width_95());
        return (std::isfinite(results.centre()) &&
                std::isfinite(results.penumbra()) &&
                std::isfinite(results.width_50()) &&
                std::isfinite(results.width_90()) &&
                std::isfinite(results.width_95()));
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfile::ResultsValid Exception thrown: " << exc.what();
        return false;
    }
}

bool operator==(const BeamProfile& b1, const BeamProfile& b2) {
    const BeamMeasurement* m1 = &b1;
    const BeamMeasurement* m2 = &b2;
    return (b1.axis_ == b2.axis_ &&
            b1.smoothing_elements_ == b2.smoothing_elements_ &&
            *m1 == *m2);
}

bool operator!=(const BeamProfile& p1, const BeamProfile& p2) {
    return !(p1 == p2);
}

void BeamProfile::Export(QString directory) const {
    QFile data(directory + "-profile" + "-" + AxisToString(axis_) + "-" + ".csv");
    if (!data.open(QFile::WriteOnly | QFile::Truncate)) {
        throw std::runtime_error("Could not open file");
    }
    QTextStream output(&data);
    output << "mm," << " Intensity" << "\n";
    for (int idx = 0; idx < static_cast<int>(points_.size()); ++idx) {
        output << QString::number(points_.at(idx).pos(axis_), 'f', 3) << " , "
               << QString::number(points_.at(idx).intensity(signal_noise_), 'f', 3) << "\n";
    }
}

BeamProfile BeamProfile::ScaleIntensity(double scale_to) const {
    std::vector<MeasurementPoint> scaled_points = this->GetScaledIntensity(scale_to);
    return BeamProfile(hardware_, axis_, scaled_points, timestamp_, currents_, smoothing_elements_, signal_noise_);
}

void BeamProfile::Translate(double mm) {
    points_ = this->GetTranslated(axis_, mm);
}
