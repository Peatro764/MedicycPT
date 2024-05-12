#include "BraggPeakSeries.h"


BraggPeakSeries::BraggPeakSeries() {}

void BraggPeakSeries::AddPoint(QDateTime timestamp,
                               MeasurementCurrents currents,
                               double width_50, double penumbra, double parcours) {
    w50_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), width_50));
    penumbra_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), penumbra));
    parcours_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), parcours));
    currents_.push_back(currents);

    w50_values_.push_back(width_50);
    penumbra_values_.push_back(penumbra);
    parcours_values_.push_back(parcours);
}

