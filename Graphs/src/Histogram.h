#ifndef BEAMCHARACTERISTICS_HISTOGRAM_H
#define BEAMCHARACTERISTICS_HISTOGRAM_H

#include "qcustomplot.h"

#include <vector>

class Histogram : public QObject
{

public:
    Histogram(QCustomPlot* customplot, QString title, QString xlabel, double min, double max, int n_bins);
    ~Histogram();
    void Register(QString name);
    void SetData(QString name, std::vector<double> data);
    void SetSingleElement(QString name, double value);
    void RemoveAll();
    void Clear();
    void Clear(QString name);
    double Mean(QString name) const;
    double StdDev(QString name) const;
    void Normalize();
    void EnableTitle();
    void EnableXLabel();
    void EnableLegend();

public slots:
    void mousePress();
    void mouseWheel();

private:
    void GenerateColors();
    void Plot();
    bool Exists(QString name) const;
    double Max(const QVector<double>& data) const;
    QColor GetColor() const;
    void AddValue(QString name, double value);
    std::map<QString, std::vector<double>> values_;
    QVector<double> keys_;
    std::map<QString, QVector<double>> nmb_;
    QCustomPlot* customplot_;
    std::map<QString, QCPBars*> bars_;
    double bin_size_;
    double min_;
    double max_;
    int n_bins_;
    std::vector<QColor> colors_;
};

#endif

