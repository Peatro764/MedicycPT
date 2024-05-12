#include "Histogram.h"

#include <QVector>
#include <algorithm>

#include "Axis.h"
#include "Calc.h"

Histogram::Histogram(QCustomPlot* customplot, QString title, QString xlabel, double min, double max, int n_bins)
    : keys_(n_bins),
      customplot_(customplot),
      bin_size_((max - min) / static_cast<double>(n_bins)),
      min_(min),
      max_(max),
      n_bins_(n_bins) {
    (void)title;
    GenerateColors();
        customplot_->xAxis->setLabel(xlabel);
//        customplot_->axisRect()->setupFullAxesBox();

    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

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

    customplot_->yAxis->setLabel(QString(""));
    customplot_->yAxis->setTickLabels(false);
    customplot_->xAxis->setRange(min_, max_);
    customplot_->yAxis->setRange(0.0, 10.0);
    for (int i = 0; i < n_bins_; ++i) {
        keys_[i] = min_ + 0.5 * bin_size_ + (static_cast<double>(i) * bin_size_);
    }
    customplot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);
    QObject::connect(customplot_, &QCustomPlot::mousePress, this, &Histogram::mousePress);
    QObject::connect(customplot_, &QCustomPlot::mouseWheel, this, &Histogram::mouseWheel);
}

Histogram::~Histogram() {
    customplot_->clearGraphs();
}

void Histogram::EnableTitle() {

}

void Histogram::EnableXLabel() {

}

void Histogram::EnableLegend() {
    customplot_->legend->setFont(QFont("Helvetica",7));
    customplot_->legend->setVisible(true);
    customplot_->update();
    customplot_->replot();
}


void Histogram::GenerateColors() {
    colors_.push_back(QRgb(0x209fdf));
    colors_.push_back(QRgb(0x99ca53));
    colors_.push_back(QRgb(0xf6a625));
    colors_.push_back(QRgb(0x6d5fd5));
    colors_.push_back(QRgb(0xbf593e));
}

bool Histogram::Exists(QString name) const {
    return (bars_.find(name) != bars_.end());
}

void Histogram::Register(QString name) {
    if (Exists(name)) {
        qWarning() << "Histogram::Register Curve exists";
        return;
    }
    QColor color = GetColor();
    QPen pen(color);
    pen.setWidth(3);
    color.setAlpha(122);
    QBrush brush(color);
    bars_[name] = new QCPBars(customplot_->xAxis, customplot_->yAxis);
    bars_[name]->setPen(pen);
    bars_[name]->setName(name);
    bars_[name]->setAntialiased(false);
    bars_[name]->setAntialiasedFill(false);
    bars_[name]->setBrush(brush); //QColor("#FFA100"));
    bars_[name]->setVisible(true);
    bars_[name]->setWidth(bin_size_);
    nmb_[name] = QVector<double>(n_bins_);
    values_[name] = std::vector<double>();

    for (int i = 0; i < n_bins_; ++i) {
        nmb_[name][i] = 0.0;
    }
    bars_[name]->setData(keys_, nmb_[name]);
}

QColor Histogram::GetColor() const {
    if (bars_.size() < colors_.size()) {
        return colors_.at(static_cast<int>(bars_.size()));
    } else {
        return Qt::black;
    }
}

double Histogram::Mean(QString name) const {
    if (!Exists(name)) {
        qWarning() << "Histogram::Mean Curve does not exist";
        return 0.0;
    }
    return calc::Mean(values_.at(name));
}

double Histogram::StdDev(QString name) const {
    if (!Exists(name)) {
        qWarning() << "Histogram::StdDev Curve  does not exist";
        return 0.0;
    }
    return calc::StdDev(values_.at(name));
}

void Histogram::SetData(QString name, std::vector<double> data) {
    if (!Exists(name)) {
        qWarning() << "Histogram::SetData Curve does not exist";
        return;
    }
    values_[name].clear();
    for (int i = 0; i < n_bins_; ++i) {
        nmb_[name][i] = 0.0;
    }

    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
        this->AddValue(name, data.at(i));
    }
    bars_[name]->setData(keys_, nmb_[name]);
    Plot();
}

void Histogram::SetSingleElement(QString name, double value) {
    if (!Exists(name)) {
        qWarning() << "Histogram::SetSingleElement Curve does not exist";
        return;
    }
    AddValue(name, value);
    bars_[name]->setData(keys_, nmb_[name]);
    Plot();
}

void Histogram::AddValue(QString name, double value) {
    if (!Exists(name)) {
        qWarning() << "Histogram::AddValue Curve  does not exist";
        return;
    }
    values_[name].push_back(value);
    int bin = static_cast<int>((value - min_) / bin_size_);
    if (bin < 0) bin = 0;
    if (bin >= n_bins_) bin = (n_bins_ - 1);
    nmb_[name][bin] = nmb_[name][bin] + 1;
}

void Histogram::RemoveAll() {
    customplot_->clearGraphs();
    bars_.clear();
    values_.clear();
    keys_.clear();
    nmb_.clear();
}

void Histogram::Clear() {
    for (auto& v : nmb_) {
        Clear(v.first);
    }
}

void Histogram::Clear(QString name) {
    if (!Exists(name)) {
        qWarning() << "Histogram::Clear Curve does not exist";
        return;
    }
    values_[name].clear();
    for (int i = 0; i < nmb_[name].size(); ++i) {
        nmb_[name][i] = 0.0;
    }
    bars_[name]->setData(keys_, nmb_[name]);
    Plot();
}

void Histogram::Plot() {
    for (auto& v : nmb_) {
        const double max = Max(v.second);
         if (max > customplot_->yAxis->range().upper * 0.95) {
            customplot_->yAxis->setRangeUpper(max * 1.5);
         }
    }
    customplot_->replot();
}

double Histogram::Max(const QVector<double>& data) const {
 return *std::max_element(data.constBegin(), data.constEnd());
}

void Histogram::Normalize() {
    for (auto& mypair : nmb_) {
        const double max = Max(mypair.second);
        for (double &myvar : mypair.second) {
            myvar = myvar / max;
        }
        bars_[mypair.first]->setData(keys_, mypair.second);
    }
    customplot_->yAxis->setRangeUpper(1.1);
    customplot_->replot();
}

void Histogram::mousePress() {
    customplot_->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void Histogram::mouseWheel() {
    customplot_->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
