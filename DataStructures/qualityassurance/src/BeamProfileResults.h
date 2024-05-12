#ifndef QUALITYASSURANCE_BEAMPROFILERESULTS_H
#define QUALITYASSURANCE_BEAMPROFILERESULTS_H

#include <QDateTime>

#include "Axis.h"

class BeamProfileResults {
public:
    BeamProfileResults(Axis axis, QDateTime timestamp, double centre, double width_95, double width_90, double width_50, double penumbra, double flatness)
        : axis_(axis),
          timestamp_(timestamp),
          centre_(centre),
          width_95_(width_95),
          width_90_(width_90),
          width_50_(width_50),
          penumbra_(penumbra),
          flatness_(flatness) {}
    Axis axis() const { return axis_; }
    QDateTime timestamp() const { return timestamp_; }
    double centre() const { return centre_; }
    double width_95() const { return width_95_; }
    double width_90() const { return width_90_; }
    double width_50() const { return width_50_; }
    double penumbra() const { return penumbra_; }
    double flatness() const { return flatness_; }

private:
    Axis axis_;
    QDateTime timestamp_;
    double centre_;
    double width_95_;
    double width_90_;
    double width_50_;
    double penumbra_;
    double flatness_;
};


#endif
