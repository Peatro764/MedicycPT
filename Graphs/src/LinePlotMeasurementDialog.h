#ifndef QUALITYASSURANCE_LINEPLOTMEASUREMENTDIALOG_H
#define QUALITYASSURANCE_LINEPLOTMEASUREMENTDIALOG_H

#include <QDialog>
#include <QDateTime>

#include "qcustomplot.h"

namespace Ui {
class LinePlotMeasurementDialog;
}

class LinePlotMeasurementDialog : public QDialog
{
    Q_OBJECT

public:
    LinePlotMeasurementDialog(QWidget *parent, QCustomPlot *customplot, QCPGraph* plottable);
    ~LinePlotMeasurementDialog();

private slots:
    void MoveGreenTracer(int value);
    void MoveRedTracer(int value);

private:
    double SliderValueToPosition(int value);
    int PositionToSliderValue(double position);
    Ui::LinePlotMeasurementDialog *ui_;
    QCustomPlot *customplot_;
    QCPGraph * plottable_;
    QCPItemTracer *tracer_green_;
    QCPItemTracer *tracer_red_;
    int slider_values_ = 2000;
    double min_ = 0.0;
    double max_= 0.0;
};

#endif

