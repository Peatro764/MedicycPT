#include "CompensateurTransparantDialog.h"
#include "ui_CompensateurTransparantDialog.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QDateTime>

#include "Calc.h"
#include "Material.h"

CompensateurTransparantDialog::CompensateurTransparantDialog(QWidget *parent, const Compensateur& compensateur, const Patient& patient) :
    QDialog(parent),
    ui_(new Ui::CompensateurTransparantDialog)
{
    ui_->setupUi(this);

    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui_->printPushButton, SIGNAL(clicked()), this, SLOT(Print()));

    setWindowTitle("Compensateur Transparant");

    Draw(compensateur, patient);
}

void CompensateurTransparantDialog::Draw(const Compensateur& compensateur, const Patient& patient) {
    ui_->customplot->plotLayout()->clear();
    ui_->customplot->replot();

    QCPLayoutGrid *sublayout_column0 = new QCPLayoutGrid;
    QCPLayoutGrid *sublayout_column1 = new QCPLayoutGrid;
    QCPLayoutGrid *sublayout_column2 = new QCPLayoutGrid;
    QCPLayoutGrid *sublayout_column2b = new QCPLayoutGrid;
    ui_->customplot->plotLayout()->addElement(0, 0, sublayout_column0);
    ui_->customplot->plotLayout()->addElement(0, 1, sublayout_column1);
    ui_->customplot->plotLayout()->addElement(0, 2, sublayout_column2);

    QCPAxisRect *axisrect_r0c0 = new QCPAxisRect(ui_->customplot, false); // false means to not setup default axes
    QCPAxisRect *axisrect_r1c0 = new QCPAxisRect(ui_->customplot, false); // false means to not setup default axes
    QCPAxisRect *axisrect_r0c1 = new QCPAxisRect(ui_->customplot, false); // false means to not setup default axes
    QCPAxisRect *axisrect_r0c2 = new QCPAxisRect(ui_->customplot, false); // false means to not setup default axes
    QCPAxisRect *axisrect_r1c3 = new QCPAxisRect(ui_->customplot, false); // false means to not setup default axes
    QCPTextElement *mmplexi_list = new QCPTextElement(ui_->customplot);
    QCPTextElement *dossier_list = new QCPTextElement(ui_->customplot);
    QCPTextElement  *title_r0c0 = new QCPTextElement(ui_->customplot);
    QCPTextElement  *title_r1c0 = new QCPTextElement(ui_->customplot);
    QCPTextElement  *title_r0c1 = new QCPTextElement(ui_->customplot);
    QCPTextElement  *title_r0c2 = new QCPTextElement(ui_->customplot);

    axisrect_r0c0->setMinimumSize(188 + 50 + 31, 188 + 10 + 31);
    axisrect_r0c0->setMaximumSize(188 + 50 + 31, 188 + 10 + 31);

    axisrect_r1c0->setMinimumSize(188 + 50 + 31, 188 + 10 + 31);
    axisrect_r1c0->setMaximumSize(188 + 50 + 31, 188 + 10 + 31);

    axisrect_r0c1->setMinimumSize(90 + 2 * 30, 90 + 10 + 30);
    axisrect_r0c1->setMaximumSize(90 + 2 * 30, 90 + 10 + 30);

    mmplexi_list->setMinimumSize(90 + 2 * 30, 630 - 90 - 2 * 30 - 100);
    mmplexi_list->setMaximumSize(90 + 2 * 30, 630 - 90 - 2 * 30);

    axisrect_r0c2->setMinimumSize(360 + 2 * 30, 360 + 10 + 30);
    axisrect_r0c2->setMaximumSize(360 + 2 * 30, 360 + 10 + 30);

    axisrect_r1c3->setMinimumSize(45 * 3, 45 * 3);
    axisrect_r1c3->setMaximumSize(45 * 3, 45 * 3);

    dossier_list->setMinimumSize(0, 0);

    QString comp_coord;
    comp_coord.append("X (mm) \tY (mm plexi)\n");
    QCPCurveDataContainer fraiseuse_coord(compensateur.FraiseuseFormat());
    for (auto it = fraiseuse_coord.constBegin(); it < fraiseuse_coord.constEnd(); ++it) {
        comp_coord.append(QString::number(it->mainKey(), 'f', 2)).append("\t\t").append(QString::number(it->mainValue(), 'f', 2)).append("\n");
    }
    mmplexi_list->setText(comp_coord);
    mmplexi_list->setTextFlags((int)Qt::AlignTop);

    QString dossier_info;
    dossier_info.append(patient.GetLastName().toUpper() + " " + patient.GetFirstName() + " \t" + QString::number(compensateur.dossier()) + "\n");
    dossier_info.append(QDateTime::currentDateTime().toString() + "\n");
    dossier_info.append("Rappel des donnees (mm)\n");
    dossier_info.append("D_OEIL = " + QString::number(compensateur.eye().diameter(), 'f', 2) + " \tE_SCLERE = " + QString::number(compensateur.eye().sclere_thickness(), 'f', 2) + "\n");
    dossier_info.append("E_PAUP = " + QString::number(compensateur.bolus_thickness(), 'f', 2) + " \tPAR MAX = " + QString::number(compensateur.parcours(), 'f', 2) + "\n");
    dossier_info.append("X0 = " + QString::number(compensateur.center_point().mainKey(), 'f', 2) + " \tY0 = " + QString::number(compensateur.center_point().mainValue(), 'f', 2));
    dossier_list->setText(dossier_info);
    dossier_list->setFont(QFont("sans", 10));
    dossier_list->setTextFlags((int)Qt::AlignLeft);

    title_r0c0->setText("Compensateur Ech 2.09:1");
    title_r1c0->setText("Compensateur Ech 2.09:1");
    title_r0c1->setText("Oeil Ech 1:1");
    title_r0c2->setText("Isodoses Ech 4:1");

    axisrect_r0c0->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
    axisrect_r0c0->axis(QCPAxis::atBottom)->setRange(QCPRange(-15.0, 15.0));
    axisrect_r0c0->axis(QCPAxis::atLeft)->setRange(QCPRange(0.0, 30.0));

    axisrect_r0c0->setAutoMargins(QCP::MarginSide::msNone);
    axisrect_r0c0->setMargins(QMargins(50, 10, 31, 31));
    title_r0c0->setAutoMargins(QCP::MarginSide::msNone);
    title_r0c0->setMargins(QMargins(0, 40, 0, 0));

    axisrect_r1c0->setupFullAxesBox(true);
    axisrect_r1c0->axis(QCPAxis::atBottom)->setRange(QCPRange(-15.0, 15.0));
    axisrect_r1c0->axis(QCPAxis::atLeft)->setRange(QCPRange(0.0, 30.0));
    axisrect_r1c0->axis(QCPAxis::atBottom)->setLabel("X (mm)");
    axisrect_r1c0->axis(QCPAxis::atLeft)->setLabel("Y (mm)");
    axisrect_r1c0->setAutoMargins(QCP::MarginSide::msNone);
    axisrect_r1c0->setMargins(QMargins(50, 10, 31, 31));
    title_r1c0->setAutoMargins(QCP::MarginSide::msNone);
    title_r1c0->setMargins(QMargins(0, 40, 0, 0));

    axisrect_r0c1->setupFullAxesBox(true);
    axisrect_r0c1->axis(QCPAxis::atBottom)->setRange(QCPRange(-15.0, 15.0));
    axisrect_r0c1->axis(QCPAxis::atLeft)->setRange(QCPRange(0.0, 30.0));
    axisrect_r0c1->axis(QCPAxis::atBottom)->setLabel("X (mm)");
    axisrect_r0c1->axis(QCPAxis::atLeft)->setLabel("Y (mm)");
    axisrect_r0c1->axis(QCPAxis::atLeft)->setTickLabelFont(QFont("sans", 6));
    axisrect_r0c1->axis(QCPAxis::atBottom)->setTickLabelFont(QFont("sans", 6));
    axisrect_r0c1->setAutoMargins(QCP::MarginSide::msNone);
    axisrect_r0c1->setMargins(QMargins(30, 10, 30, 30));
    title_r0c1->setAutoMargins(QCP::MarginSide::msNone);
    title_r0c1->setMargins(QMargins(0, 40, 0, 0));

    axisrect_r0c2->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
    axisrect_r0c2->axis(QCPAxis::atBottom)->setRange(QCPRange(-15.0, 15.0));
    axisrect_r0c2->axis(QCPAxis::atLeft)->setRange(QCPRange(0.0, 30.0));
    axisrect_r0c2->axis(QCPAxis::atBottom)->setLabel("X (mm)");
    axisrect_r0c2->axis(QCPAxis::atLeft)->setLabel("Y (mm)");
    axisrect_r0c2->setAutoMargins(QCP::MarginSide::msNone);
    axisrect_r0c2->setMargins(QMargins(30, 10, 30, 30));
    title_r0c2->setAutoMargins(QCP::MarginSide::msNone);
    title_r0c2->setMargins(QMargins(0, 40, 0, 0));

    axisrect_r1c3->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
    axisrect_r1c3->axis(QCPAxis::atBottom)->setRange(QCPRange(-45.0 / 2, 45.0 / 2));
    axisrect_r1c3->axis(QCPAxis::atLeft)->setRange(QCPRange(-45.0 / 2, 45.0 / 2));
    axisrect_r1c3->setAutoMargins(QCP::MarginSide::msNone);
    axisrect_r1c3->setMargins(QMargins(0, 0, 0, 0));
    axisrect_r1c3->setVisible(true);
    axisrect_r1c3->axis(QCPAxis::atBottom)->setVisible(false);
    axisrect_r1c3->axis(QCPAxis::atLeft)->setVisible(false);

    sublayout_column0->addElement(0, 0, title_r0c0);
    sublayout_column0->addElement(1, 0, axisrect_r0c0);
    sublayout_column0->addElement(2, 0, title_r1c0);
    sublayout_column0->addElement(3, 0, axisrect_r1c0);
    sublayout_column1->addElement(0, 0, title_r0c1);
    sublayout_column1->addElement(1, 0, axisrect_r0c1);
    sublayout_column1->addElement(2, 0, mmplexi_list);
    sublayout_column2->addElement(0, 0, title_r0c2);
    sublayout_column2->addElement(1, 0, axisrect_r0c2);
    sublayout_column2b->addElement(0, 0, dossier_list);
    sublayout_column2b->addElement(0, 1, axisrect_r1c3);
    sublayout_column2->addElement(2, 0, sublayout_column2b);

    dossier_list->setTextFlags(Qt::AlignLeft);

    compXY_graph_ =  new QCPCurve(axisrect_r0c0->axis(QCPAxis::atBottom), axisrect_r0c0->axis(QCPAxis::atLeft));
    const double comp_outer_radius(compensateur.FraiseuseFormat().at(compensateur.FraiseuseFormat().size() - 1)->mainKey());
    QCPCurveDataContainer outer_circle(MakeCircle(comp_outer_radius, compensateur.center_point(), 0.0, comp_outer_radius));
    compXY_graph_->data()->set(outer_circle);
    compXY_graph_->setLineStyle(QCPCurve::lsLine);
    compXY_graph_->setScatterStyle(QCPScatterStyle::ssNone);
    compXY_graph_->setPen(QPen(Qt::black, 1));
    compXY_graph_->setVisible(true);

    compXZ_graph_ = new QCPCurve(axisrect_r1c0->axis(QCPAxis::atBottom), axisrect_r1c0->axis(QCPAxis::atLeft));
    compXZ_graph_->data()->set(compensateur.FullStepCurve());
    compXZ_graph_->setLineStyle(QCPCurve::lsLine);
    compXZ_graph_->setScatterStyle(QCPScatterStyle::ssNone);
    compXZ_graph_->setPen(QPen(Qt::black, 1));
    compXZ_graph_->setVisible(true);

    eyeXZ_graph_ = new QCPCurve(axisrect_r0c1->axis(QCPAxis::atBottom), axisrect_r0c1->axis(QCPAxis::atLeft));
    QCPCurveDataContainer eye_curve(compensateur.eye().EyeCurve(2.0 * M_PI, 0.0));
    bool found;
    const double eye_offset(eye_curve.valueRange(found).upper);
    Offset(eye_curve, 0.0, eye_offset);
    eyeXZ_graph_->data()->set(eye_curve);
    eyeXZ_graph_->setPen(QPen(Qt::black, 1));
    eyeXZ_graph_->setLineStyle(QCPCurve::lsLine);
    eyeXZ_graph_->setScatterStyle(QCPScatterStyle::ssNone);
    eyeXZ_graph_->setVisible(true);

    sclereXZ_graph_ = new QCPCurve(axisrect_r0c1->axis(QCPAxis::atBottom), axisrect_r0c1->axis(QCPAxis::atLeft));
    QCPCurveDataContainer sclere_curve(compensateur.eye().SclereCurve(2.0 * M_PI, 0.0));
    Offset(sclere_curve, 0.0, eye_offset);
    sclereXZ_graph_->data()->set(sclere_curve);
    sclereXZ_graph_->setPen(QPen(Qt::black, 1));
    sclereXZ_graph_->setLineStyle(QCPCurve::lsLine);
    sclereXZ_graph_->setScatterStyle(QCPScatterStyle::ssNone);
    sclereXZ_graph_->setVisible(true);

    QCPRange sclere_range(sclereXZ_graph_->getKeyRange(found));

    QCPCurveDataContainer beam;
    for (double x = sclere_range.lower + 0.01; x <= sclere_range.upper - 0.01; x += 0.01) {
        const double comp_mm_tissue(material::Plexiglas2Tissue(compensateur.mm_plexiglas(std::fabs(x))));
        double penetration_depth(std::max(compensateur.parcours() - comp_mm_tissue, 0.0));
        penetration_depth -= compensateur.bolus_thickness(); // shift y = 0 to outer eye
        beam.add(QCPCurveData(x, x, penetration_depth));
    }

    isodoseXZ1_graph_ = new QCPCurve(axisrect_r0c1->axis(QCPAxis::atBottom), axisrect_r0c1->axis(QCPAxis::atLeft));
    isodoseXZ1_graph_->data()->set(beam);
    isodoseXZ1_graph_->setPen(QPen(Qt::black, 1));
    isodoseXZ1_graph_->setLineStyle(QCPCurve::lsLine);
    isodoseXZ1_graph_->setScatterStyle(QCPScatterStyle::ssNone);
    isodoseXZ1_graph_->setVisible(true);

    QCPCurveDataContainer beam_with_stoppers;
    beam_with_stoppers.add(QCPCurveData(-15.0, -15.0, compensateur.parcours() - compensateur.bolus_thickness()));
    beam_with_stoppers.add(QCPCurveData(sclere_range.lower, sclere_range.lower, compensateur.parcours() - compensateur.bolus_thickness()));
    for (double x = sclere_range.lower; x <= sclere_range.upper; x += 0.01) {
        const double comp_mm_tissue(material::Plexiglas2Tissue(compensateur.mm_plexiglas(std::fabs(x))));
        double penetration_depth(std::max(compensateur.parcours() - comp_mm_tissue, 0.0));
        penetration_depth -= compensateur.bolus_thickness(); // shift y = 0 to outer eye
        beam_with_stoppers.add(QCPCurveData(x, x, penetration_depth));
    }
    beam_with_stoppers.add(QCPCurveData(15.0, 15.0, compensateur.parcours() - compensateur.bolus_thickness()));
    beam_with_stoppers.add(QCPCurveData(sclere_range.upper, sclere_range.upper, compensateur.parcours() - compensateur.bolus_thickness()));

    isodoseXZ2_graph_ = new QCPCurve(axisrect_r0c2->axis(QCPAxis::atBottom), axisrect_r0c2->axis(QCPAxis::atLeft));
    isodoseXZ2_graph_->data()->set(beam_with_stoppers);
    isodoseXZ2_graph_->setPen(QPen(Qt::black, 1));
    isodoseXZ2_graph_->setLineStyle(QCPCurve::lsLine);
    isodoseXZ2_graph_->setScatterStyle(QCPScatterStyle::ssNone);
    isodoseXZ2_graph_->setVisible(true);

    compensateur_base_ = new QCPCurve(axisrect_r1c3->axis(QCPAxis::atBottom), axisrect_r1c3->axis(QCPAxis::atLeft));
    QCPCurveDataContainer comp_base_data(MakeCircle(45.0/2.0, QCPCurveData(0.0, 0.0, 0.0), 0.0, 0.0));
    compensateur_base_->data()->set(comp_base_data);
    compensateur_base_->setPen(QPen(Qt::black, 1));
    compensateur_base_->setLineStyle(QCPCurve::lsLine);
    compensateur_base_->setScatterStyle(QCPScatterStyle::ssNone);
    compensateur_base_->setVisible(true);

    ui_->customplot->replot();
    axisrect_r0c0->axis(QCPAxis::atLeft)->setOffset(- 188 / 2);
    axisrect_r0c0->axis(QCPAxis::atBottom)->setOffset(std::round(-comp_outer_radius * 188.0 / 30.0));
    axisrect_r0c2->axis(QCPAxis::atLeft)->setOffset(- 360 / 2);
    ui_->customplot->replot();


}

