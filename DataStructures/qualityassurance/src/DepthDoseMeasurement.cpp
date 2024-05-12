#include "DepthDoseMeasurement.h"

#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <math.h>
#include <QDebug>
#include <QString>

#include "QCPCurveUtils.h"
#include "Calc.h"
#include "Material.h"

DepthDoseMeasurement::DepthDoseMeasurement(QA_HARDWARE hardware) : BeamMeasurement(hardware) {}

DepthDoseMeasurement::DepthDoseMeasurement(QA_HARDWARE hardware, const std::vector<MeasurementPoint>& points, double noise) :
    BeamMeasurement(hardware, points, noise)
{
    Sort(Axis::Z);
}

DepthDoseMeasurement::DepthDoseMeasurement(QA_HARDWARE hardware, const std::vector<MeasurementPoint>& points, QDateTime timestamp,
                     MeasurementCurrents currents, int smoothing_elements, double noise)
    : BeamMeasurement(hardware, points, timestamp, currents, noise), smoothing_elements_(smoothing_elements)
{}

DepthDoseResults DepthDoseMeasurement::GetResults() const {
    try {
        QCPCurveDataContainer curve = this->GetIntensityCurve(Axis::Z);
        double penumbra_left, penumbra_right;
        double width50_left, width50_right;
        double m100_left, m100_right, m98_left, m98_right;
        if (IsBraggPeak(curve)) {
            QCPCurveData max = qcpcurveutils::MaxElement(curve);
            Penumbra(curve, max.mainValue(), &penumbra_left, &penumbra_right);
            Width50(curve, &width50_left, &width50_right);
            m100_left = max.mainKey();
            m100_right = max.mainKey();
            m98_left = qcpcurveutils::RightMostKeyForValue(curve, 0.98 * max.mainValue());
            m98_right = qcpcurveutils::RightMostKeyForValue(curve, 0.98 * max.mainValue());
            return DepthDoseResults(GetTimestamp(),
                                    Parcours(curve, max.mainValue()),
                                    penumbra_left, penumbra_right,
                                    m100_left, m100_right,
                                    m98_left, m98_right,
                                    width50_left, width50_right,
                                    true); // is bragg peak

        } else { // SOBP
            ModulationPlateau(curve, &m100_left, &m100_right, &m98_left, &m98_right);
            const double m100_right_value = qcpcurveutils::ValueAtKey(curve, m100_right);
            Penumbra(curve, m100_right_value, &penumbra_left, &penumbra_right);
            Width50(curve, &width50_left, &width50_right);
            return DepthDoseResults(GetTimestamp(),
                                    Parcours(curve, m100_right_value),
                                    penumbra_left, penumbra_right,
                                    m100_left, m100_right,
                                    m98_left, m98_right,
                                    width50_left, width50_right,
                                    false); // is bragg peak

        }
    }
    catch (std::exception& exc) {
        qDebug() << "DepthDoseMeasurement::GetResults Exception thrown " << exc.what();
        return DepthDoseResults();
    }
}

bool DepthDoseMeasurement::IsBraggPeak(QCPCurveDataContainer curve) const {
    double left, right;
    this->Width(curve, 0.75, &left, &right);
    return (right - left < 2.0);
}

