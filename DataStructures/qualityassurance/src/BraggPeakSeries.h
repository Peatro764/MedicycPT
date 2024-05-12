#ifndef QUALITYASSURANCE_BraggPeakSeries_H
#define QUALITYASSURANCE_BraggPeakSeries_H

#include <QVector>
#include <QPointF>
#include <QDateTime>
#include <vector>

#include "MeasurementCurrents.h"

class BraggPeakSeries
{
public:
    BraggPeakSeries();
    QVector<QPointF> w50() const { return w50_; }
    QVector<QPointF> penumbra() const { return penumbra_; }
    QVector<QPointF> parcours() const { return parcours_; }
    std::vector<double> w50values() const { return w50_values_; }
    std::vector<double> penumbravalues() const { return penumbra_values_; }
    std::vector<double> parcoursvalues() const { return parcours_values_; }

    QVector<MeasurementCurrents> currents() const { return currents_; }
    int N() const { return w50_.size(); }

    void AddPoint(QDateTime timestamp,
                  MeasurementCurrents currents,
                  double width_50, double penumbra, double parcours);

private:
    QVector<QPointF> w50_;
    QVector<QPointF> penumbra_;
    QVector<QPointF> parcours_;
    QVector<MeasurementCurrents> currents_;

    std::vector<double> w50_values_;
    std::vector<double> penumbra_values_;
    std::vector<double> parcours_values_;
};

#endif
