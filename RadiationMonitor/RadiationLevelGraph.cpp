#include "RadiationLevelGraph.h"

#include <QDebug>

RadiationLevelGraph::RadiationLevelGraph() {}

RadiationLevelGraph::RadiationLevelGraph(QChartView *chartview)
    : chartview_(chartview) {

    instantenous_series_.setName("Instantenous Radiation Level");
    integrated_series_.setName("Integrated Radiation Level");

    //chartview->setRubberBand(QChartView::RectangleRubberBand);

    QPen rad_pen;
    rad_pen.setColor(QRgb(0x209fdf));
    rad_pen.setWidth(3);
    instantenous_series_.setPen(rad_pen);

    QPen integrated_pen;
    integrated_pen.setColor(QRgb(0x99ca53));
    integrated_pen.setWidth(3);
    integrated_series_.setPen(integrated_pen);

    auto chart = chartview_->chart();
    QColor bg_color = QColor("#31363b");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setBackgroundBrush(QBrush(bg_color));
    chart->legend()->setVisible(false);
    chartview->setRenderHint(QPainter::Antialiasing);
    chart->setDropShadowEnabled(false);

    axis_x_.setTickCount(3);
    axis_x_.setFormat("mm:ss");

    axis_y_instantenous_.setLabelsFont(QFont("Arial", 8));
    axis_y_instantenous_.setTitleText("Débit [mSv/h]");
    axis_y_instantenous_.setTitleFont(QFont("Arial", 10));
    axis_y_integrated_.setTitleText("Charge intégrée [pC]");
    axis_y_integrated_.setTitleFont(QFont("Arial", 10));
    axis_y_integrated_.setLabelsFont(QFont("Arial", 8));
    axis_y_instantenous_.setLabelFormat(QString("%2.1E"));
    axis_y_integrated_.setLabelFormat(QString("%2.1E"));

    axis_y_integrated_.setGridLineVisible(false);
    axis_y_instantenous_.setGridLineVisible(true);

    axis_x_.setTitleBrush(QBrush(Qt::white));
    axis_x_.setLabelsBrush(QBrush(Qt::white));
    axis_x_.setShadesBrush(QBrush(Qt::white));
    axis_y_instantenous_.setTitleBrush(QBrush(Qt::white));
    axis_y_instantenous_.setLabelsBrush(QBrush(Qt::white));
    axis_y_instantenous_.setShadesBrush(QBrush(Qt::white));
    axis_y_integrated_.setTitleBrush(QBrush(Qt::white));
    axis_y_integrated_.setLabelsBrush(QBrush(Qt::white));
    axis_y_integrated_.setShadesBrush(QBrush(Qt::white));

    chart->addSeries(&instantenous_series_);
    chart->addSeries(&integrated_series_);

    chart->addAxis(&axis_x_, Qt::AlignBottom);
    instantenous_series_.attachAxis(&axis_x_);
    integrated_series_.attachAxis(&axis_x_);

    chart->addAxis(&axis_y_instantenous_, Qt::AlignLeft);
    chart->addAxis(&axis_y_integrated_, Qt::AlignRight);
    instantenous_series_.attachAxis(&axis_y_instantenous_);
    integrated_series_.attachAxis(&axis_y_integrated_);

    axis_x_.setRange(QDateTime(QDate(1970, 1, 1)), QDateTime(QDate(1970, 1, 2)));
//    axis_y_instantenous_.setRange(1.0e-4, 1.0e1);
    axis_y_instantenous_.setRange(1e-3, 1.0e1);
    axis_y_instantenous_.setBase(10);
    axis_y_integrated_.setRange(1.0e-4, 1.0e4);
    axis_y_integrated_.setBase(10);

    chartview_->repaint();

    QObject::connect(&instantenous_series_, &QLineSeries::pressed, this, &RadiationLevelGraph::GraphPressed);
}

void RadiationLevelGraph::GraphPressed(const QPointF &point) {
    if (manual_measurement_pos_set_) {
        manual_measurement_pos_set_ = false;
        QDateTime stop_time = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(point.x()));
        emit(TimeIntervalGiven(manual_measurement_pos_, stop_time));
    } else {
        manual_measurement_pos_ = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(point.x()));
        manual_measurement_pos_set_ = true;
    }
}

RadiationLevelGraph::~RadiationLevelGraph()  {
    auto chart = chartview_->chart();
    chart->removeSeries(&instantenous_series_);
    chart->removeSeries(&integrated_series_);
    chart->removeAxis(&axis_x_);
    chart->removeAxis(&axis_y_instantenous_);
    chart->removeAxis(&axis_y_integrated_);
}