void DepthDoseMeasurement::ModulationPlateau(QCPCurveDataContainer curve, double *left100, double *right100, double *left98, double *right98) const {
    QCPCurveDataContainer diff_curve = this->GetDifferentalIntensityCurve(Axis::Z);

    auto t_stat = StepDetectionTStatistic(8);
    std::vector<std::pair<int, double>> candidates = GetStepCandidates(t_stat, 5.0);
    std::sort(candidates.begin(), candidates.end(), [=](std::pair<int, double>& a, std::pair<int, double>& b) {
        return a.second > b.second;
    });

    if (candidates.empty()) {
        throw std::runtime_error("No steps detected");
    }

    if (static_cast<int>(candidates.size()) == 1) { // then probably a one sided sobp
        *left100 = diff_curve.begin()->mainKey();
        *right100 = diff_curve.at(candidates.at(0).first)->mainKey();
        *left98 = *left100;
        *right98 = qcpcurveutils::RightMostKeyForValue(curve, 0.98 * qcpcurveutils::ValueAtKey(curve, *right100));
    } else { // get the points with the two highest t-values
        int index_start = std::min(candidates.at(0).first, candidates.at(1).first);
        int index_end = std::max(candidates.at(0).first, candidates.at(1).first);
        *left100 = diff_curve.at(index_start)->mainKey();
        *right100 = diff_curve.at(index_end)->mainKey();
        *left98 = qcpcurveutils::LeftMostKeyForValue(curve, 0.98 * qcpcurveutils::ValueAtKey(curve, *left100));
        *right98 = qcpcurveutils::RightMostKeyForValue(curve, 0.98 * qcpcurveutils::ValueAtKey(curve, *right100));
    }
}

void DepthDoseMeasurement::Width50(QCPCurveDataContainer curve, double *left, double *right) const {
    this->Width(curve, 0.5, left, right);
}

void DepthDoseMeasurement::Width(QCPCurveDataContainer curve, double fraction_of_max, double *left, double *right) const {
    QCPCurveData max = qcpcurveutils::MaxElement(curve);
    *right = qcpcurveutils::RightMostKeyForValue(curve, fraction_of_max * max.mainValue());
    *left = qcpcurveutils::LeftMostKeyForValue(curve, fraction_of_max * max.mainValue());
    if (std::abs(*right - *left) < 0.001) { // -> no left flank
        *left = curve.at(0)->mainKey();
    }
}

void DepthDoseMeasurement::Penumbra(QCPCurveDataContainer curve, double shoulder_value, double *left, double *right) const {
    *left = qcpcurveutils::RightMostKeyForValue(curve, 0.9 * shoulder_value);
    *right = qcpcurveutils::RightMostKeyForValue(curve, 0.1 * shoulder_value);
}

double DepthDoseMeasurement::Parcours(QCPCurveDataContainer curve, double shoulder_value) const {
    const double parcours = qcpcurveutils::RightMostKeyForValue(curve, 0.9 * shoulder_value);
    return parcours;
}

DepthDoseMeasurement DepthDoseMeasurement::ScaleAxis(Axis axis, double factor) const {
    std::vector<MeasurementPoint> scaled_points = GetScaledPoints(axis, factor);
    return DepthDoseMeasurement(hardware_, scaled_points, timestamp_, currents_, smoothing_elements_, signal_noise_);
}

DepthDoseMeasurement DepthDoseMeasurement::ScaleIntensity(double scale_to) const {
    std::vector<MeasurementPoint> scaled_points = this->GetScaledIntensity(scale_to);
    return DepthDoseMeasurement(hardware_, scaled_points, timestamp_, currents_, smoothing_elements_, signal_noise_);
}

bool DepthDoseMeasurement::ResultsValid() const {
    try {
        DepthDoseResults results = this->GetResults();
        return (std::isfinite(results.width50()) &&
                std::isfinite(results.penumbra()) &&
                std::isfinite(results.parcours()) &&
                (results.IsBraggPeak() ||
                (std::isfinite(results.mod98()) &&
                std::isfinite(results.mod100()))));
    }
    catch (std::exception& exc) {
        qDebug() << "DepthDoseMeasurement::ResultsValid Exception thrown: " << exc.what();
        return false;
    }
}

std::vector<std::pair<int, double>> DepthDoseMeasurement::GetStepCandidates(std::vector<double> t_stat, double min_t) const {
    std::vector<std::pair<int, double>> peaks;
    // identify peaks (low-high-low)
    for (int idx = 1; idx < (static_cast<int>(t_stat.size()) - 1); ++idx) {
        if (std::abs(t_stat.at(idx)) > std::abs(t_stat.at(idx - 1)) && std::abs(t_stat.at(idx)) > std::abs(t_stat.at(idx + 1)) && t_stat.at(idx) > min_t) {
            peaks.push_back(std::make_pair(idx, t_stat.at(idx)));
        }
    }

    // remove local peaks that are smaller than primary peak
    const int min_peak_distance(20);
    std::vector<std::pair<int, double>> peaks_doubles_removed;
    for (int idx = 0; idx < static_cast<int>(peaks.size()); ++idx) {
        if (LargestLocalPeak(peaks.at(idx), peaks, min_peak_distance)) {
            peaks_doubles_removed.push_back(peaks.at(idx));
        }
    }
    return peaks_doubles_removed;
}

