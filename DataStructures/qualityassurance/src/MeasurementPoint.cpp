#include "MeasurementPoint.h"

MeasurementPoint::MeasurementPoint(Point point, BeamSignal signal)
    : point_(point), signal_(signal) {}

QString MeasurementPoint::toString() const {
    return this->point().toString() + " " + this->signal().toString();
}

MeasurementPoint MeasurementPoint::ScaleAxis(Axis axis, double factor) const {
    MeasurementPoint p = *this;
    p.point_.set(axis, factor * p.point_.value(axis));
    return p;
}

MeasurementPoint MeasurementPoint::TranslateAxis(Axis axis, double mm) const {
    MeasurementPoint p = *this;
    p.point_.set(axis, mm + p.point_.value(axis));
    return p;
}

MeasurementPoint MeasurementPoint::ScaleIntensity(double noise, double factor) const {
    return MeasurementPoint(this->point(), this->signal().scale(noise, factor));
}


bool operator==(const MeasurementPoint& p1, const MeasurementPoint& p2) {
    return  p1.point() == p2.point() &&
            p1.signal() == p2.signal();
}

bool operator!=(const MeasurementPoint& p1, const MeasurementPoint& p2) {
    return !(p1 == p2);
}
