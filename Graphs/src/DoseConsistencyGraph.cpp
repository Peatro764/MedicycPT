#include "DoseConsistencyGraph.h"

#include <QDebug>

DoseConsistencyGraph::DoseConsistencyGraph(QChartView *chartview, bool title, bool dropShadow)
    : chartview_(chartview) {
    qDebug() << "DoseConsistencyGraph::DoseConsistencyGraph";

    data_.setName("UM Delivrée");
    data_.setMarkerShape(QScatterSeries::MarkerShapeCircle);
    data_.setMarkerSize(15.0);
    auto chart = chartview->chart();
    if (title) {
        chart->setTitle("Consistency measured vs estimated dose");
        chart->setTitleFont(QFont("Century Schoolbook L", 16));
    }
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 10));
    chartview->setRenderHint(QPainter::Antialiasing);
    chart->setDropShadowEnabled(dropShadow);

    axis_x_.setTickCount(10);
    axis_x_.setTitleText("Seance");
    axis_x_.setTitleFont(QFont("Arial", 12));
    axis_y_.setTitleText("Deviation [SD]");
    axis_y_.setTitleFont(QFont("Arial", 12));

    chart->addSeries(&data_);

    chart->addAxis(&axis_x_, Qt::AlignBottom);
    data_.attachAxis(&axis_x_);

    chart->addAxis(&axis_y_, Qt::AlignLeft);
    data_.attachAxis(&axis_y_);

    axis_x_.setRange(0.0, 10.0);
    axis_y_.setRange(-5.0, 5.0);
    axis_y_.setTickCount(7);
}

DoseConsistencyGraph::~DoseConsistencyGraph()  {
    qDebug() << "DoseConsistencyGraph::~DoseConsistencyGraph";
    auto chart = chartview_->chart();
    chart->removeSeries(&data_);
    chart->removeAxis(&axis_x_);
    chart->removeAxis(&axis_y_);
}

void DoseConsistencyGraph::SetData(std::vector<Seance> seances) {
    Clear();
    axis_x_.setRange(0, seances.size() + 1);
    int idx(0);
    for (Seance s : seances) {
        data_.append(idx++, s.GetDoseDelivered() - s.GetDoseEstimated());
    }
}

void DoseConsistencyGraph::Clear() {
    data_.clear();
}

