#include "TimeSeriesGraph.h"

#include <QDebug>

TimeSeriesGraph::TimeSeriesGraph(QChartView *chartview)
    : chartview_(chartview) {

    auto chart = chartview->chart();

    QColor bg_color = QColor("#31363b");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setBackgroundBrush(QBrush(bg_color));
    chart->legend()->setLabelBrush(QBrush(Qt::white));
    axis_time_.setLabelsBrush(QBrush(Qt::white));
    axis_time_.setShadesBrush(QBrush(Qt::white));
    axis_value_.setLabelsBrush(QBrush(Qt::white));
    axis_value_.setLabelsBrush(QBrush(Qt::white));

    chart->legend()->setVisible(true);
    chart->legend()->setFont(QFont("Arial", 9));
    chart->legend()->detachFromChart();
    chart->legend()->setPos(100, 40);
    chart->legend()->setGeometry(QRectF(100.0, 40.0, 100.0, 20.0));
    chart->legend()->update();
    chartview->setRenderHint(QPainter::Antialiasing);
    chart->setDropShadowEnabled(false);
    chartview_->setRubberBand(QChartView::RectangleRubberBand);

    axis_time_.setTickCount(10);
    axis_time_.setFormat(QString("MMM dd"));
    axis_value_.setTitleText("[mm tissue]");
    axis_value_.setTitleFont(QFont("Arial", 12));
    chart->addAxis(&axis_time_, Qt::AlignBottom);
    chart->addAxis(&axis_value_, Qt::AlignLeft);
    axis_value_.setRange(0.0, 0.01);
}

void TimeSeriesGraph::CreateNewSerie(Axis axis) {
    scatter_series_[axis] = std::shared_ptr<QScatterSeries>(new QScatterSeries());
    scatter_series_[axis]->setName(AxisToString(axis));
    scatter_series_[axis]->setMarkerShape(static_cast<QScatterSeries::MarkerShape>(static_cast<int>(axis) / 2));
    scatter_series_[axis]->setMarkerSize(10.0);

    auto chart = chartview_->chart();
    chart->addSeries(scatter_series_[axis].get());
    scatter_series_[axis]->attachAxis(&axis_time_);
    scatter_series_[axis]->attachAxis(&axis_value_);

    scatter_series_[axis]->setVisible(true);
    QObject::connect(scatter_series_[axis].get(), &QScatterSeries::clicked, this, [=](const QPointF& point) { emit PointClicked(axis, QDateTime::fromMSecsSinceEpoch(point.x())); });
}


void TimeSeriesGraph::HandleClickedPoint(const QPointF& point) {
    qDebug() << "POINT " << QDateTime::fromMSecsSinceEpoch(point.x()) << " " << point.y();
}

TimeSeriesGraph::~TimeSeriesGraph()  {
    qDebug() << "TimeSeriesGraph::~TimeSeriesGraph";
    auto chart = chartview_->chart();
    for (auto const& s : scatter_series_) {
        chart->removeSeries(s.second.get());
    }
    chart->removeAxis(&axis_time_);
    chart->removeAxis(&axis_value_);
}

void TimeSeriesGraph::SetTimeRange(QDateTime lower, QDateTime upper) {
    if (upper <= lower) {
        qWarning() << "TimeSeriesGraph::SetTimeRange Upper limit equal or smaller than lower limit";
        return;
    }
    axis_time_.setRange(lower, upper);
}

void TimeSeriesGraph::SetValueRange(double lower, double upper) {
    if (upper <= lower) {
        qWarning() << "TimeSeriesGraph::SetValueRange Upper limit equal or smaller than lower limit";
        return;
    }
    axis_value_.setRange(lower, upper);
}

void TimeSeriesGraph::AddPoint(Axis axis, QDateTime timestamp, double value) {
    if (scatter_series_.find(axis) == scatter_series_.end()) {
        qWarning() << "TimeSeriesGraph::AddPoint Series does not exist";
    } else {
        scatter_series_[axis]->append(timestamp.toMSecsSinceEpoch(), value);
        if (timestamp > axis_time_.max()) {
            qDebug() << "TimeSeriesGraph::AddData Expanding time range to accomodate new data";
            axis_time_.setRange(axis_time_.min(), QDateTime::fromMSecsSinceEpoch(timestamp.toMSecsSinceEpoch() * 1.25));
        }
        if (axis_init_) {
            if (value > axis_value_.max()) {
                axis_value_.setRange(axis_value_.min(), value + 0.15);
            }
            if (value < axis_value_.min()) {
                axis_value_.setRange(value - 0.15, axis_value_.max());
            }
        } else {
            axis_value_.setRange(value - 0.15, value + 0.15);
            axis_init_ = true;
        }
    }
}

void TimeSeriesGraph::SetPoints(Axis axis, QVector<QPointF> points) {
    if (scatter_series_.find(axis) == scatter_series_.end()) {
        qWarning() << "TimeSeriesGraph::AddPoint Series does not exist";
    } else {
        scatter_series_[axis]->replace(points);
        chartview_->chart()->legend()->setGeometry(QRectF(100.0, 40.0, 100.0, 60.0));
        AdjustAxes();
        chartview_->chart()->legend()->update();
        chartview_->repaint();
    }
}

void TimeSeriesGraph::Clear(Axis axis) {
    if (scatter_series_.find(axis) == scatter_series_.end()) {
        qWarning() << "TimeSeriesGraph::AddPoint Series does not exist";
    } else {
        scatter_series_[axis]->clear();
    }
}

void TimeSeriesGraph::Clear() {
    for (auto const& s : scatter_series_) {
        s.second->clear();
    }
}

void TimeSeriesGraph::AdjustAxes() {
    QList<QPointF> points;
    for (auto const& s : scatter_series_) {
        points.append(s.second->points());
    }
    if (points.empty()) {
        return;
    }
    double min = points.at(0).y();
    double max = min;
    for (const QPointF &p : points) {
        min = std::min(min, p.y());
        max = std::max(max, p.y());
    }
    min = min - 0.15;
    max = max + 0.15;
    axis_value_.setRange(min, max);
}
