#include "SeanceDoseGraph.h"
#include "Calc.h"

#include <QDebug>

SeanceDoseGraph::SeanceDoseGraph(QtCharts::QChartView* chartview)
    : chartview_(chartview),
      pre_dose_set_(nullptr),
      del_dose_set_(nullptr),
      mrm_dose_set_(nullptr)
{
    qDebug() << "SeanceDoseGraph::SeanceDoseGraph";
    auto chart = chartview->chart();
    QColor bg_color = QColor("#31363b");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setBackgroundBrush(QBrush(bg_color));
    chart->legend()->setLabelBrush(QBrush(Qt::white));
    chart->setTitle("Séances");
    chart->setTitleFont(QFont("Century Schoolbook L", 16));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 10));
    chartview_->setRenderHint(QPainter::Antialiasing);
    chart->setDropShadowEnabled(false);

    chartview_->chart()->addSeries(&bar_series_);

    y_axis_.setCategories(QStringList("None"));
    y_axis_.setTitleText("");
    y_axis_.setTitleFont(QFont("Arial", 12));
    chart->setAxisY(&y_axis_, &bar_series_);

    x_axis_.setTitleText("Dose [Gy]");
    x_axis_.setTitleFont(QFont("Arial", 12));
    chart->setAxisX(&x_axis_, &bar_series_);
    x_axis_.setRange(0.0, 60.0);
    x_axis_.setLabelsBrush(QBrush(Qt::white));
    x_axis_.setShadesBrush(QBrush(Qt::white));
    y_axis_.setLabelsBrush(QBrush(Qt::white));
    y_axis_.setLabelsBrush(QBrush(Qt::white));
    x_axis_.setTitleBrush(QBrush(Qt::white));
    y_axis_.setTitleBrush(QBrush(Qt::white));

    QObject::connect(&bar_series_, SIGNAL(clicked(int,QBarSet*)), this, SIGNAL(ShowSeanceInfoReq(int)));
}

SeanceDoseGraph::~SeanceDoseGraph() {
    qDebug() << "SeanceDoseGraph::~SeanceDoseGraph";
    chartview_->chart()->removeSeries(&bar_series_);
    chartview_->chart()->removeAxis(&x_axis_);
    chartview_->chart()->removeAxis(&y_axis_);
    bar_series_.clear();
}

void SeanceDoseGraph::DisableDropShadowEffect() {
     chartview_->chart()->setDropShadowEnabled(false);
}

void SeanceDoseGraph::clear() {
    for (auto serie : chartview_->chart()->series()) {
        chartview_->chart()->removeSeries(serie);
    }
    bar_series_.clear();
}

void SeanceDoseGraph::SetData(const std::vector<Seance>& seances) {
    qDebug() << "SeanceDoseGraph::SetData";
    for (auto serie : chartview_->chart()->series()) {
        chartview_->chart()->removeSeries(serie);
    }
    bar_series_.clear(); // removes and deletes all BarSets
    mrm_dose_set_ = new QtCharts::QBarSet("Estimée");
    del_dose_set_ = new QtCharts::QBarSet("Delivrée");
    pre_dose_set_ = new QtCharts::QBarSet("Planifiée");
    QStringList seance_idx;
    for (auto idx = 0; idx < (int)seances.size(); ++idx) {
        pre_dose_set_->append(seances.at(idx).GetDosePrescribed());
        del_dose_set_->append(seances.at(idx).GetDoseDelivered());
        mrm_dose_set_->append(seances.at(idx).GetDoseEstimated());
        seance_idx << QString::number(idx + 1);
    }
    bar_series_.append(pre_dose_set_); // bar series takes ownership
    bar_series_.append(del_dose_set_); // bar series takes ownership
    bar_series_.append(mrm_dose_set_);

    y_axis_.setCategories(seance_idx);

    pre_dose_set_->setColor(QColor("cornflowerblue"));
    del_dose_set_->setColor(QColor("yellowgreen"));
    mrm_dose_set_->setColor(QRgb(0xf6a625));
    bar_series_.setBarWidth(0.75);

    chartview_->chart()->addSeries(&bar_series_);
    x_axis_.setRange(0.0f, 1.00 * MaxValue());
}

double SeanceDoseGraph::MaxValue() const {
 double max_value(0.0);
 for (auto bar_set : bar_series_.barSets()) {
     for (auto idx = 0; idx < bar_set->count(); ++idx) {
         max_value = std::max(max_value, bar_set->at(idx));
     }
 }
 return max_value;
}

void SeanceDoseGraph::AddDeliveredDose(const int& seance_idx, const double& dose) {
    if (bar_series_.count() == 0 || !del_dose_set_) {
        qWarning() << "SeanceDosGraph::AddDeliveredDose There are no bar sets in the bar series";
        return;
    }
    if (seance_idx < 0 || seance_idx > del_dose_set_->count()) {
        qWarning() << "SeanceDosGraph::AddDeliveredDose Seance idx out of bounds";
        return;
    }
    if (dose < 0.0) {
        qWarning() << "SeanceDosGraph::AddDeliveredDose I will not add a negative dose";
        return;
    }

    del_dose_set_->replace(seance_idx - 1, dose + del_dose_set_->at(seance_idx - 1));
    x_axis_.setRange(0.0, 1.05 * MaxValue());
    chartview_->chart()->removeSeries(&bar_series_);
    chartview_->chart()->addSeries(&bar_series_);
    qDebug() << "SeanceDosGraph::AddDeliveredDose Adding " << QString::number(dose, 'f', 3)
             << " to seance idx " << QString::number(seance_idx);
}

