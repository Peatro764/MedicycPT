#include "SOBPSeries.h"


SOBPSeries::SOBPSeries() {}

void SOBPSeries::AddPoint(QDateTime timestamp,
                          MeasurementCurrents currents,
                          double penumbra, double parcours,
                          double modulation_98, double modulation_100) {
    penumbra_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), penumbra));
    parcours_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), parcours));
    modulation_98_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), modulation_98));
    modulation_100_.push_back(QPointF(timestamp.toMSecsSinceEpoch(), modulation_100));
    currents_.push_back(currents);

    modulation_values_.push_back(modulation_100);
    penumbra_values_.push_back(penumbra);
    parcours_values_.push_back(parcours);
}

