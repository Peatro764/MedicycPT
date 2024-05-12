#include "LinePlot.h"

#include <QVector>

#include "Calc.h"
#include "LinePlotMeasurementDialog.h"

LinePlot::LinePlot(QCustomPlot* customplot)
    : customplot_(customplot) {
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_->setInteractions(QCP::iSelectPlottables);
    customplot_->setInteraction(QCP::iRangeDrag, true);
    customplot_->setInteraction(QCP::iRangeZoom, true);
    customplot_->xAxis->setLabel("Position [mm]");
    customplot_->yAxis->setLabel("Intensity");
    //customplot_->axisRect()->setupFullAxesBox();
    customplot_->yAxis->grid()->setSubGridVisible(true);
    customplot_->xAxis->grid()->setSubGridVisible(true);

    SetDarkStyle();

    customplot_->legend->setBrush(Qt::transparent);
    customplot_->legend->setVisible(true);

    QObject::connect(customplot_, &QCustomPlot::plottableDoubleClick, this, &LinePlot::DisplayMeasurementDialog);
    customplot_->update();
}

LinePlot::~LinePlot() {
    qDebug() << "~LinePlot";
    Clear();
    customplot_->clearGraphs();
}

void LinePlot::SetDarkStyle() {
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
    customplot_->update();
}

void LinePlot::SetLightStyle() {
    QColor fg_color = Qt::black;
    QColor bg_color = Qt::white;

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

    customplot_->update();
}

void LinePlot::SetAxisLabels(QString x_label, QString y_label) {
    customplot_->xAxis->setLabel(x_label);
    customplot_->yAxis->setLabel(y_label);
    customplot_->update();
}

void LinePlot::ShowLegend(bool toggle) {
    customplot_->legend->setVisible(toggle);
    customplot_->update();
}

void LinePlot::RemoveIndicatorPoints(QString curve) {
    if (!CurvesExist(curve)) {
        qWarning() << "LinePlot::RemoveIndicatorPoints curve does not exist: " << curve;
        return;
    }
    std::vector<QCPItemTracer*> points = indicator_points_[curve];
    for (QCPItemTracer* p : points) {
        customplot_->removeItem(p);
    }
    indicator_points_[curve].clear();
    customplot_->replot();
}

void LinePlot::ToggleVisible(QString curve, bool visible) {
    if (!CurvesExist(curve)) {
        qWarning() << "LinePlot::ToggleVisible curve does not exist: " << curve;
        return;
    }
    curves_[curve]->setVisible(visible);
    customplot_->replot();
}

bool LinePlot::IsVisible(QString curve) const {
    if (!CurvesExist(curve)) {
        qWarning() << "LinePlot::IsVisible curve does not exist: " << curve;
        return false;
    }
    return curves_.at(curve)->visible();
}

