#include "CompXYGraph.h"

#include <QVector>
#include <QtMath>

#include "Compensateur.h"

CompXYGraph::CompXYGraph(QCustomPlot* customplot)
    : customplot_(customplot) {
    customplot_->legend->setVisible(false);
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_->yAxis->setLabel("Y [mm]");
    customplot_->xAxis->setLabel("X [mm]");

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

    if (customplot_->plotLayout()->rowCount() == 1) {
        customplot_->plotLayout()->insertRow(0);
        customplot_->plotLayout()->addElement(0, 0, new QCPTextElement(customplot_, "Compensateur X-Y", QFont("Century Schoolbook L", 12)));

        qobject_cast<QCPTextElement*>(customplot_->plotLayout()->element(0, 0))->setTextColor(fg_color);
    }

    customplot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);

    ConnectSignals();
}

CompXYGraph::~CompXYGraph() {
    customplot_->clearGraphs();
}

void CompXYGraph::ConnectSignals() {
    QObject::connect(customplot_, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    QObject::connect(customplot_, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
}

void CompXYGraph::Plot(const Compensateur& compensateur) {
    customplot_->clearPlottables();

    QCPCurveDataContainer radius_height(compensateur.FraiseuseFormat());
    for (QCPCurveData p : radius_height) {
        QCPCurve *c = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
        c->data()->set(MakeCircle(p.key, compensateur.center_point()));
        c->setPen(QPen(QColor(32, 159, 223, 80), 2));
        c->setBrush(QBrush(QColor(32, 159, 223, 20)));
        c->setLineStyle(QCPCurve::lsLine);
        c->setScatterStyle(QCPScatterStyle::ssNone);
        c->setVisible(true);
    }

    double theo_outer_radius(compensateur.eye().sclere_radius());
    QCPCurve *c_theo_outer_radius = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
    c_theo_outer_radius->data()->set(MakeCircle(theo_outer_radius, compensateur.center_point()));
    c_theo_outer_radius->setPen(QPen(QColor(246, 166, 37, 255), 2));
    c_theo_outer_radius->setLineStyle(QCPCurve::lsLine);
    c_theo_outer_radius->setScatterStyle(QCPScatterStyle::ssNone);
    c_theo_outer_radius->setVisible(true);

    auto* curve(customplot_->plottable(customplot_->plottableCount() - 1));
    curve->setName("Compensateur X-Y");
    bool found;
    const double max_x(std::max(curve->getKeyRange(found).upper, std::abs(curve->getKeyRange(found).lower)));
    const double max_y(std::max(curve->getValueRange(found).upper, std::abs(curve->getValueRange(found).lower)));
    const double max(std::max(max_x, max_y));
    customplot_->xAxis->setRangeUpper(1.2 * max);
    customplot_->xAxis->setRangeLower(-1.2 * max);
    customplot_->yAxis->setRangeUpper(1.2 * max);
    customplot_->yAxis->setRangeLower(-1.2 * max);

    customplot_->replot();
}

QCPCurveDataContainer CompXYGraph::MakeCircle(const double radius, const QCPCurveData& centre) const {
    QCPCurveDataContainer data;
    for (double angle = 0; angle < (2.0 * 3.1415); angle += (3.1415/100.0)) {
        data.add(QCPCurveData(angle, radius * qCos(angle) + centre.key, radius * qSin(angle) + centre.value));
    }
    return data;
}

void CompXYGraph::mousePress()
{
    customplot_->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void CompXYGraph::mouseWheel()
{
    customplot_->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