void RadiationLevelGraph::SetWarningThreshold1(double level_uSv) {
    (void)level_uSv;
}

void RadiationLevelGraph::SetWarningThreshold2(double level_uSv) {
    (void)level_uSv;
}

void RadiationLevelGraph::SetWarningThreshold3(double level_uSv) {
    (void)level_uSv;
}

void RadiationLevelGraph::SetTimeRange(QDateTime lower, QDateTime upper) {
    if (upper <= lower) {
        qWarning() << "RadiationLevelGraph::SetTimeRange Upper limit equal or smaller than lower limit";
        return;
    }
    axis_x_.setRange(lower, upper);
}

void RadiationLevelGraph::SetLowerDoseRange(double lower) {
    axis_y_instantenous_.setMin(lower*1000.0);
}

void RadiationLevelGraph::SetUpperDoseRange(double upper) {
    axis_y_instantenous_.setMax(upper*1000.0);
}

void RadiationLevelGraph::SetDoseRange(double lower, double upper) {
    if (upper <= lower) {
        qWarning() << "RadiationLevelGraph::SetDoseRange Upper limit equal or smaller than lower limit";
        return;
    }
    axis_y_instantenous_.setRange(lower*1000.0, upper*1000.0);
}

void RadiationLevelGraph::AddInstantenousData(double value) {
    QDateTime modified_timestamp = QDateTime::currentDateTime();
    axis_x_.setRange(modified_timestamp.addSecs(-120), modified_timestamp);

//    const double dose = std::max(m.conversed_value()*1000.0, 1.0e-5);
    const double dose = std::max(value*1000.0, 1.0e-3);

    instantenous_series_.append(modified_timestamp.toMSecsSinceEpoch(), dose);

    if (instantenous_series_.at(0).x() < axis_x_.min().toMSecsSinceEpoch()) {
        instantenous_series_.remove(0);
    }

    if (measurement_indicator_) {
        if (update_rectangle_) {
            rect_time_stop_ = instantenous_series_.pointsVector().last().x();
        }
        QRectF rect = measurement_indicator_->rect();
        rect.setLeft(chartview_->chart()->mapToPosition(QPointF(std::max(rect_time_start_, axis_x_.min().toMSecsSinceEpoch()), axis_y_instantenous_.min())).x());
        rect.setRight(chartview_->chart()->mapToPosition(QPointF(std::max(rect_time_stop_, axis_x_.min().toMSecsSinceEpoch()), axis_y_instantenous_.min())).x());
        measurement_indicator_->setRect(rect);
    }
}

void RadiationLevelGraph::AddIntegratedData(double value) {
    const double dose = std::max(value*1.0e12, 1.0e-4);
    integrated_series_.append(QDateTime::currentMSecsSinceEpoch(), dose);
    if (integrated_series_.at(0).x() < axis_x_.min().toMSecsSinceEpoch()) {
        integrated_series_.remove(0);
    }

}

void RadiationLevelGraph::Clear() {
    instantenous_series_.clear();
    integrated_series_.clear();
}

void RadiationLevelGraph::StartRectangle() {
    if (measurement_indicator_) {
        measurement_indicator_->hide();
        // TODO: delete old here or let qchart do it?
    }
    qDebug() << "RadiationLevelGraph::StartRectangle";
    measurement_indicator_ = new QGraphicsRectItem(chartview_->chart());
    measurement_indicator_->setBrush(QBrush(Qt::yellow));
    measurement_indicator_->setVisible(true);
    rect_time_start_ = chartview_->chart()->mapToPosition(QPointF(axis_x_.min().toMSecsSinceEpoch(), 100)).x();
    if (!instantenous_series_.pointsVector().empty()) {
        rect_time_start_ = instantenous_series_.pointsVector().last().x();
    }
    double y_low = chartview_->chart()->mapToPosition(QPointF(axis_x_.min().toMSecsSinceEpoch(), axis_y_instantenous_.min()), &instantenous_series_).y();
    double y_high = chartview_->chart()->mapToPosition(QPointF(axis_x_.min().toMSecsSinceEpoch(), axis_y_instantenous_.max()), &instantenous_series_).y();

    measurement_indicator_->setRect(chartview_->chart()->mapToPosition(QPointF(rect_time_start_, axis_y_instantenous_.min())).x(), y_high, 1, y_low - y_high); // x, y, width, heigh
    update_rectangle_ = true;
}

void RadiationLevelGraph::StopRectangle() {
    qDebug() << "RadiationLevelGraph::StopRectangle";
    update_rectangle_ = false;
}
