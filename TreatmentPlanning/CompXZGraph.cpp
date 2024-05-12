#include "CompXZGraph.h"

#include <QVector>

#include "Compensateur.h"

CompXZGraph::CompXZGraph(QCustomPlot* customplot)
    : customplot_(customplot) {
    customplot_->legend->setVisible(false);
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

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

    customplot_->yAxis->setLabel("Z [mm plexiglas]");
    customplot_->xAxis->setLabel("X [mm]");
    if (customplot_->plotLayout()->rowCount() == 1) {
        customplot_->plotLayout()->insertRow(0);
        customplot_->plotLayout()->addElement(0, 0, new QCPTextElement(customplot_, "Compensateur X-Z", QFont("Century Schoolbook L", 12)));
        qobject_cast<QCPTextElement*>(customplot_->plotLayout()->element(0, 0))->setTextColor(fg_color);
    }
    customplot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);

    ConnectSignals();
}

CompXZGraph::~CompXZGraph() {
    customplot_->clearGraphs();
}

void CompXZGraph::ConnectSignals() {
    QObject::connect(customplot_, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    QObject::connect(customplot_, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
}

void CompXZGraph::Plot(const Compensateur& compensateur) {
    customplot_->clearPlottables();

    compXZ_step_curve_ = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
    compXZ_step_curve_->setName("Compensateur X-Z");

    compXZ_step_curve_->setPen(QPen(QColor(32, 159, 223, 255), 2));
    compXZ_step_curve_->setLineStyle(QCPCurve::lsLine);
    compXZ_step_curve_->setScatterStyle(QCPScatterStyle::ssNone);
    compXZ_step_curve_->setBrush(QBrush(QColor(32, 159, 223, 100))); // light blue

    compXZ_step_curve_->data()->set(compensateur.FullStepCurve());
    compXZ_step_curve_->setVisible(true);

    compXZ_theo_curve_ = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
    compXZ_theo_curve_->setName("Compensateur Theo X-Z");

    compXZ_theo_curve_->setPen(QPen(QColor(246, 166, 37, 255), 2)); // orange
    compXZ_theo_curve_->setLineStyle(QCPCurve::lsLine);
    compXZ_theo_curve_->setScatterStyle(QCPScatterStyle::ssNone);

    compXZ_theo_curve_->data()->set(compensateur.FullTheoCurve());
    compXZ_theo_curve_->setVisible(true);

    bool found;
    const double max_x(std::max(compXZ_step_curve_->getKeyRange(found).upper, std::abs(compXZ_step_curve_->getKeyRange(found).lower)));
    customplot_->xAxis->setRangeUpper(1.2 * max_x);
    customplot_->xAxis->setRangeLower(-1.2 * max_x);
    customplot_->yAxis->setRangeUpper(2.4 * max_x);
    customplot_->yAxis->setRangeLower(0.0);

    customplot_->replot();
}

void CompXZGraph::mousePress()
{
    customplot_->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void CompXZGraph::mouseWheel()
{
    customplot_->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
