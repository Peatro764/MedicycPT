#ifndef MODULATEURGRAPH_H
#define MODULATEURGRAPH_H

#include <vector>

#include "qcustomplot.h"
#include "ModulateurMaker.h"

class ModulateurGraph : public QObject
{
    Q_OBJECT

public:
    ModulateurGraph(QCustomPlot* customplot);
    ~ModulateurGraph();
    void Plot(const um_plexi_weight& angles);
    void Clear();

public slots:
    void mousePress();
    void mouseWheel();

private:
    QCPCurveDataContainer MakeCircle(const double radius, const QCPCurveData& centre) const;
    QCPCurveDataContainer MakeSegment(const double radius, const QCPCurveData& centre, const double angle_spread, const double angle_centre) const;
    void ConnectSignals();
    QCustomPlot* customplot_;
};

#endif

