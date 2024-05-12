#ifndef COMPXZGRAPH_H
#define COMPXZGRAPH_H

#include <vector>

#include "qcustomplot.h"
#include "Compensateur.h"

class CompXZGraph : public QObject
{
    Q_OBJECT

public:
    CompXZGraph(QCustomPlot* customplot);
    ~CompXZGraph();
    void Plot(const Compensateur& compensateur);

public slots:
    void mousePress();
    void mouseWheel();

private:
    void ConnectSignals();
    QCustomPlot* customplot_;
    QCPCurve* compXZ_step_curve_;
    QCPCurve* compXZ_theo_curve_;
};

#endif

