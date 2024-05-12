#ifndef SEANCEDOSEGRAPH_H
#define SEANCEDOSEGRAPH_H

#include <QObject>
#include <QtCharts/QChartView>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

#include "Seance.h"

using namespace QtCharts;

class SeanceDoseGraph : public QObject
{
Q_OBJECT
public:
    SeanceDoseGraph(QtCharts::QChartView* chartview);
    ~SeanceDoseGraph();
    void SetData(const std::vector<Seance>& seances);
    void AddDeliveredDose(const int& seance_idx, const double& dose);
    void DisableDropShadowEffect();
    void clear();

signals:
    void ShowSeanceInfoReq(int seance_idx);

private:
    double MaxValue() const;
    QStringList categories_;
    QtCharts::QChartView* chartview_;
    QtCharts::QHorizontalBarSeries bar_series_;
    QtCharts::QBarSet* pre_dose_set_;
    QtCharts::QBarSet* del_dose_set_;
    QtCharts::QBarSet* mrm_dose_set_;
    QtCharts::QBarCategoryAxis y_axis_;
    QtCharts::QValueAxis x_axis_;
};

#endif //SEANCEDOSEGRAPH_H

