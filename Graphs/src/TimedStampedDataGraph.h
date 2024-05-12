#ifndef TIMEDSTAMPEDDATAGRAPH_H
#define TIMEDSTAMPEDDATAGRAPH_H

#include "TimedStampedDataSeries.h"
#include "qcustomplot.h"

class TimedStampedDataGraph : public QObject
{
    Q_OBJECT

public:
    TimedStampedDataGraph(QCustomPlot* customplot);
    ~TimedStampedDataGraph();
    void SetYAxisTitle(QString name);
    void Register(QString name);
    void SetData(QString name, const TimedStampedDataSeries& data);
    void AddPoint(QString name, double time, double value, double error);
    void RemoveAll();
    void Clear();
    void Clear(QString name);
    void SetTimeRange(QDate lower, QDate upper);

private slots:
    void mousePress();
    void mouseWheel();

private:
    bool Exists(QString name) const;
    void GenerateColors();
    QColor GetColor() const;
    void Plot();
    QCustomPlot* customplot_;
    std::map<QString, QCPGraph*> graphs_;
    std::map<QString, QCPErrorBars*> errors_;
    std::vector<QColor> colors_;
};

#endif

