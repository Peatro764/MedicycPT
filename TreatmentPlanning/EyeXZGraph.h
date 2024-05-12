#ifndef EYEXZGRAPH_H
#define EYEXZGRAPH_H

#include <vector>

#include "qcustomplot.h"
#include "Compensateur.h"

class EyeXZGraph : public QObject
{
    Q_OBJECT

public:
    EyeXZGraph(QCustomPlot* customplot);
    ~EyeXZGraph();
    void Plot(const Compensateur& compensateur, const double& modulation);

public slots:
    void mousePress();
    void mouseWheel();

private:
    void ConnectSignals();
    void DrawBeam(const Compensateur& compensateur, const double& modulation,
                  double max_z, QCPRange sclere_xz_range);
    void DrawEyeBall(const Eye& eye);
    void DrawSclere(const Eye& eye);
    void DrawBolus(const Eye& eye, double z, double dz, double left, double right);
    QCustomPlot* customplot_;
    QCPCurve* beamXZ_curve_;
    QCPCurve* eyeXZ_curve_;
    QCPCurve* sclereXZ_curve_;
    QCPCurve* bolusXZ_curve_;
};

#endif