void LinePlot::ShowHorisontalDistanceToZero(QString curve, double pos, QString label, double y_line_offset, double y_label_offset) {
    QCPItemTracer *tracer_1 = new QCPItemTracer(customplot_);
    tracer_1->setGraph(curves_.at(curve));
    tracer_1->setInterpolating(true);
    tracer_1->setStyle(QCPItemTracer::tsCircle);
    tracer_1->setPen(curves_[curve]->pen());
    tracer_1->setSize(7);
    tracer_1->setGraphKey(pos);
    tracer_1->updatePosition();
    indicator_points_[curve].push_back(tracer_1);

    QCPItemLine *hor = new QCPItemLine(customplot_);
    hor->setLayer("overlay");
    hor->setClipToAxisRect(false);
    hor->setParent(tracer_1);
    hor->setHead(QCPLineEnding::esFlatArrow);
    hor->setTail(QCPLineEnding::esFlatArrow);
    hor->setPen(curves_[curve]->pen());

    hor->start->setTypeX(QCPItemPosition::ptPlotCoords);
    hor->start->setTypeY(QCPItemPosition::ptAbsolute);
    hor->end->setTypeX(QCPItemPosition::ptPlotCoords);
    hor->end->setTypeY(QCPItemPosition::ptAbsolute);
    hor->start->setParentAnchorY(tracer_1->position);
    hor->end->setParentAnchorY(tracer_1->position);
    hor->start->setCoords(0.0, y_line_offset);
    hor->end->setCoords(tracer_1->position->coords().x(), y_line_offset);

    QPen ver_pen = curves_[curve]->pen();
    ver_pen.setStyle(Qt::DashLine);

    QCPItemLine *ver1 = new QCPItemLine(customplot_);
    ver1->setLayer("overlay");
    ver1->setClipToAxisRect(false);
    ver1->setParent(tracer_1);
    ver1->setHead(QCPLineEnding::esNone);
    ver1->setTail(QCPLineEnding::esNone);
    ver1->setPen(ver_pen);
    ver1->start->setParentAnchor(tracer_1->position);
    ver1->end->setParentAnchor(hor->end);

    QCPItemText *text = new QCPItemText(customplot_);
    text->setParent(tracer_1);
    text->position->setParentAnchor(hor->start);
    text->position->setTypeX(QCPItemPosition::ptPlotCoords);
    text->position->setTypeY(QCPItemPosition::ptAbsolute);
    text->position->setCoords(0.5 * (hor->start->coords().x() + hor->end->coords().x()), y_label_offset);
    text->setText(label);
    text->setColor(curves_[curve]->pen().color());

    customplot_->replot();
}

void LinePlot::ShowHorisontalDistance(QString curve, double pos1, double pos2, QString label, double y_line_offset, double y_label_offset) {
    QCPItemTracer *tracer_1 = new QCPItemTracer(customplot_);
    tracer_1->setGraph(curves_.at(curve));
    tracer_1->setInterpolating(true);
    tracer_1->setStyle(QCPItemTracer::tsCircle);
    tracer_1->setPen(curves_[curve]->pen());
    tracer_1->setSize(7);
    tracer_1->setGraphKey(pos1);
    tracer_1->updatePosition();
    indicator_points_[curve].push_back(tracer_1);

    QCPItemTracer *tracer_2 = new QCPItemTracer(customplot_);
    tracer_2->setGraph(curves_.at(curve));
    tracer_2->setInterpolating(true);
    tracer_2->setStyle(QCPItemTracer::tsCircle);
    tracer_2->setPen(curves_[curve]->pen());
    tracer_2->setSize(7);
    tracer_2->setGraphKey(pos2);
    tracer_2->updatePosition();
    indicator_points_[curve].push_back(tracer_2);

    QCPItemLine *hor = new QCPItemLine(customplot_);
    hor->setLayer("overlay");
    hor->setClipToAxisRect(false);
    hor->setParent(tracer_1);
    hor->setHead(QCPLineEnding::esFlatArrow);
    hor->setTail(QCPLineEnding::esFlatArrow);
    hor->setPen(curves_[curve]->pen());

    hor->start->setTypeX(QCPItemPosition::ptPlotCoords);
    hor->start->setTypeY(QCPItemPosition::ptAbsolute);
    hor->end->setTypeX(QCPItemPosition::ptPlotCoords);
    hor->end->setTypeY(QCPItemPosition::ptAbsolute);
    if (tracer_1->position->coords().y() < tracer_2->position->coords().y()) {
        hor->start->setParentAnchorY(tracer_1->position);
        hor->end->setParentAnchorY(tracer_1->position);
    } else {
        hor->start->setParentAnchorY(tracer_2->position);
        hor->end->setParentAnchorY(tracer_2->position);
    }
    hor->start->setCoords(tracer_1->position->coords().x(), y_line_offset);
    hor->end->setCoords(tracer_2->position->coords().x(), y_line_offset);

    QPen ver_pen = curves_[curve]->pen();
    ver_pen.setStyle(Qt::DashLine);

    QCPItemLine *ver1 = new QCPItemLine(customplot_);
    ver1->setLayer("overlay");
    ver1->setClipToAxisRect(false);
    ver1->setParent(tracer_1);
    ver1->setHead(QCPLineEnding::esNone);
    ver1->setTail(QCPLineEnding::esNone);
    ver1->setPen(ver_pen);
    ver1->start->setParentAnchor(tracer_1->position);
    ver1->end->setParentAnchor(hor->start);

    QCPItemLine *ver2 = new QCPItemLine(customplot_);
    ver2->setLayer("overlay");
    ver2->setClipToAxisRect(false);
    ver2->setParent(tracer_2);
    ver2->setHead(QCPLineEnding::esNone);
    ver2->setTail(QCPLineEnding::esNone);
    ver2->setPen(ver_pen);
    ver2->start->setParentAnchor(tracer_2->position);
    ver2->end->setParentAnchor(hor->end);

    QCPItemText *text = new QCPItemText(customplot_);
    text->setParent(tracer_1);
    text->position->setParentAnchor(hor->start);
    text->position->setTypeX(QCPItemPosition::ptPlotCoords);
    text->position->setTypeY(QCPItemPosition::ptAbsolute);
    text->position->setCoords(0.5 * (hor->start->coords().x() + hor->end->coords().x()), y_label_offset);
    text->setText(label);
    text->setColor(curves_[curve]->pen().color());

    customplot_->replot();
}

