#ifndef RADIATIONLEVELGRAPH_H
#define RADIATIONLEVELGRAPH_H

#include <QtCharts/QChartView>
#include <QLineSeries>
#include <QSplineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QLogValueAxis>
#include <QGraphicsRectItem>
#include <QDateTime>

using namespace QtCharts;

class RadiationLevelGraph : public QObject
{
Q_OBJECT
public:
    RadiationLevelGraph();
    RadiationLevelGraph(QtCharts::QChartView* chartview);
    ~RadiationLevelGraph();
    void Clear();
    void SetTimeRange(QDateTime lower, QDateTime upper);
    void SetDoseRange(double lower, double upper);
    void SetUpperDoseRange(double upper);
    void SetLowerDoseRange(double lower);
    void AddInstantenousData(double value);//InstantenousMeasurement m);
    void AddIntegratedData(double value);//IntegratedMeasurement m);
    void SetWarningThreshold1(double level_uSv);
    void SetWarningThreshold2(double level_uSv);
    void SetWarningThreshold3(double level_uSv);

public slots:
    void StartRectangle();
    void StopRectangle();

private slots:
    void GraphPressed(const QPointF &point);

signals:
    void TimeIntervalGiven(QDateTime start_time, QDateTime end_time);

private:
    QChartView* chartview_;
    QLineSeries instantenous_series_;
    QLineSeries integrated_series_;
    QLineSeries warning_thr1_series_;
    QLineSeries warning_thr2_series_;
    QLineSeries warning_thr3_series_;

    QDateTimeAxis axis_x_;
    QLogValueAxis axis_y_instantenous_;
    QLogValueAxis axis_y_integrated_;

    QGraphicsRectItem *measurement_indicator_ = nullptr;
    bool update_rectangle_ = false;
    qint64 rect_time_start_ = 0;
    qint64 rect_time_stop_ = 0;

    QDateTime manual_measurement_pos_;
    bool manual_measurement_pos_set_ = false;
};

#endif
