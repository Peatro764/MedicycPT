#include "BeamSignal.h"

#include <QDebug>

#include "Calc.h"

BeamSignal::BeamSignal()
    : raw_(0.0), reference_(0.0) {}

BeamSignal::BeamSignal(double raw, double reference)
    : raw_(raw),
      reference_(reference) {}

QString BeamSignal::toString() const {
    return "BeamSignal(raw=" + QString::number(raw(), 'f', 2) +
            ", ref=" + QString::number(reference(), 'f', 2);
}

double BeamSignal::value(double noise) const {
    if (reference_ < 0.0001) {
        return 0.0;
    } else {
        return (raw_ - noise) / reference_;
    }
}

BeamSignal BeamSignal::scale(double noise, double factor) const {
    const double new_raw = factor * (raw_ - noise) + noise;
    return BeamSignal(new_raw, reference_);
}

bool operator==(const BeamSignal& s1, const BeamSignal& s2) {
    return  calc::AlmostEqual(s1.raw(), s2.raw(), 0.0001) &&
            calc::AlmostEqual(s1.reference(), s2.reference(), 0.0001);
}

bool operator!=(const BeamSignal& s1, const BeamSignal& s2) {
    return !(s1 == s2);
}
