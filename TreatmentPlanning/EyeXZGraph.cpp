#include "EyeXZGraph.h"

#include <QVector>

#include "Compensateur.h"
#include "Calc.h"
#include "Material.h"

EyeXZGraph::EyeXZGraph(QCustomPlot* customplot)
    : customplot_(customplot) {
    customplot_->legend->setVisible(false);
    customplot_->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_->yAxis->setLabel("Z [mm tissue]");
    customplot_->xAxis->setLabel("X [mm]");

    QColor fg_color = Qt::white;
    QColor bg_color = QColor("#31363b");

    customplot_->setBackground(bg_color);

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

    if (customplot_->plotLayout()->rowCount() == 1) {
        customplot_->plotLayout()->insertRow(0);
        customplot_->plotLayout()->addElement(0, 0, new QCPTextElement(customplot_, "Eye X-Z", QFont("Century Schoolbook L", 12)));
        qobject_cast<QCPTextElement*>(customplot_->plotLayout()->element(0, 0))->setTextColor(fg_color);
    }
    customplot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);



    ConnectSignals();
}

EyeXZGraph::~EyeXZGraph() {
    customplot_->clearGraphs();
}

void EyeXZGraph::ConnectSignals() {
    QObject::connect(customplot_, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    QObject::connect(customplot_, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
}

void EyeXZGraph::Plot(const Compensateur& compensateur, const double& modulation) {
    customplot_->clearPlottables();

    DrawEyeBall(compensateur.eye());
    DrawSclere(compensateur.eye());
    bool found;
    DrawBolus(compensateur.eye(),
              eyeXZ_curve_->getValueRange(found).upper,
              compensateur.bolus_thickness(),
              eyeXZ_curve_->getKeyRange(found).lower,
              eyeXZ_curve_->getKeyRange(found).upper);

    const double max_z(eyeXZ_curve_->getValueRange(found).upper + compensateur.bolus_thickness());
    DrawBeam(compensateur, modulation, max_z, sclereXZ_curve_->getKeyRange(found));

    customplot_->xAxis->setRangeUpper(1.2 * max_z);
    customplot_->xAxis->setRangeLower(-1.2 * max_z);
    customplot_->yAxis->setRangeUpper(1.2 * max_z);
    customplot_->yAxis->setRangeLower(-1.2 * max_z);

    customplot_->replot();
}

void EyeXZGraph::DrawBeam(const Compensateur& compensateur, const double& modulation,
                          double max_z, QCPRange sclere_xz_range) {
    QCPCurveDataContainer beam;
    // pre-modulated beam
    const double delta(compensateur.eye().sclere_thickness() / 2.0);
    for (double x = sclere_xz_range.lower - delta; x <= sclere_xz_range.upper + delta; x += 0.01) {
        const double comp_mm_tissue(material::Plexiglas2Tissue(compensateur.mm_plexiglas(std::fabs(x))));
        const double penetration_depth(std::max(compensateur.parcours() - comp_mm_tissue - modulation, 0.0));
        beam.add(QCPCurveData(x, x, max_z - penetration_depth));
    }
    // post-modulated beam
    double last_key(beam.at(beam.size() - 1)->sortKey());
    for (double x = sclere_xz_range.upper + delta; x >= sclere_xz_range.lower - delta; x -= 0.01) {
        const double comp_mm_tissue(material::Plexiglas2Tissue(compensateur.mm_plexiglas(std::fabs(x))));
        const double penetration_depth(std::max(compensateur.parcours() - comp_mm_tissue, 0.0));
        beam.add(QCPCurveData(last_key += 0.01, x, max_z - penetration_depth));
    }
    beam.add(QCPCurveData(last_key += 0.01, beam.at(0)->mainKey(), beam.at(0)->mainValue()));
    beamXZ_curve_ = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
    beamXZ_curve_->data()->set(beam);
    beamXZ_curve_->setPen(QPen(QColor(220,20,60,255), 1.0)); // red
    beamXZ_curve_->setLineStyle(QCPCurve::lsLine);
    beamXZ_curve_->setScatterStyle(QCPScatterStyle::ssNone);
    beamXZ_curve_->setBrush(QBrush(QColor(220,20,60,80), Qt::SolidPattern)); // first graph will be filled with translucent blue
    beamXZ_curve_->setVisible(true);
}


void EyeXZGraph::DrawBolus(const Eye& eye, double z, double dz, double left, double right) {
    bolusXZ_curve_ = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
    QCPCurveDataContainer eye_data(eye.EyeCurve(M_PI, 0.0));
    double last_t(eye_data.at(eye_data.size() - 1)->t);
    bolusXZ_curve_->data()->set(eye_data);
    bolusXZ_curve_->addData(last_t + 1, right, z + dz);
    bolusXZ_curve_->addData(last_t + 2, left, z + dz);
    bolusXZ_curve_->addData(last_t + 3, left, 0.0);
    bolusXZ_curve_->setPen(QPen(QColor(153, 202, 83, 255), 1.5));
    bolusXZ_curve_->setBrush(QBrush(QColor(153, 202, 83, 120)));
    bolusXZ_curve_->setLineStyle(QCPCurve::lsLine);
    bolusXZ_curve_->setScatterStyle(QCPScatterStyle::ssNone);
    bolusXZ_curve_->setVisible(true);
}

void EyeXZGraph::DrawEyeBall(const Eye& eye) {
    eyeXZ_curve_ = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
    eyeXZ_curve_->data()->set(eye.EyeCurve(2.0 * M_PI, 0.0));
    eyeXZ_curve_->setPen(QPen(QColor(246, 166, 37, 255), 2));//   QColor(32, 159, 223, 255), 2));
    eyeXZ_curve_->setBrush(QBrush(QColor(246, 166, 37, 40)));        //QBrush(QColor(32, 159, 223, 40)));
    eyeXZ_curve_->setLineStyle(QCPCurve::lsLine);
    eyeXZ_curve_->setScatterStyle(QCPScatterStyle::ssNone);
    eyeXZ_curve_->setVisible(true);
}

void EyeXZGraph::DrawSclere(const Eye& eye) {
    sclereXZ_curve_ = new QCPCurve(customplot_->xAxis, customplot_->yAxis);
    sclereXZ_curve_->data()->set(eye.SclereCurve(2.0 * M_PI, 0.0));
    sclereXZ_curve_->setPen(QPen(QColor(246, 166, 37, 255), 2));  //QPen(QColor(32, 159, 223, 255), 2));
    sclereXZ_curve_->setBrush(QBrush(QColor(246, 166, 37, 80))); //QBrush(QColor(32, 159, 223, 80)));
    sclereXZ_curve_->setLineStyle(QCPCurve::lsLine);
    sclereXZ_curve_->setScatterStyle(QCPScatterStyle::ssNone);
    sclereXZ_curve_->setVisible(true);
}

void EyeXZGraph::mousePress()
{
    customplot_->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void EyeXZGraph::mouseWheel()
{
    customplot_->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