void LinePlot::SetIndicatorPoints(QString curve, std::vector<double> x) {
    qDebug() << "LinePlot::SetIndicatorPoints " << curve;
    if (!CurvesExist(curve)) {
        qWarning() << "LinePlot::SetIndicatorPoints curve does not exist: " << curve;
        return;
    }

    for (double x_i : x) {
        QCPItemTracer *tracer = new QCPItemTracer(customplot_);
        tracer->setGraph(curves_.at(curve));
        tracer->setInterpolating(true);
        tracer->setStyle(QCPItemTracer::tsCircle);
        tracer->setPen(curves_[curve]->pen());
        tracer->setSize(7);
        tracer->setGraphKey(x_i);
        indicator_points_[curve].push_back(tracer);

        QCPItemText *text = new QCPItemText(customplot_);
        text->setParent(tracer);
        text->position->setParentAnchor(tracer->position);
        text->position->setCoords(-10, -30);
        text->setText("m98");

        QCPItemLine *arrow = new QCPItemLine(customplot_);
        arrow->setLayer("overlay");
        arrow->setClipToAxisRect(false);
        arrow->setHead(QCPLineEnding::esSpikeArrow);
        arrow->end->setParentAnchor(tracer->position);
        arrow->start->setParentAnchor(text->bottom);
     //   arrow->position->setCoords(0, -5);
    }
    customplot_->replot();
}

QPen LinePlot::Pen(int line_width) const {
    QPen pen;
    const int n_plots(static_cast<int>(curves_.size()));
    pen.setWidth(line_width);

    switch (n_plots) {
    case 0:
        pen.setColor("deepskyblue");
        break;
    case 1:
        pen.setColor("lawngreen");
        break;
    case 2:
        pen.setColor("orangered");
        break;
    case 3:
        pen.setColor("lightgray");
        break;
    case 4:
        pen.setColor("yellow");
        break;
    case 5:
        pen.setColor("cyan");
        break;
    default:
        pen.setColor("magenta");
        break;
    }
    return pen;
}

void LinePlot::InitCurve(QString curve, Axis axis, int line_width, Qt::PenStyle style) {
    if (CurvesExist(curve)) {
        qWarning() << "LinePlot::InitCurve curve exist, skipping";
        return;
    }

    principal_axis_[curve] = axis;
    indicator_points_[curve] = std::vector<QCPItemTracer*>();

    QPen pen = Pen(line_width);
    pen.setStyle(style);
    curves_[curve] = new QCPGraph(customplot_->xAxis, customplot_->yAxis);
    curves_[curve]->setName(curve);
    curves_[curve]->setPen(pen);
    curves_[curve]->setLineStyle(QCPGraph::lsLine);
    curves_[curve]->setVisible(true);
    curves_[curve]->selectionDecorator()->setPen(pen);

    customplot_->update();
    Plot();
}

