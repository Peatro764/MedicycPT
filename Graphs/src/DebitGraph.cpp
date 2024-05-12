#include "DebitGraph.h"

#include <QVector>

DebitGraph::DebitGraph(QCustomPlot* customplot)
    : customplot_(customplot) {
    GenerateColors();
    customplot_->legend->setFont(QFont("Helvetica",7));
    customplot_->legend->setVisible(true);
    customplot_->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_->yAxis->setLabel("Debit [cGy/UM]");
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

DebitGraph::~DebitGraph() {
    customplot_->clearGraphs();
}

bool DebitGraph::Exists(QString name) const {
    return (debits_graph_.find(name) != debits_graph_.end());
}

void DebitGraph::RemoveAll() {
    customplot_->clearGraphs();
    debits_graph_.clear();
}


void DebitGraph::Register(QString name) {
    if (Exists(name)) {
        qWarning() << "Histogram::Register Curve Exists";
        return;
    }
    QColor color = GetColor();
    QPen pen(color);
    pen.setWidth(3);
    color.setAlpha(122);

    debits_graph_[name] = customplot_->addGraph();
    debits_graph_[name]->setPen(pen);
    debits_graph_[name]->setLineStyle(QCPGraph::lsNone);
    debits_graph_[name]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, pen, QBrush(color), 7));
    debits_graph_[name]->setVisible(true);
    debits_graph_[name]->setName(name);
}

void DebitGraph::Clear() {
    for (auto& g : debits_graph_) {
        Clear(g.first);
    }
}

void DebitGraph::Clear(QString name) {
    if (!Exists(name)) {
        qWarning() << "DebitGraph::Clear Curve does not exist";
        return;
    }
    debits_graph_[name]->data().clear();
    Plot();
}


void DebitGraph::SetData(QString name, const std::vector<Debit> &debits) {
    if (!Exists(name)) {
        qWarning() << "DebitGraph::SetData Curve  does not exist";
        return;
    }
    QVector<double> keys;
    QVector<double> values;
    for (auto debit : debits) {
        keys.push_back(debit.timestamp().toTime_t());
        values.push_back(debit.mean());
    }
    debits_graph_[name]->setData(keys, values, false);
    Plot();
}

void DebitGraph::AddPoint(QString name, const Debit& debit) {
    if (!Exists(name)) {
        qWarning() << "DebitGraph::AddPoint Curve  does not exist";
        return;
    }
    debits_graph_[name]->addData(debit.timestamp().toTime_t(), debit.mean());
    Plot();
}

void DebitGraph::Plot() {
    customplot_->rescaleAxes(true);
    customplot_->xAxis->scaleRange(1.1);
    customplot_->replot();
}

void DebitGraph::mousePress() {
    customplot_->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void DebitGraph::mouseWheel() {
    customplot_->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

QColor DebitGraph::GetColor() const {
    if (debits_graph_.size() < colors_.size()) {
        return colors_.at(static_cast<int>(debits_graph_.size()));
    } else {
        return Qt::black;
    }
}

void DebitGraph::GenerateColors() {
    colors_.push_back(QRgb(0x209fdf));
    colors_.push_back(QRgb(0x99ca53));
    colors_.push_back(QRgb(0xf6a625));
    colors_.push_back(QRgb(0x6d5fd5));
    colors_.push_back(QRgb(0xbf593e));
}
