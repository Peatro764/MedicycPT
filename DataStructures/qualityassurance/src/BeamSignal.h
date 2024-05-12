#ifndef QUALITYASSURANCE_BEAMSIGNAL_H
#define QUALITYASSURANCE_BEAMSIGNAL_H

#include <QString>

class BeamSignal {
public:
    BeamSignal(double raw, double reference);
    BeamSignal();

    double value(double noise) const;
    double raw() const { return raw_; }
    double reference() const { return reference_; }
    QString toString() const;
    BeamSignal scale(double noise, double factor) const;

private:
    double raw_ = 0.0;
    double reference_ = 0.0;
};

bool operator==(const BeamSignal& s1, const BeamSignal& s2);
bool operator!=(const BeamSignal& s1, const BeamSignal& s2);

#endif
