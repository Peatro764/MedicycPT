#ifndef SEANCEPROGRESSGRAPH_H
#define SEANCEPROGRESSGRAPH_H

#include <QtCharts/QChartView>
#include <QLineSeries>
#include <QValueAxis>
//
#include "SeanceRecord.h"

using namespace QtCharts;

class SeanceProgressGraph : public QObject
{
Q_OBJECT
public:
    SeanceProgressGraph(QtCharts::QChartView* chartview, bool title, bool dropShadow);
    ~SeanceProgressGraph();
    void SetData(SeanceRecord record);
    void Clear();
    void SetTimeRange(double lower, double upper);
    void SetDoseRange(double lower, double upper);
    void AddData(double time, double um_del_1, double um_del_2, double i_chambre1, double i_chambre2, int cf9_status);
    void SetCurrentAsOffset();

signals:

private:
    QChartView* chartview_;
    QLineSeries um_del_series_1_;
    QLineSeries um_del_series_2_;
    QLineSeries ichambre1_series_;
    QLineSeries ichambre2_series_;
    QLineSeries cf9_status_series_;
    QValueAxis axis_x_;
    QValueAxis axis_y_dose_;
    QValueAxis axis_y_ichambre_;
    qreal um_offset_1_ = 0.0;
    qreal um_offset_2_ = 0.0;
};

#endif //SEANCEPROGRESSRAPH_H
