#include "LinePlotMeasurementDialog.h"
#include "ui_LinePlotMeasurementDialog.h"

LinePlotMeasurementDialog::LinePlotMeasurementDialog(QWidget *parent, QCustomPlot *customplot, QCPGraph *plottable)
    : QDialog(parent),
      ui_(new Ui::LinePlotMeasurementDialog),
      customplot_(customplot),
      plottable_(plottable),
      tracer_green_(new QCPItemTracer(customplot)),
      tracer_red_(new QCPItemTracer(customplot))
{
    ui_->setupUi(this);
    setWindowTitle("LinePlot Measurement");

    bool ok(false);
    QCPRange range = plottable->getKeyRange(ok);
    min_ = range.lower;
    max_ = range.upper;

    tracer_red_->setGraph(plottable);
    tracer_red_->setInterpolating(true);
    tracer_red_->setStyle(QCPItemTracer::tsCircle);
    tracer_red_->setPen(QPen(Qt::red));
    tracer_red_->setBrush(Qt::red);
    tracer_red_->setSize(7);

    tracer_green_->setGraph(plottable);
    tracer_green_->setInterpolating(true);
    tracer_green_->setStyle(QCPItemTracer::tsCircle);
    tracer_green_->setPen(QPen(Qt::green));
    tracer_green_->setBrush(Qt::green);
    tracer_green_->setSize(7);

    QObject::connect(ui_->verticalSlider_green, &QSlider::valueChanged, this, &LinePlotMeasurementDialog::MoveGreenTracer);
    QObject::connect(ui_->verticalSlider_red, &QSlider::valueChanged, this, &LinePlotMeasurementDialog::MoveRedTracer);

    ui_->verticalSlider_green->setMinimum(0);
    ui_->verticalSlider_green->setMaximum(slider_values_);
    ui_->verticalSlider_green->setValue(95);

    ui_->verticalSlider_red->setMinimum(0);
    ui_->verticalSlider_red->setMaximum(slider_values_);
    ui_->verticalSlider_green->setValue(105);

    QObject::connect(ui_->pushButton_close, SIGNAL(clicked()), this, SLOT(accept()));
}

LinePlotMeasurementDialog::~LinePlotMeasurementDialog() {
    tracer_red_->setGraph(0);
    tracer_green_->setGraph(0);
    customplot_->removeItem(tracer_red_);
    customplot_->removeItem(tracer_green_);
}

double LinePlotMeasurementDialog::SliderValueToPosition(int value) {
    return min_ + (static_cast<double>(value) / static_cast<double>(slider_values_)) * (max_ - min_);
}

int LinePlotMeasurementDialog::PositionToSliderValue(double position) {
    return static_cast<int>(std::round((position - min_) * static_cast<double>(slider_values_) / (max_ - min_)));
}

void LinePlotMeasurementDialog::MoveGreenTracer(int value) {
    tracer_green_->setGraphKey(SliderValueToPosition(value));
    ui_->lineEdit_horisontal_green->setText(QString::number(tracer_green_->graphKey(), 'f', 2));
    ui_->lineEdit_horisontal_diff->setText(QString::number(tracer_green_->graphKey() - tracer_red_->graphKey(), 'f', 2));
    ui_->lineEdit_vertical_green->setText(QString::number(tracer_green_->position->value(), 'f', 2));
    ui_->lineEdit_vertical_diff->setText(QString::number(tracer_green_->position->value() - tracer_red_->position->value(), 'f', 2));
    customplot_->replot();
}

void LinePlotMeasurementDialog::MoveRedTracer(int value) {
    tracer_red_->setGraphKey(SliderValueToPosition(value));
    ui_->lineEdit_horisontal_red->setText(QString::number(tracer_red_->graphKey(), 'f', 2));
    ui_->lineEdit_horisontal_diff->setText(QString::number(tracer_green_->graphKey() - tracer_red_->graphKey(), 'f', 2));
    ui_->lineEdit_vertical_red->setText(QString::number(tracer_red_->position->value(), 'f', 2));
    ui_->lineEdit_vertical_diff->setText(QString::number(tracer_green_->position->value() - tracer_red_->position->value(), 'f', 2));
    customplot_->replot();
}



