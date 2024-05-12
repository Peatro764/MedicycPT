#ifndef TOTALDOSEGRAPH_H
#define TOTALDOSEGRAPH_H

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

class TotalDoseGraph
{

public:
    TotalDoseGraph(QtCharts::QChartView* chartview);
    ~TotalDoseGraph();
    void SetData(const double& del_dose, const double& total_dose);

private:
    void SetInProcess(const double& del_dose, const double& rem_dose);
    void SetOverRadiation(const double& del_dose, const double& overshoot);
    void SetInErrorCondition();
    QtCharts::QChartView* chartview_;
    QtCharts::QPieSeries dose_pie_;
    QtCharts::QPieSlice* del_slice_;
    QtCharts::QPieSlice* rem_slice_;
};

#endif //TOTALDOSEGRAPH_H

