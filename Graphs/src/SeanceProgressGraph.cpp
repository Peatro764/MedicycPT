#include "SeanceProgressGraph.h"

#include <QDebug>

SeanceProgressGraph::SeanceProgressGraph(QChartView *chartview, bool title, bool dropShadow)
    : chartview_(chartview) {
    (void)dropShadow;
    qDebug() << "SeanceProgressGraph::SeanceProgressGraph";

    um_del_series_1_.setName("UM1 Delivrée");
    um_del_series_2_.setName("UM2 Delivrée");
    ichambre1_series_.setName("I Chambre 1");
    ichambre2_series_.setName("I Chambre 2");
    cf9_status_series_.setName("Etat CF9");

    auto chart = chartview->chart();
    QColor bg_color = QColor("#31363b");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setBackgroundBrush(QBrush(bg_color));
    chart->legend()->setLabelBrush(QBrush(Qt::white));
    if (title) {
        chart->setTitle("Evolution Intrafraction");
        chart->setTitleFont(QFont("Century Schoolbook L", 16));
    }
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 10));
    chartview->setRenderHint(QPainter::Antialiasing);
    chart->setDropShadowEnabled(false);

    axis_x_.setTickCount(10);
    axis_x_.setTitleText("Durée [s]");
    axis_x_.setTitleFont(QFont("Arial", 12));
    axis_y_dose_.setTitleText("Dose [UM]");
    axis_y_dose_.setTitleFont(QFont("Arial", 12));
    axis_y_ichambre_.setTitleText("I.Chambre [nA]");
    axis_y_ichambre_.setTitleFont(QFont("Arial", 12));
    axis_x_.setLabelsBrush(QBrush(Qt::white));
    axis_x_.setShadesBrush(QBrush(Qt::white));
    axis_x_.setTitleBrush(QBrush(Qt::white));
    axis_y_dose_.setTitleBrush(QBrush(Qt::white));
    axis_y_ichambre_.setTitleBrush(QBrush(Qt::white));
    axis_y_dose_.setLabelsBrush(QBrush(Qt::white));
    axis_y_dose_.setLabelsBrush(QBrush(Qt::white));
    axis_y_ichambre_.setLabelsBrush(QBrush(Qt::white));
    axis_y_ichambre_.setLabelsBrush(QBrush(Qt::white));

    chart->addSeries(&um_del_series_1_);
    chart->addSeries(&um_del_series_2_);
    chart->addSeries(&ichambre1_series_);
    chart->addSeries(&ichambre2_series_);
    chart->addSeries(&cf9_status_series_);

    chart->addAxis(&axis_x_, Qt::AlignBottom);
    um_del_series_1_.attachAxis(&axis_x_);
    um_del_series_2_.attachAxis(&axis_x_);
    ichambre1_series_.attachAxis(&axis_x_);
    ichambre2_series_.attachAxis(&axis_x_);
    cf9_status_series_.attachAxis(&axis_x_);

    chart->addAxis(&axis_y_ichambre_, Qt::AlignLeft);
    ichambre1_series_.attachAxis(&axis_y_ichambre_);
    ichambre2_series_.attachAxis(&axis_y_ichambre_);

    chart->addAxis(&axis_y_dose_, Qt::AlignRight);
    um_del_series_1_.attachAxis(&axis_y_dose_);
    um_del_series_2_.attachAxis(&axis_y_dose_);
    cf9_status_series_.attachAxis(&axis_y_ichambre_);

    axis_x_.setRange(0.0, 10.0);
    axis_y_dose_.setRange(0.0, 1500.0);
    axis_y_ichambre_.setRange(0.0, 600.0);
    axis_y_dose_.setTickCount(7);
    axis_y_ichambre_.setTickCount(7);
}

SeanceProgressGraph::~SeanceProgressGraph()  {
    qDebug() << "SeanceProgressGraph::~SeanceProgressGraph";
    auto chart = chartview_->chart();
    chart->removeSeries(&um_del_series_1_);
    chart->removeSeries(&um_del_series_2_);
    chart->removeSeries(&ichambre1_series_);
    chart->removeSeries(&ichambre2_series_);
    chart->removeSeries(&cf9_status_series_);
    chart->removeAxis(&axis_x_);
    chart->removeAxis(&axis_y_dose_);
    chart->removeAxis(&axis_y_ichambre_);
}