QCPCurveDataContainer CompensateurTransparantDialog::MakeCircle(const double radius, const QCPCurveData& centre, double offset_x, double offset_y) const {
    QCPCurveDataContainer data;
    for (double angle = 0; angle < (2.0 * M_PI); angle += (2 * M_PI / 100.0)) {
        data.add(QCPCurveData(angle, radius * qCos(angle) + centre.key + offset_x, radius * qSin(angle) + centre.value + offset_y));
    }
    return data;
}

void CompensateurTransparantDialog::Offset(QCPCurveDataContainer &data, double offset_x, double offset_y) {
    for (auto it = data.begin(); it < data.end(); ++it) {
        it->key += offset_x;
        it->value += offset_y;
    }
}

void CompensateurTransparantDialog::Print() {
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setFullPage(true);
    printer.setOrientation(QPrinter::Landscape);

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Transparancy"));
    if (dialog->exec() != QDialog::Accepted)
        return;

    try {
        QCPPainter painter(&printer);
        QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);

         int plotWidth = ui_->customplot->viewport().width();
         int plotHeight = ui_->customplot->viewport().height();
         double w_scale = pageRect.width() / (double)plotWidth;
         double h_scale = pageRect.height() / (double)plotHeight;

         painter.setMode(QCPPainter::pmVectorized);
         painter.setMode(QCPPainter::pmNoCaching);
         painter.setMode(QCPPainter::pmNonCosmetic);

         painter.scale(w_scale, h_scale);
         ui_->customplot->toPainter(&painter, plotWidth, plotHeight);

        painter.end();
    }
    catch (std::exception const& exc) {
        qWarning() << "CompensateurTransparantDialog::Print catched an exception: " << exc.what();
        QMessageBox::warning(this, tr("MTP"), "Failed printing transparant");
    }
}

QWidget* CompensateurTransparantDialog::Widget() {
    return ui_->customplot;
}

void CompensateurTransparantDialog::Print(QPrinter* printer) {
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(true);
    printer->setOrientation(QPrinter::Landscape);

    try {
        QCPPainter painter(printer);
        QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

         int plotWidth = ui_->customplot->viewport().width();
         int plotHeight = ui_->customplot->viewport().height();
         double w_scale = pageRect.width() / (double)plotWidth;
         double h_scale = pageRect.height() / (double)plotHeight;

         painter.setMode(QCPPainter::pmVectorized);
         painter.setMode(QCPPainter::pmNoCaching);
         painter.setMode(QCPPainter::pmNonCosmetic);

         painter.scale(w_scale, h_scale);
         ui_->customplot->toPainter(&painter, plotWidth, plotHeight);

        painter.end();
    }
    catch (std::exception const& exc) {
        qWarning() << "CompensateurTransparantDialog::Print catched an exception: " << exc.what();
        QMessageBox::warning(this, tr("MTP"), "Failed printing compensateur");
    }
}
