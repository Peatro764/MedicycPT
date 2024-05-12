#ifndef DATASTRUCTURES_BEAMMEASUREMENT_H
#define DATASTRUCTURES_BEAMMEASUREMENT_H

#include <vector>
#include <QPointF>
#include <QDateTime>

#include "Axis.h"
#include "Hardware.h"
#include "MeasurementPoint.h"
#include "qcustomplot.h"
#include "MeasurementCurrents.h"

class BeamMeasurement
{
public:
    BeamMeasurement();
    BeamMeasurement(QA_HARDWARE hardware);
    BeamMeasurement(QA_HARDWARE hardware, const std::vector<MeasurementPoint>& points, double noise);
    BeamMeasurement(QA_HARDWARE hardware, const std::vector<MeasurementPoint>& points,
                    QDateTime timestamp,
                    const MeasurementCurrents& currents,
                    double noise);

    void SetCurrents(MeasurementCurrents currents) { currents_ = currents; }
    void SetSignalNoise(double noise);
    double GetSignalNoise() const { return signal_noise_; }
    void SetTimestamp(QDateTime time) { timestamp_ = time; }
    MeasurementPoint Max() const;
    std::vector<MeasurementPoint> GetPoints() const { return points_; }
    void operator<<(const MeasurementPoint& p);
    void Sort(Axis axis);
    void Clear() { points_.clear(); }
    bool IsEmpty() const { return points_.empty(); }
    std::vector<MeasurementPoint> GetScaledPoints(Axis axis, double factor) const;
    std::vector<MeasurementPoint> GetScaledIntensity(double scale_to) const;
    std::vector<MeasurementPoint> GetTranslated(Axis axis, double mm) const;
    std::vector<double> GetSignalValues() const;
    std::vector<double> GetAxisValues(Axis axis) const;

    QCPCurveDataContainer GetIntensityCurve(Axis axis) const;
    QCPCurveDataContainer GetDifferentalIntensityCurve(Axis axis) const;
    QCPCurveDataContainer GetChambreCurve(Axis axis) const;
    QCPCurveDataContainer GetSignalNoiseCurve(Axis axis) const;

    QDateTime GetTimestamp() const { return timestamp_; }
    MeasurementCurrents GetCurrents() const { return currents_; }
    double GetMaxCurrentDiode() const;
    double AveragePosition(Axis axis) const;
    QA_HARDWARE Hardware() const { return hardware_; }
    void SetHardware(QA_HARDWARE hardware) { hardware_ = hardware; }

protected:
    friend bool operator==(const BeamMeasurement& b1, const BeamMeasurement& b2);
    friend bool operator!=(const BeamMeasurement& b1, const BeamMeasurement& b2);

    double PenumbraForValue(QCPCurveDataContainer curve, double max_value) const;

    QA_HARDWARE hardware_ = QA_HARDWARE::UNK;
    std::vector<MeasurementPoint> points_;
    QDateTime timestamp_;
    MeasurementCurrents currents_;
    double signal_noise_ = 0.0;
};

bool operator==(const BeamMeasurement& b1, const BeamMeasurement& b2);
bool operator!=(const BeamMeasurement& b1, const BeamMeasurement& b2);

#endif
