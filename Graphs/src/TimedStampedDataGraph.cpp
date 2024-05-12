#include "TimedStampedDataGraph.h"

#include <QVector>

TimedStampedDataGraph::TimedStampedDataGraph(QCustomPlot* customplot)
    : customplot_(customplot) {
    GenerateColors();
    customplot_->legend->setFont(QFont("Helvetica",13));
    customplot_->legend->setVisible(true);
    customplot_->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_->xAxis->ticker()->setTickCount(15);
//    customplot_->axisRect()->setupFullAxesBox();
    QColor fg_color = Qt::white;
    QColor bg_color = QColor("#31363b");

    customplot_->setBackground(bg_color);
    customplot_->legend->setBrush(QBrush(bg_color));
    customplot_->legend->setBorderPen(QPen(fg_color));
    customplot_->legend->setTextColor(fg_color);
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

//    if (customplot_->plotLayout()->rowCount() == 1) {
//        customplot_->plotLayout()->insertRow(0);
//        customplot_->plotLayout()->addElement(0, 0, new QCPTextElement(customplot_, "Variation du débit", QFont("Century Schoolbook L", 16)));
//    }

    customplot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);
    QObject::connect(customplot_, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    QObject::connect(customplot_, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("yyyy\nMM-dd");
    customplot_->xAxis->setTicker(dateTicker);
}

void TimedStampedDataGraph::SetYAxisTitle(QString name) {
    customplot_->yAxis->setLabel(name);
}

TimedStampedDataGraph::~TimedStampedDataGraph() {
    customplot_->clearGraphs();
}

bool TimedStampedDataGraph::Exists(QString name) const {
    return (graphs_.find(name) != graphs_.end());
}

void TimedStampedDataGraph::RemoveAll() {
    customplot_->clearGraphs();
    graphs_.clear();
    errors_.clear();
}


void TimedStampedDataGraph::Register(QString name) {
    if (Exists(name)) {
        qWarning() << "TimedStampedDataGraph::Register Curve Exists";
        return;
    }
    QColor color = GetColor();
    QPen pen(color);
    pen.setWidth(3);
    color.setAlpha(122);

    graphs_[name] = customplot_->addGraph();
    graphs_[name]->setPen(pen);
    graphs_[name]->setLineStyle(QCPGraph::lsNone);
    graphs_[name]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, pen, QBrush(color), 7));
    graphs_[name]->setVisible(true);
    graphs_[name]->setName(name);

    errors_[name] = new QCPErrorBars(customplot_->xAxis, customplot_->yAxis);
    errors_[name]->removeFromLegend();
    errors_[name]->setDataPlottable(graphs_[name]);
    errors_[name]->setPen(pen);
}

void TimedStampedDataGraph::Clear() {
    for (auto& g : graphs_) {
        Clear(g.first);
    }
}

void TimedStampedDataGraph::Clear(QString name) {
    if (!Exists(name)) {
        qWarning() << "TimedStampedDataGraph::Clear Curve does not exist";
        return;
    }
    graphs_[name]->data().clear();
    errors_[name]->data().clear();
    Plot();
}


void TimedStampedDataGraph::SetData(QString name, const TimedStampedDataSeries& data) {
    if (!Exists(name)) {
        qWarning() << "TimedStampedDataGraph::SetData Curve  does not exist";
        return;
    }
    graphs_[name]->setData(data.time, data.value, false);
    errors_[name]->setData(data.error);
    Plot();
}

void TimedStampedDataGraph::AddPoint(QString name, double time, double value, double error) {
    if (!Exists(name)) {
        qWarning() << "TimedStampedDataGraph::AddPoint Curve  does not exist";
        return;
    }
    graphs_[name]->addData(time, value);
    errors_[name]->addData(error);
    Plot();
}

void TimedStampedDataGraph::SetTimeRange(QDate lower, QDate upper) {
    if (upper <= lower) {
        qWarning() << "TimedStampedDataGraph::SetTimeRange Upper limit equal or smaller than lower limit";
        return;
    }
    customplot_->xAxis->setRange(QDateTime(lower).toTime_t(), QDateTime(upper).toTime_t());
    Plot();
}

void TimedStampedDataGraph::Plot() {
    customplot_->rescaleAxes(true);
    customplot_->xAxis->scaleRange(1.1);
    customplot_->replot();
}

void TimedStampedDataGraph::mousePress() {
    customplot_->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void TimedStampedDataGraph::mouseWheel() {
    customplot_->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

QColor TimedStampedDataGraph::GetColor() const {
    if (graphs_.size() < colors_.size()) {
        return colors_.at(static_cast<int>(graphs_.size()));
    } else {
        return Qt::black;
    }
}

void TimedStampedDataGraph::GenerateColors() {
    colors_.push_back(QRgb(0x209fdf));
    colors_.push_back(QRgb(0x99ca53));
    colors_.push_back(QRgb(0xf6a625));
    colors_.push_back(QRgb(0x6d5fd5));
    colors_.push_back(QRgb(0xbf593e));
}
