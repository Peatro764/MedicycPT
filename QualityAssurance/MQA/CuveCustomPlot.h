#ifndef BEAMCHARACTERISTICS_CUVECUSTOMPLOT_H
#define BEAMCHARACTERISTICS_CUVECUSTOMPLOT_H

#include <QObject>
#include <memory>

#include "qcustomplot.h"
#include "Range.h"

class CuveCustomPlot : public QObject
{
    Q_OBJECT

public:
    CuveCustomPlot(QCustomPlot* customplot, QString h_title, QString v_title);
    ~CuveCustomPlot();
public slots:
    void SetData(QCPColorMapData data);
    void AddPointByBin(int x_bin, int y_bin, double signal);
    void AddPointByCoord(double x_coord, double y_coord, double signal);
    void Clear();
    void Rescale();
    void SetDataRange(Range x, Range y, int x_bins, int y_bins);
    void SetPlotRange(Range x, Range y, double dx, double dy);
    void SetInterpolate(bool on);
    void SetTightBoundary(bool on);
    void SetGradientLevels(int n);
    void SetAxisTitles(QString h_title, QString v_title);

signals:
    void MouseOver(int h_bin, int v_bin, double h_pos, double v_pos, double value);

private slots:
    void mouseMove(QMouseEvent* event);

private:
    void Plot();
    QCPRange AsQCPRange(const Range& range) const;
    QCustomPlot* customplot_;
    QCPColorMap* color_map_;
};

#endif
