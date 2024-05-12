#ifndef DOSECONSISTENCYGRAPH_H
#define DOSECONSISTENCYGRAPH_H

#include <QtCharts/QChartView>
#include <QScatterSeries>
#include <QValueAxis>
//
#include "Seance.h"

using namespace QtCharts;

class DoseConsistencyGraph : public QObject
{
Q_OBJECT
public:
    DoseConsistencyGraph(QtCharts::QChartView* chartview, bool title, bool dropShadow);
    ~DoseConsistencyGraph();
    void SetData(std::vector<Seance> seances);
    void Clear();

signals:

private:
    QChartView* chartview_;
    QScatterSeries data_;
    QValueAxis axis_x_;
    QValueAxis axis_y_;
};

#endif
