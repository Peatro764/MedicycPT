#ifndef PROTONTHERAPIE_TIMEDSTAMPEDDATASERIES_H
#define PROTONTHERAPIE_TIMEDSTAMPEDDATASERIES_H

#include <QDateTime>
#include <QVector>

struct TimedStampedDataSeries {
    QVector<double> time;
    QVector<double> value;
    QVector<double> error;
};

#endif
