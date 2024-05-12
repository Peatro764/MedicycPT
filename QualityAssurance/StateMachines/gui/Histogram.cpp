#include "Histogram.h"

#include <QVector>
#include <algorithm>

#include "Axis.h"
#include "Calc.h"

Histogram::Histogram(QCustomPlot* customplot, QString title, QString xlabel, double min, double max, int n_bins)
    : keys_(n_bins),
      nmb_(n_bins),
      customplot_(customplot),
      bin_size_((max - min) / static_cast<double>(n_bins)),
      min_(min),
      max_(max),
      n_bins_(n_bins) {
    customplot_->legend->setVisible(false);
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
//    customplot_->xAxis->setLabel(xlabel);
//    customplot_->axisRect()->setupFullAxesBox();
//    if (customplot_->plotLayout()->rowCount() == 1) {
//        customplot_->plotLayout()->insertRow(0);
//        customplot_->plotLayout()->addElement(0, 0, new QCPTextElement(customplot_, title, QFont("Century Schoolbook L", 16)));
//    }
    bars_ = new QCPBars(customplot_->xAxis, customplot_->yAxis);
    bars_->setPen(QPen(Qt::black));
    bars_->setAntialiased(false);
    bars_->setAntialiasedFill(false);
    bars_->setBrush(QColor("#FFA100"));
    bars_->setVisible(true);

    customplot_->yAxis->setLabel(QString(""));
    customplot_->yAxis->setTickLabels(false);
    bars_->setWidth(bin_size_);
    customplot_->xAxis->setRange(min_, max_);
    customplot_->yAxis->setRange(0.0, 10.0);
    for (int i = 0; i < n_bins_; ++i) {
        keys_[i] = min_ + 0.5 * bin_size_ + (static_cast<double>(i) * bin_size_);
        nmb_[i] = 0.0;
    }
    bars_->setData(keys_, nmb_);

}

Histogram::~Histogram() {
    customplot_->clearGraphs();
}

double Histogram::Mean() const {
    return calc::Mean(values_);
}

double Histogram::StdDev() const {
    return calc::StdDev(values_);
}

void Histogram::SetData(std::vector<double> data) {
    values_ = data;
    std::sort(data.begin(), data.end());
    nmb_.clear();
    for (int i = 0; i < n_bins_; ++i) {
        nmb_[i] = 0.0;
    }

    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
        this->AddValue(data.at(i));
    }
    Plot();
}

void Histogram::SetSingleElement(double value) {
    AddValue(value);
    bars_->setData(keys_, nmb_);
    Plot();
}

void Histogram::AddValue(double value) {
    values_.push_back(value);
    int bin = static_cast<int>((value - min_) / bin_size_);
    if (bin < 0) bin = 0;
    if (bin >= n_bins_) bin = (n_bins_ - 1);
    nmb_[bin] = nmb_[bin] + 1;
    qDebug() << value << " " << bin << " " << nmb_[bin];
}

void Histogram::Clear() {
    values_.clear();
    for (int i = 0; i < nmb_.size(); ++i) {
        nmb_[i] = 0.0;
    }
    Plot();
}

void Histogram::Plot() {
//    customplot_->rescaleAxes(true);
//    customplot_->yAxis->setRangeLower(0.0);
//    customplot_->yAxis->setRangeUpper(customplot_->yAxis->range().upper * 1.1);
    if (*std::max_element(nmb_.constBegin(), nmb_.constEnd()) > customplot_->yAxis->range().upper * 0.95) {
        customplot_->yAxis->setRangeUpper(customplot_->yAxis->range().upper * 1.5);
    }
    customplot_->replot();
}
