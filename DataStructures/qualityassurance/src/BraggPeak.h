#ifndef QUALITYASSURANCE_BRAGGPEAK_H
#define QUALITYASSURANCE_BRAGGPEAK_H

#include <vector>
#include <QPointF>
#include <QDateTime>

#include "DepthDoseMeasurement.h"

class BraggPeak
{
public:
    BraggPeak(DepthDoseMeasurement depth_dose,
              double width_50, double penumbra, double parcours);

    DepthDoseMeasurement depth_dose() const { return depth_dose_; }
    double width50() const { return width_50_; }
    double penumbra() const { return penumbra_; }
    double parcours() const { return parcours_; }

private:
    DepthDoseMeasurement depth_dose_;
    double width_50_;
    double penumbra_;
    double parcours_;
};

bool operator==(const BraggPeak& b1, const BraggPeak& b2);
bool operator!=(const BraggPeak& b1, const BraggPeak& b2);


#endif
