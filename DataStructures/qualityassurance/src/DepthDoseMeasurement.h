#ifndef QUALITYASSURANCE_DepthDoseMeasurementMEASUREMENT_H
#define QUALITYASSURANCE_DepthDoseMeasurementMEASUREMENT_H

#include <vector>
#include <QPointF>
#include <QDateTime>

#include "DepthDoseResults.h"
#include "MeasurementPoint.h"
#include "BeamMeasurement.h"
#include "qcustomplot.h"

class DepthDoseMeasurement : public BeamMeasurement
{
public:
    DepthDoseMeasurement(QA_HARDWARE hardware);
    DepthDoseMeasurement(QA_HARDWARE hardware, const std::vector<MeasurementPoint>& points, double noise);
    DepthDoseMeasurement(QA_HARDWARE hardware, const std::vector<MeasurementPoint>& points, QDateTime timestamp,
              MeasurementCurrents currents, int smoothing_elements, double noise);

    DepthDoseResults GetResults() const;
    int GetSmoothingElements() const { return smoothing_elements_; }
    void SetSmoothingElements(int elements) { smoothing_elements_ = elements; }
    DepthDoseMeasurement ScaleAxis(Axis axis, double factor) const;
    DepthDoseMeasurement ScaleIntensity(double scale_to) const;
    void Export(QString directory) const;
    bool ResultsValid() const;

private:
    friend bool operator==(const DepthDoseMeasurement& b1, const DepthDoseMeasurement& b2);
    friend bool operator!=(const DepthDoseMeasurement& b1, const DepthDoseMeasurement& b2);

    void ModulationPlateau(QCPCurveDataContainer curve, double *left100, double *right100, double *left98, double *right98) const;
    void Width(QCPCurveDataContainer curve, double fraction_of_max, double *left, double *right) const;
    void Width50(QCPCurveDataContainer curve, double *left, double *right) const;
    void Penumbra(QCPCurveDataContainer curve, double shoulder_value, double *left, double *right) const;
    double Parcours(QCPCurveDataContainer curve, double shoulder_value) const;

    bool IsBraggPeak(QCPCurveDataContainer curve) const;

    std::vector<std::pair<int, double>> GetStepCandidates(std::vector<double> t_stat, double min_t) const;
    bool LargestLocalPeak(std::pair<int, double> peak, std::vector<std::pair<int, double>> peaks, int min_peak_distance) const;
    std::vector<double> StepDetectionTStatistic(int sliding_window_size) const;

    int smoothing_elements_ = 0;
};

bool operator==(const DepthDoseMeasurement& b1, const DepthDoseMeasurement& b2);
bool operator!=(const DepthDoseMeasurement& b1, const DepthDoseMeasurement& b2);


#endif
