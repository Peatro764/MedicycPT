#include "CompErrorGraph.h"

#include <QVector>

#include "Compensateur.h"
#include "Calc.h"
#include "Material.h"

CompErrorGraph::CompErrorGraph(QCustomPlot* customplot)
    : customplot_(customplot) {
    customplot_->legend->setVisible(false);
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_->yAxis->setLabel("Erreur z [mm tissue]");
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
        customplot_->plotLayout()->addElement(0, 0, new QCPTextElement(customplot_, "Compensateur Z Erreur", QFont("Century Schoolbook L", 12)));
        qobject_cast<QCPTextElement*>(customplot_->plotLayout()->element(0, 0))->setTextColor(fg_color);
    }
    customplot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);

    ConnectSignals();
}

CompErrorGraph::~CompErrorGraph() {
    customplot_->clearGraphs();
}

void CompErrorGraph::ConnectSignals() {
    QObject::connect(customplot_, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    QObject::connect(customplot_, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
}

void CompErrorGraph::Plot(const Compensateur& compensateur) {
    customplot_->clearPlottables();

    compError_bars_ = new QCPBars(customplot_->xAxis, customplot_->yAxis);
    compError_bars_->setWidth(compensateur.parcours_error() / 5.0);
    compError_bars_->setPen(QPen(QColor(153, 202, 83, 255), 1));
    compError_bars_->setBrush(QBrush(QColor(153, 202, 83, 60)));

    QCPCurveDataContainer theo_curve(compensateur.HalfTheoCurve());
    for (auto it = theo_curve.constBegin(); it < (theo_curve.constEnd() - 1); ++it) {
        double error(material::Plexiglas2Tissue(it->mainValue() - compensateur.mm_plexiglas(it->mainKey())));
        if (error > 5.0) {
            qDebug() << it->mainKey() << " " << it->mainValue() << " " << compensateur.mm_plexiglas(it->mainKey());
        }
        compError_bars_->addData(it->mainKey(), error);
    }

    compError_bars_->rescaleAxes();
    customplot_->replot();
}

void CompErrorGraph::mousePress()
{
    customplot_->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void CompErrorGraph::mouseWheel()
{
    customplot_->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
