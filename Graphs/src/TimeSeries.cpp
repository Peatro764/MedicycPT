#include "TimeSeries.h"

#include <QDebug>
#include <algorithm>

TimeSeries::TimeSeries(QChartView *chartview, bool title, bool dropShadow, bool legend)
    : chartview_(chartview) {
    (void)dropShadow;

    series_1_.setName("Data");

    auto chart = chartview->chart();
    chartview->setRubberBand(QChartView::RectangleRubberBand);
    QColor bg_color = QColor("#31363b");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setBackgroundBrush(QBrush(bg_color));
    chart->legend()->setLabelBrush(QBrush(Qt::white));
    if (title) {
        chart->setTitle("Evolution Intrafraction");
        chart->setTitleFont(QFont("Century Schoolbook L", 16));
    }
    chart->legend()->setVisible(legend);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 10));
    chartview->setRenderHint(QPainter::Antialiasing);
    chart->setDropShadowEnabled(false);

    axis_x_.setTickCount(10);
    axis_x_.setTitleText("Time [s]");
    axis_x_.setTitleFont(QFont("Arial", 12));
    axis_y_.setTitleText("Current [A]");
    axis_x_.setLabelsBrush(QBrush(Qt::white));
    axis_x_.setShadesBrush(QBrush(Qt::white));
    axis_x_.setTitleBrush(QBrush(Qt::white));
    axis_y_.setTitleBrush(QBrush(Qt::white));
    axis_y_.setLabelsBrush(QBrush(Qt::white));
    axis_y_.setLabelFormat("%.2e");

    chart->addSeries(&series_1_);

    chart->addAxis(&axis_x_, Qt::AlignBottom);
    series_1_.attachAxis(&axis_x_);

    chart->addAxis(&axis_y_, Qt::AlignLeft);

    series_1_.attachAxis(&axis_y_);

    axis_x_.setRange(0.0, 30.0);
    axis_y_.setRange(1e-13, 2e-13);
    axis_y_.setTickCount(7);
}

TimeSeries::~TimeSeries()  {
    auto chart = chartview_->chart();
    chart->removeSeries(&series_1_);
    chart->removeAxis(&axis_x_);
    chart->removeAxis(&axis_y_);
}

void TimeSeries::SetXRange(double lower, double upper) {
    if (upper <= lower) {
        return;
    }
    axis_x_.setRange(lower, upper);
}

void TimeSeries::SetYRange(double lower, double upper) {
    if (upper <= lower) {
        return;
    }
    axis_y_.setRange(lower, upper);
}

void TimeSeries::AddData(double x, double y) {
//    qDebug() << "TimeSeries " << x << " " << y;
    bool update_y_range = false;
    series_1_.append(x, y);

    if (x > axis_x_.max()) {
        axis_x_.setRange(0, x * 2);
    }

    if (first_event_ || y > y_max_) {
        y_max_ = y;
        update_y_range = true;
    }
    if (first_event_ || y < y_min_) {
        update_y_range = true;
        y_min_ = y;
    }
    if (update_y_range) {
        float new_upper = y_max_ >= 0.0 ? (1.1 * y_max_) : (0.9 * y_max_);
        float new_lower = y_min_ >= 0.0 ? (0.9 * y_min_) : (1.1 * y_min_);
        qDebug() << "UPDATE " << x << " " << y << " " << new_upper << " " << new_lower;
//        if (abs(new_upper - new_lower) < 1e-12) {
//            if (new_upper > 0.0) {
//                new_upper = new_upper * 3;
//            } else {
//                new_upper = new_upper * 1/3;
//            }
//            if (new_lower > 0.0) {
//                new_lower = new_lower * 1/3;
//            } else {
//                new_lower = new_lower * 3;
//            }
//        }
//        if (new_lower < 0.0) new_lower = -1 * std::max(abs(new_lower), static_cast<float>(12e-13));
//        if (new_upper > 0.0) new_upper = std::max(abs(new_upper), static_cast<float>(12e-13));
        axis_y_.setRange(new_lower, new_upper);
    }
    first_event_ = false;
}


void TimeSeries::Clear() {
    series_1_.clear();
    first_event_ = true;
}

