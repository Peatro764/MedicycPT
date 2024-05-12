#include "BraggPeak.h"

#include <stdexcept>
#include <QDebug>
#include <QString>

#include "Calc.h"

BraggPeak::BraggPeak(DepthDoseMeasurement depth_dose, double width_50, double penumbra, double parcours)
    : depth_dose_(depth_dose), width_50_(width_50), penumbra_(penumbra), parcours_(parcours) {}

bool operator==(const BraggPeak& b1, const BraggPeak& b2) {
    return (b1.depth_dose() == b2.depth_dose() &&
            calc::AlmostEqual(b1.parcours(), b2.parcours(), 0.0001) &&
            calc::AlmostEqual(b1.penumbra(), b2.penumbra(), 0.0001) &&
            calc::AlmostEqual(b1.width50(), b2.width50(), 0.0001));
}

bool operator!=(const BraggPeak& b1, const BraggPeak& b2) {
    return !(b1 == b2);
}
