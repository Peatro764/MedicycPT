#include "ModulateurGraph.h"

#include <QVector>
#include <QtMath>
#include <math.h>
#include <QDebug>

#include "ColorHandler.h"

ModulateurGraph::ModulateurGraph(QCustomPlot* customplot)
    : customplot_(customplot) {
    customplot_->legend->setVisible(false);
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_->yAxis->setLabel("Y [a.u]");
    customplot_->xAxis->setLabel("X [a.u]");
    customplot_->xAxis->grid()->setVisible(false);
    customplot_->yAxis->grid()->setVisible(false);
    customplot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);
    QColor fg_color = Qt::white;
    QColor bg_color = QColor("#31363b");

    customplot_->setBackground(bg_color);

    customplot_->yAxis->setLabelColor(fg_color);
    customplot_->yAxis->setBasePen(QPen(fg_color, 1));
    customplot_->yAxis->setTickPen(QPen(fg_color, 1));
    customplot_->yAxis->setSubTickPen(QPen(fg_color, 1));
    customplot_->yAxis->setTickLabelColor(fg_color);
    customplot_->yAxis->grid()->setPen(QPen(fg_color, 0.5, Qt::DotLine));
    customplot_->yAxis->grid()->setSubGridVisible(false);

    customplot_->xAxis->setLabelColor(fg_color);
    customplot_->xAxis->setBasePen(QPen(fg_color, 1));
    customplot_->xAxis->setTickPen(QPen(fg_color, 1));
    customplot_->xAxis->setSubTickPen(QPen(fg_color, 1));
    customplot_->xAxis->setTickLabelColor(fg_color);
    customplot_->xAxis->grid()->setPen(QPen(fg_color, 0.5, Qt::DotLine));
    customplot_->xAxis->grid()->setSubGridVisible(false);

    ConnectSignals();
}

ModulateurGraph::~ModulateurGraph() {
    customplot_->clearGraphs();
}

void ModulateurGraph::ConnectSignals() {
    QObject::connect(customplot_, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    QObject::connect(customplot_, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
}

void ModulateurGraph::Clear() {
    customplot_->clearGraphs();
}

void ModulateurGraph::Plot(const um_plexi_weight& angles) {
    customplot_->clearPlottables();

    QCPCurveData center_point(0.0, 0.0, 0.0);

    const double radius(50.0);
    QCPCurve *outer_circle = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
    outer_circle->data()->set(MakeCircle(radius, center_point));
    outer_circle->setPen(QPen(Qt::black, 5));
    outer_circle->setLineStyle(QCPCurve::lsLine);
    outer_circle->setScatterStyle(QCPScatterStyle::ssNone);
    outer_circle->setVisible(true);

    const int n_wings(static_cast<int>(std::round(360.0 / angles.begin()->second)));
    std::vector<QColor> colors(colorhandler::GenerateColors((int)angles.size()));
    for (int wing = 0; wing < n_wings; ++wing) {
        int color_index(0);
        for (auto& angle : angles) {
            const double centre_angle(wing * 360.0 / (double)n_wings);
            QCPCurve *segment = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
            segment->data()->set(MakeSegment(radius, center_point, angle.second, centre_angle));
            QColor color(colors.at(color_index++));

            if (color_index == 1) { // no border for zero thickness
                segment->setPen(QPen(color, 1));
            } else {
                segment->setPen(QPen(Qt::black, 1));
            }
            segment->setLineStyle(QCPCurve::lsLine);
            segment->setScatterStyle(QCPScatterStyle::ssNone);
            segment->setBrush(QBrush(color));
            segment->setVisible(true);
        }
    }

    customplot_->xAxis->setRangeUpper(1.2 * radius);
    customplot_->xAxis->setRangeLower(-1.2 * radius);
    customplot_->yAxis->setRangeUpper(1.2 * radius);
    customplot_->yAxis->setRangeLower(-1.2 * radius);
    customplot_->replot();
}

QCPCurveDataContainer ModulateurGraph::MakeCircle(const double radius, const QCPCurveData& centre) const {
    QCPCurveDataContainer data;
    for (double angle = 0; angle < (2.0 * 3.1415); angle += (3.1415/100.0)) {
        data.add(QCPCurveData(angle, radius * qCos(angle) + centre.key, radius * qSin(angle) + centre.value));
    }
    return data;
}

QCPCurveDataContainer ModulateurGraph::MakeSegment(const double radius, const QCPCurveData& centre, const double angle_spread, const double angle_centre) const {
    QCPCurveDataContainer data;
    data.add(QCPCurveData(0.0, centre.mainKey(), centre.mainValue()));
    double start_angle((M_PI / 180.0) * (angle_centre - angle_spread / 2.0));
    double end_angle((M_PI / 180.0) * (angle_centre + angle_spread / 2.0));
    double key(1.0);
    const double delta_angle(std::abs(start_angle - end_angle) / (3600.0 * std::abs(start_angle - end_angle) / ((2.0 * M_PI))));
    for (double angle = start_angle; angle <= end_angle; angle += delta_angle) {
        data.add(QCPCurveData(key, radius * qCos(angle) + centre.mainKey(), radius * qSin(angle) + centre.mainValue()));
        key += 1.0;
    }
    data.add(QCPCurveData(key, centre.mainKey(), centre.mainValue()));
    return data;
}

void ModulateurGraph::mousePress()
{
    customplot_->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void ModulateurGraph::mouseWheel()
{
    customplot_->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