void SeanceProgressGraph::SetTimeRange(double lower, double upper) {
    if (upper <= lower) {
        qWarning() << "SeanceProgressGraph::SetTimeRange Upper limit equal or smaller than lower limit";
        return;
    }
    axis_x_.setRange(lower, upper);
}

void SeanceProgressGraph::SetDoseRange(double lower, double upper) {
    if (upper <= lower) {
        qWarning() << "SeanceProgressGraph::SetDoseRange Upper limit equal or smaller than lower limit";
        return;
    }
    axis_y_dose_.setRange(lower, upper);
}

void SeanceProgressGraph::AddData(double time, double um_del_1, double um_del_2, double i_chambre1, double i_chambre2, int cf9_status) {
    um_del_series_1_.append(time, um_del_1 + um_offset_1_);
    um_del_series_2_.append(time, um_del_2 + um_offset_2_);
    ichambre1_series_.append(time, i_chambre1);
    ichambre2_series_.append(time, i_chambre2);
    cf9_status_series_.append(time, 100.0 * static_cast<double>(1 - cf9_status)); // cf9_status = 0, CF9 is open

    if (time > axis_x_.max()) {
        qDebug() << "SeanceProgressGraph::AddData Expanding time range to accomodate new data";
        axis_x_.setRange(axis_x_.min(), time * 1.25);
    }

    if (um_del_1 > axis_y_dose_.max()) {
        axis_y_dose_.setRange(0.0, 1.25 * um_del_1);
    }
}

void SeanceProgressGraph::SetCurrentAsOffset() {
    if (um_del_series_1_.count() > 0) {
        um_offset_1_ = um_del_series_1_.at(um_del_series_1_.count() - 1).y();
    } else {
        um_offset_1_ = 0.0;
    }
    if (um_del_series_2_.count() > 0) {
        um_offset_2_ = um_del_series_2_.at(um_del_series_2_.count() - 1).y();
    } else {
        um_offset_2_ = 0.0;
    }
}

void SeanceProgressGraph::SetData(SeanceRecord record) {
    qDebug() << "SeanceProgressGraph::SetData";
    Clear();

    auto time = record.GetDuration();
    auto dose1 = record.GetUM1Delivered();
    auto dose2 = record.GetUM2Delivered();
    auto ich1 = record.GetIChambre1();
    auto ich2 = record.GetIChambre2();
    auto cf9_status = record.GetCF9Status();

    if (time.empty() || dose1.empty() || dose2.empty() || ich1.empty() || ich2.empty() || cf9_status.empty()) {
        qWarning() << "SeanceProgressGraph::SetData Empty vector";
        return;
    }

    for (auto idx = 0; idx < (int)std::min(time.size(), dose1.size()); ++idx) {
        um_del_series_1_.append(time.at(idx), dose1.at(idx));
    }

    for (auto idx = 0; idx < (int)std::min(time.size(), dose2.size()); ++idx) {
        um_del_series_2_.append(time.at(idx), dose2.at(idx));
    }

    for (auto idx = 0; idx < (int)std::min(time.size(), ich1.size()); ++idx) {
        ichambre1_series_.append(time.at(idx), ich1.at(idx));
    }

    for (auto idx = 0; idx < (int)std::min(time.size(), ich2.size()); ++idx) {
        ichambre2_series_.append(time.at(idx), ich2.at(idx));
    }

    for (auto idx = 0; idx < (int)std::min(time.size(), cf9_status.size()); ++idx) {
        cf9_status_series_.append(time.at(idx), 100.0*static_cast<double>(1.0 - cf9_status.at(idx)));
    }

    axis_x_.setRange(0.0, time.back());
    auto dose_max = 1.05 * std::max(record.GetUMPrevu(), *std::max_element(dose1.begin(), dose1.end()));
    axis_y_dose_.setRange(0.0, dose_max);
}

void SeanceProgressGraph::Clear() {
    um_offset_1_ = 0.0;
    um_offset_2_ = 0.0;
    um_del_series_1_.clear();
    um_del_series_2_.clear();
    ichambre1_series_.clear();
    ichambre2_series_.clear();
    cf9_status_series_.clear();
}

