#ifndef QUALITYASSURANCE_BEAMPROFILESERIES_H
#define QUALITYASSURANCE_BEAMPROFILESERIES_H

#include <QVector>
#include <QPointF>
#include <QDateTime>

#include "Axis.h"
#include "BeamProfileResults.h"
#include "MeasurementCurrents.h"

class BeamProfileSeries
{
public:
    BeamProfileSeries(Axis axis);
    Axis axis() const { return axis_; }
    QVector<QPointF> w90() const { return w90_; }
    QVector<QPointF> w95() const { return w95_; }
    QVector<QPointF> penumbra() const { return penumbra_; }
    QVector<QPointF> flatness() const { return flatness_; }
    std::vector<double> flatnessvalues() const { return flatness_values_; }
    std::vector<double> penumbravalues() const { return penumbra_values_; }
    std::vector<double> w90values() const { return w90_values_; }

    QVector<MeasurementCurrents> currents() const { return currents_; }
    int N() const { return w90_.size(); }

    void AddPoint(QDateTime timestamp,
                  MeasurementCurrents currents,
                  double width_95, double width_90, double penumbra, double flatness);

private:
    Axis axis_;
    QVector<QPointF> w90_;
    QVector<QPointF> w95_;
    QVector<QPointF> penumbra_;
    QVector<QPointF> flatness_;
    std::vector<double> w90_values_;
    std::vector<double> penumbra_values_;
    std::vector<double> flatness_values_;
    QVector<MeasurementCurrents> currents_;
};

#endif
