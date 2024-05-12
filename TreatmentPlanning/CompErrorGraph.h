#ifndef COMPERRORGRAPH_H
#define COMPERRORGRAPH_H

#include <vector>

#include "qcustomplot.h"
#include "Compensateur.h"

class CompErrorGraph : public QObject
{
    Q_OBJECT

public:
    CompErrorGraph(QCustomPlot* customplot);
    ~CompErrorGraph();
    void Plot(const Compensateur& compensateur);

public slots:
    void mousePress();
    void mouseWheel();

private:
    void ConnectSignals();
    QCustomPlot* customplot_;
    QCPBars* compError_bars_;
};

#endif

