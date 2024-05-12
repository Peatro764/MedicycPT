#ifndef QUALITYASSURANCE_TimeSeriesGraph_H
#define QUALITYASSURANCE_TimeSeriesGraph_H

#include <QtCharts/QChartView>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QDateTime>
#include <map>
#include <memory>

#include "BeamProfileResults.h"
#include "Axis.h"

using namespace QtCharts;

class TimeSeriesGraph : public QObject
{
Q_OBJECT
public:
    TimeSeriesGraph(QtCharts::QChartView* chartview);
    ~TimeSeriesGraph();
    void CreateNewSerie(Axis axis);
    void AddPoint(Axis axis, QDateTime timestamp, double value);
    void SetPoints(Axis axis, QVector<QPointF> points);
    void Clear(Axis axis);
    void Clear();
    void SetTimeRange(QDateTime lower, QDateTime upper);
    void SetValueRange(double lower, double upper);

private slots:
    void HandleClickedPoint(const QPointF& point);
    void AdjustAxes();

signals:
    void PointClicked(Axis axis, QDateTime timestamp);

private:
    QChartView* chartview_;
    QDateTimeAxis axis_time_;
    QValueAxis axis_value_;
    std::map<Axis, std::shared_ptr<QScatterSeries>> scatter_series_;
    bool axis_init_ = false;
};

#endif
