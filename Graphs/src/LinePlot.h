#ifndef BEAMCHARACTERISTICS_LINEPLOT_H
#define BEAMCHARACTERISTICS_LINEPLOT_H

#include <QObject>

#include "qcustomplot.h"
#include "BeamMeasurement.h"
#include "MeasurementPoint.h"

#include <map>
#include <memory>

class LinePlot : public QObject
{
    Q_OBJECT

public:
    LinePlot(QCustomPlot* customplot);
    ~LinePlot();
public slots:
    void InitCurve(QString curve, Axis axis, int line_width, Qt::PenStyle style);
    void SetCurves(QString curve, BeamMeasurement m);
    void AddPoint(QString curve, MeasurementPoint p, double noise);
    void SetRange(double hor_min, double hor_max, double ver_min, double ver_max);
    void SetHorRange(double hor_min, double hor_max);
    void SetHorLowerRange(double value);
    void SetHorUpperRange(double value);
    void Remove();
    void Clear();
    void Clear(QString curve);
    void Remove(QString curve);
    void SetAxisLabels(QString x_label, QString y_label);
    void ShowLegend(bool toggle);
    void SetIndicatorPoints(QString curve, std::vector<double> x);
    void ShowHorisontalDistanceToZero(QString curve, double pos, QString label, double y_line_offset, double y_label_offset);
    void ShowHorisontalDistance(QString curve, double pos1, double pos2, QString label, double y_line_offset, double y_label_offset);
    void RemoveIndicatorPoints(QString curve);
    void ToggleVisible(QString curve, bool visible);
    bool IsVisible(QString curve) const;
    void SetDarkStyle();
    void SetLightStyle();

private slots:
    void DisplayMeasurementDialog(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);

private:
    QCPGraphDataContainer CurveToGraphData(QCPCurveDataContainer curve_data) const;
    bool CurvesExist(QString curve) const;
    QPen Pen(int line_width) const;
    void Plot();
    void RescaleAxis();
    QCustomPlot* customplot_;
    std::map<QString, QCPGraph*> curves_;
    std::map<QString, std::vector<QCPItemTracer*>> indicator_points_;
    std::map<QString, Axis> principal_axis_;
    double min_ = -0.01;
    double max_ = 0.01;
};

#endif