void LinePlot::Clear() {
    for ( const auto &p : curves_) {
        Clear(p.first);
    }
}

void LinePlot::Clear(QString curve) {
    if (CurvesExist(curve)) {
        RemoveIndicatorPoints(curve);
        curves_[curve]->data()->clear();
        RescaleAxis();
        Plot();
    }
}

void LinePlot::Remove() {
    customplot_->clearPlottables();
    curves_.clear();
    principal_axis_.clear();
    Plot();
}

void LinePlot::Remove(QString curve) {
    if (CurvesExist(curve)) {
        RemoveIndicatorPoints(curve);
        customplot_->removePlottable(curves_[curve]);
        curves_.erase(curve);
        principal_axis_.erase(curve);
        RescaleAxis();
        Plot();
    }
}

void LinePlot::SetRange(double hor_min, double hor_max, double ver_min, double ver_max) {
    customplot_->xAxis->setRange(hor_min, hor_max);
    customplot_->yAxis->setRange(ver_min, ver_max);
    min_ = ver_min;
    max_ = ver_max;
    Plot();
}

 void LinePlot::SetHorRange(double hor_min, double hor_max) {
     customplot_->xAxis->setRange(hor_min, hor_max);
     Plot();
 }

 void LinePlot::SetHorLowerRange(double value) {
     customplot_->xAxis->setRangeLower(value);
     Plot();
 }

 void LinePlot::SetHorUpperRange(double value) {
     customplot_->xAxis->setRangeUpper(value);
     Plot();
 }

 void LinePlot::AddPoint(QString curve, MeasurementPoint p, double noise) {
     if (CurvesExist(curve)) {
         const double value = p.signal().value(noise);
         curves_[curve]->addData(p.pos(principal_axis_[curve]), value);
         if (value < min_) {
             min_ = value - 0.15*std::abs(value);
             customplot_->yAxis->setRangeLower(min_);
         }
         if (value > max_) {
             max_ = value + 0.15*std::abs(value);
             customplot_->yAxis->setRangeUpper(max_);
         }
         Plot();
     }
}

void LinePlot::SetCurves(QString curve, BeamMeasurement m) {
    if (CurvesExist(curve)) {
        QCPCurveDataContainer data = m.GetIntensityCurve(principal_axis_[curve]);
        curves_[curve]->data()->set(CurveToGraphData(data));
        RescaleAxis();
        Plot();
    }
}

QCPGraphDataContainer LinePlot::CurveToGraphData(QCPCurveDataContainer curve_data) const {
    QCPGraphDataContainer graph_data;
    for (QCPCurveData d : curve_data) {
        graph_data.add(QCPGraphData(d.mainKey(), d.mainValue()));
    }
    return graph_data;
}

bool LinePlot::CurvesExist(QString curve) const {
    return (principal_axis_.find(curve) != principal_axis_.end());
}

void LinePlot::Plot() {
    customplot_->replot();
}

void LinePlot::RescaleAxis() {
    bool one_range_found(false);
    double lower = 0.0;
    double upper = 0.0;
    for (auto const& c : curves_) {
        bool ok(false);
        QCPRange r = c.second->getValueRange(ok);
        if (ok) {
            if (!one_range_found) {
                lower = r.lower;
                upper = r.upper;
                one_range_found = true;
            } else {
                lower = std::min(lower, r.lower);
                upper = std::max(upper, r.upper);
            }
        }
    }
    if (one_range_found) {
        min_ = lower - 0.15*std::abs(lower);
        max_ = upper + 0.15*std::abs(upper);
        customplot_->yAxis->setRange(min_, max_);
    }
}

void LinePlot::DisplayMeasurementDialog(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event) {
    (void)dataIndex;
    (void)event;
    LinePlotMeasurementDialog dialog(nullptr, customplot_,  qobject_cast<QCPGraph*>(plottable));
    dialog.exec();
    customplot_->replot();
}








