#include "BeamProfileSeries.h"


BeamProfileSeries::BeamProfileSeries(Axis axis)
    : axis_(axis) {}

void BeamProfileSeries::AddPoint(QDateTime timestamp,
                                 MeasurementCurrents currents,
                                 double width_95, double width_90, double penumbra, double flatness) {
    w90_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), width_90));
    w95_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), width_95));
    penumbra_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), penumbra));
    flatness_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), flatness));
    currents_.push_back(currents);
    flatness_values_.push_back(flatness);
    penumbra_values_.push_back(penumbra);
    w90_values_.push_back(width_90);
}

