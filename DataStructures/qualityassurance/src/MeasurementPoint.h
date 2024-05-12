#ifndef QUALITYASSURANCE_MEASUREMENTPOINT_H
#define QUALITYASSURANCE_MEASUREMENTPOINT_H

#include "BeamSignal.h"
#include "Point.h"
#include "Axis.h"

class MeasurementPoint {
public:
    MeasurementPoint(Point point, BeamSignal signal);
    Point point() const { return point_; }
    double pos(Axis axis) const { return point_.value(axis); }
    double intensity(double noise) const { return signal_.value(noise); }
    BeamSignal signal() const { return signal_; }
    QString toString() const;
    MeasurementPoint ScaleAxis(Axis axis, double factor) const;
    MeasurementPoint TranslateAxis(Axis axis, double mm) const;
    MeasurementPoint ScaleIntensity(double noise, double factor) const;

private:
    Point point_;
    BeamSignal signal_;
};

bool operator==(const MeasurementPoint& p1, const MeasurementPoint& p2);
bool operator!=(const MeasurementPoint& p1, const MeasurementPoint& p2);

#endif
