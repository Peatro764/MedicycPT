#ifndef COMPXYGRAPH_H
#define COMPXYGRAPH_H

#include <vector>

#include "qcustomplot.h"
#include "Compensateur.h"

class CompXYGraph : public QObject
{
    Q_OBJECT

public:
    CompXYGraph(QCustomPlot* customplot);
    ~CompXYGraph();
    void Plot(const Compensateur& compensateur);

public slots:
    void mousePress();
    void mouseWheel();

private:
    QCPCurveDataContainer MakeCircle(const double radius, const QCPCurveData& centre) const;
    void ConnectSignals();
    QCustomPlot* customplot_;
//    std::vector<QCPCurve*> compXY_curves_;
};

#endif

