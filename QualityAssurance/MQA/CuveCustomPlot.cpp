#include "CuveCustomPlot.h"

#include <QVector>

CuveCustomPlot::CuveCustomPlot(QCustomPlot* customplot, QString h_title, QString v_title)
    : customplot_(customplot) {
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_->xAxis->setLabel(h_title);
    customplot_->yAxis->setLabel(v_title);

    customplot_->axisRect()->setupFullAxesBox();
    customplot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    color_map_ = new QCPColorMap(customplot_->xAxis, customplot_->yAxis);
    color_map_->setInterpolate(false);
    color_map_->data()->setSize(100, 100);
    color_map_->data()->setRange(QCPRange(-15.0, 15.0), QCPRange(-15.0, 15.0));

    QCPColorScale *color_scale = new QCPColorScale(customplot_);
    customplot_->plotLayout()->addElement(0, 1, color_scale);
    color_scale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    color_map_->setColorScale(color_scale); // associate the color map with the color scale
//    color_scale->axis()->setLabel("Signal");
    QCPColorGradient color_gradient(QCPColorGradient::gpJet);
    color_gradient.setLevelCount(350);
    color_map_->setGradient(color_gradient);
    color_map_->rescaleDataRange();

    QCPMarginGroup *marginGroup = new QCPMarginGroup(customplot_);
    customplot_->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    color_scale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    customplot_->rescaleAxes();

    QObject::connect(customplot_, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
}

void CuveCustomPlot::SetAxisTitles(QString h_title, QString v_title) {
    customplot_->xAxis->setLabel(h_title);
    customplot_->yAxis->setLabel(v_title);
    customplot_->replot();
}


CuveCustomPlot::~CuveCustomPlot() {
    customplot_->clearGraphs();
}

void CuveCustomPlot::SetGradientLevels(int n) {
    QCPColorGradient gradient = color_map_->gradient();
    gradient.setLevelCount(n);
    color_map_->setGradient(gradient);
    Plot();
}

void CuveCustomPlot::SetInterpolate(bool on) {
    qDebug() << "SetInterpolate";
    color_map_->setInterpolate(on);
    Plot();
}

void CuveCustomPlot::SetTightBoundary(bool on) {
    color_map_->setTightBoundary(on);
}

void CuveCustomPlot::Clear() {
    color_map_->data()->fill(0.0);
}

void CuveCustomPlot::SetData(QCPColorMapData data) {
    color_map_->setData(&data, true); // true = data copied
    // SetPlotRange(Range(0.0, 1.0), Range(0.0, 0.0), 0.0, 0.0);
    Plot();
}

void CuveCustomPlot::AddPointByBin(int x_bin, int y_bin, double signal) {
    color_map_->data()->setCell(x_bin, y_bin, signal);
    Plot();
}

void CuveCustomPlot::AddPointByCoord(double x_coord, double y_coord, double signal) {
    color_map_->data()->setData(x_coord, y_coord, signal);
    Plot();
}

void CuveCustomPlot::Plot() {
    color_map_->rescaleDataRange();
    customplot_->replot();
}

void CuveCustomPlot::Rescale() {
    color_map_->rescaleDataRange();
}

void CuveCustomPlot::SetDataRange(Range x, Range y, int x_bins, int y_bins) {
    qDebug() << "CuveCustomPlot::SetDataRange " << x.toString() << " " << y.toString()
             << " " << QString::number(x_bins) << " " << QString::number(y_bins);
    color_map_->data()->setSize(x_bins, y_bins);
    if (std::abs(x.length()) < 0.01) {
        x = Range(x.start() - 0.01, x.start() + 0.01);
    }
    if (std::abs(y.length()) < 0.01) {
        y = Range(y.start() - 0.01, y.start() + 0.01);
    }
    color_map_->data()->setRange(AsQCPRange(x), AsQCPRange(y));
    customplot_->replot();
}

void CuveCustomPlot::SetPlotRange(Range x, Range y, double dx, double dy) {
    qDebug() << "CuveCustomPlot::SePlotRange " << x.toString() << " " << y.toString()
             << " " << QString::number(dx, 'f', 2) << " " << QString::number(dy, 'f', 2);
    if (std::abs(x.length()) < 0.01) {
        x = Range(x.start() - 0.01, x.start() + 0.01);
    }
    if (std::abs(y.length()) < 0.01) {
        y = Range(y.start() - 0.01, y.start() + 0.01);
    }
    color_map_->keyAxis()->setRange(AsQCPRange(x));
    color_map_->valueAxis()->setRange(AsQCPRange(y));
    customplot_->replot();
}

QCPRange CuveCustomPlot::AsQCPRange(const Range& range) const {
    return QCPRange(range.start(), range.end());
}

void CuveCustomPlot::mouseMove(QMouseEvent* event) {
    double x = event->pos().x();
    double y = event->pos().y();
    double x_coord = color_map_->keyAxis()->pixelToCoord(x);
    double y_coord = color_map_->valueAxis()->pixelToCoord(y);
    int x_bin, y_bin;
    color_map_->data()->coordToCell(x_coord, y_coord, &x_bin, &y_bin);
    double value = color_map_->data()->data(x_coord, y_coord);

    emit MouseOver(x_bin, y_bin, x_coord, y_coord, value);
}
