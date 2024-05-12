#ifndef BEAMCHARACTERISTICS_HISTOGRAM_H
#define BEAMCHARACTERISTICS_HISTOGRAM_H

#include "qcustomplot.h"

#include <vector>

class Histogram
{

public:
    Histogram(QCustomPlot* customplot, QString title, QString xlabel, double min, double max, int n_bins);
    ~Histogram();
    void SetData(std::vector<double> data);
    void SetSingleElement(double value);
    void Clear();
    double Mean() const;
    double StdDev() const;

private:
    void Plot();
    void AddValue(double value);
    std::vector<double> values_;
    QVector<double> keys_;
    QVector<double> nmb_;
    QCustomPlot* customplot_;
    QCPBars* bars_;
    double bin_size_;
    double min_;
    double max_;
    int n_bins_;
};

#endif

