#ifndef QUALITYASSURANCE_BEAMPROFILE_H
#define QUALITYASSURANCE_BEAMPROFILE_H

#include <vector>
#include <QPointF>
#include <QDateTime>

#include "Axis.h"
#include "Hardware.h"
#include "MeasurementPoint.h"
#include "BeamMeasurement.h"
#include "BeamProfileResults.h"
#include "qcustomplot.h"

class BeamProfile : public BeamMeasurement
{
public:
    BeamProfile();
    BeamProfile(QA_HARDWARE hardware, Axis axis);
    BeamProfile(QA_HARDWARE hardware, Axis axis, const std::vector<MeasurementPoint>& points, QDateTime timestamp,
                MeasurementCurrents currents, int smoothing_elements, double noise);

    void SetAxis(Axis axis) { axis_ = axis; }
    Axis GetAxis() const { return axis_; }
    BeamProfileResults GetResults() const;
    int GetSmoothingElements() const { return smoothing_elements_; }
    void SetSmoothingElements(int elements) { smoothing_elements_ = elements; }

    double Centre() const;
    double Width(double percentage_of_max) const;
    double Penumbra() const;
    double Flatness() const;
    bool ResultsValid() const;
    void Export(QString directory) const;
    BeamProfile ScaleIntensity(double scale_to) const;
    void Translate(double mm);

private:
    friend bool operator==(const BeamProfile& b1, const BeamProfile& b2);
    friend bool operator!=(const BeamProfile& b1, const BeamProfile& b2);

    double Centre(QCPCurveDataContainer curve) const;
    double Width(QCPCurveDataContainer curve, double centre, double percentage_of_max) const;
    double LinearSlope(QCPCurveDataContainer curve, double centre, double percentage_of_max) const;

    Axis axis_;
    int smoothing_elements_ = 3;
};

bool operator==(const BeamProfile& b1, const BeamProfile& b2);
bool operator!=(const BeamProfile& b1, const BeamProfile& b2);


#endif