bool DepthDoseMeasurement::LargestLocalPeak(std::pair<int, double> peak, std::vector<std::pair<int, double>> peaks, int min_peak_distance) const {
    for (int idx = 0; idx < static_cast<int>(peaks.size()); ++idx) {
        bool not_same = peaks.at(idx).first != peak.first;
        bool neighbours = std::abs(peaks.at(idx).first - peak.first) < min_peak_distance;
        bool smaller = peaks.at(idx).second > peak.second;
        if (not_same && neighbours && smaller) { return false; }
    }
    return true;
}

std::vector<double> DepthDoseMeasurement::StepDetectionTStatistic(int sliding_window_size) const {
    QCPCurveDataContainer diff_curve = this->GetDifferentalIntensityCurve(Axis::Z);
    QCPCurveDataContainer smooth = qcpcurveutils::Smooth(diff_curve, 1);
    if (diff_curve.size() < (2*sliding_window_size + 1)) {
        qWarning() << "DepthDoseMeasurement::BreakPoints Sample size not large enough, quitting.";
        throw std::runtime_error("Cannot get break points when sliding window size is larger than number of data points");
    }

    std::vector<double> t_stat;
    for (int idx = 0; idx < diff_curve.size(); ++idx) {
        if (idx < sliding_window_size + 1 || idx > (diff_curve.size() - sliding_window_size - 1)) {
            t_stat.push_back(0);
        } else {
            std::vector<double> s1 = qcpcurveutils::ValuesInKeyRange(smooth, idx - sliding_window_size - 1, idx - 2);
            std::vector<double> s2 = qcpcurveutils::ValuesInKeyRange(smooth, idx + 2, idx + sliding_window_size + 1);
            const double t = calc::TStatistic(s1, s2);
            t_stat.push_back(t);
        }
    }
    return t_stat;
}

bool operator==(const DepthDoseMeasurement& b1, const DepthDoseMeasurement& b2) {
    const BeamMeasurement* m1 = &b1;
    const BeamMeasurement* m2 = &b2;
    return (b1.smoothing_elements_ == b2.smoothing_elements_ &&
            calc::AlmostEqual(b1.GetSignalNoise(), b2.GetSignalNoise(), 0.0001) &&
            *m1 == *m2);
}

bool operator!=(const DepthDoseMeasurement& p1, const DepthDoseMeasurement& p2) {
    return !(p1 == p2);
}

void DepthDoseMeasurement::Export(QString directory) const {
    QFile data(directory + "-DepthDoseMeasurement.csv");
    if (!data.open(QFile::WriteOnly | QFile::Truncate)) {
        throw std::runtime_error("Could not open file");
    }
    QTextStream output(&data);
    std::vector<MeasurementPoint> mmtissue = points_;
    std::vector<MeasurementPoint> mmplexi = ScaleAxis(Axis::Z, material::ToPlexiglas(1.0, material::MATERIAL::TISSUE)).GetPoints();
    if (mmtissue.size() != mmplexi.size()) {
        throw std::runtime_error("Different size vectors");
    }

    output << "mm tissue" << ", mm plexi, " << "Intensity" << "\n";
    for (int idx = 0; idx < static_cast<int>(mmtissue.size()); ++idx) {
        output << QString::number(mmtissue.at(idx).pos(Axis::Z), 'f', 3) << " , "
               << QString::number(mmplexi.at(idx).pos(Axis::Z), 'f', 3) << " , "
               << QString::number(mmtissue.at(idx).intensity(signal_noise_), 'f', 3) << "\n";
    }
}


