#ifndef QUALITYASSURANCE_SOBPSeries_H
#define QUALITYASSURANCE_SOBPSeries_H

#include <QVector>
#include <QPointF>
#include <QDateTime>

#include "SOBP.h"
#include "MeasurementCurrents.h"

class SOBPSeries
{
public:
    SOBPSeries();
    QVector<QPointF> penumbra() const { return penumbra_; }
    QVector<QPointF> parcours() const { return parcours_; }
    QVector<QPointF> modulation98() const { return modulation_98_; }
    QVector<QPointF> modulation100() const { return modulation_100_; }
    std::vector<double> modulationvalues() const { return modulation_values_; }
    std::vector<double> penumbravalues() const { return penumbra_values_; }
    std::vector<double> parcoursvalues() const { return parcours_values_; }

    QVector<MeasurementCurrents> currents() const { return currents_; }
    int N() const { return penumbra_.size(); }

    void AddPoint(QDateTime timestamp,
                  MeasurementCurrents currents,
                  double penumbra, double parcours,
                  double modulation_98, double modulation_100);

private:
    QVector<QPointF> penumbra_;
    QVector<QPointF> parcours_;
    QVector<QPointF> modulation_98_;
    QVector<QPointF> modulation_100_;
    QVector<MeasurementCurrents> currents_;

    std::vector<double> modulation_values_;
    std::vector<double> penumbra_values_;
    std::vector<double> parcours_values_;
};

#endif
