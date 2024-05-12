#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <QtCharts/QChartView>
#include <QLineSeries>
#include <QValueAxis>
//
#include "SeanceRecord.h"

using namespace QtCharts;

class TimeSeries : public QObject
{
Q_OBJECT
public:
    TimeSeries(QtCharts::QChartView* chartview, bool title, bool dropShadow, bool legend);
    ~TimeSeries();
    void Clear();
    void SetXRange(double lower, double upper);
    void SetYRange(double lower, double upper);
    void AddData(double x, double y);

signals:

private:
    QChartView* chartview_;
    QLineSeries series_1_;
    QValueAxis axis_x_;
    QValueAxis axis_y_;
    float y_min_ = 0.0;
    float y_max_ = 0.0;
    bool first_event_ = true;
};

#endif
