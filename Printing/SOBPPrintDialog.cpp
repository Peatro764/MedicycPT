#include "SOBPPrintDialog.h"
#include "ui_SOBPPrintDialog.h"

#include <QDateTime>

#include "Calc.h"
#include "Material.h"
#include "Hardware.h"

SOBPPrintDialog::SOBPPrintDialog(QWidget *parent, QARepo* repo) :
    QDialog(parent),
    ui_(new Ui::SOBPPrintDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setWindowTitle(QString("SOBP"));
    SetupConnections();
    SetupGraphs();
}

SOBPPrintDialog::~SOBPPrintDialog() {
    delete ui_;
}

void SOBPPrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void SOBPPrintDialog::SetupGraphs() {
    sobp_graph_ = std::unique_ptr<LinePlot>(new LinePlot(ui_->customPlot_sobp));
    sobp_graph_->SetLightStyle();
    sobp_graph_->InitCurve(tissue_graph_, Axis::Z, 2, Qt::SolidLine);
    sobp_graph_->InitCurve(plexi_graph_, Axis::Z, 2, Qt::DashLine);
    sobp_graph_->SetRange(0.0, 32.0, 0.0, 0.1);
}

void SOBPPrintDialog::Load(int dossier) {
    SOBP sobp_tissue = repo_->GetSOBPForDossier(dossier);
    Fill(sobp_tissue);
}

void SOBPPrintDialog::LoadLastSOBP() {
    SOBP sobp_tissue = repo_->GetLastSOBP();
    Fill(sobp_tissue);
}

void SOBPPrintDialog::Fill(const SOBP& sobp) {
    DepthDoseMeasurement dd = sobp.depth_dose();
    sobp_graph_->SetCurves(tissue_graph_, dd);
    sobp_graph_->SetCurves(plexi_graph_, dd.ScaleAxis(Axis::Z, material::ToPlexiglas(1.0, material::MATERIAL::TISSUE)));

    ui_->lineEdit_date->setText(dd.GetTimestamp().toString(QString("yyyy:MM:dd-hh:mm")));
    ui_->lineEdit_hardware->setText(HardwareToString(dd.Hardware()));
    ui_->lineEdit_modulateur->setText(QString::number(sobp.modulateur_id()));
    ui_->lineEdit_degradeur->setText(QString::number(sobp.mm_degradeur(), 'f', 1));
    ui_->lineEdit_dossier->setText(QString::number(sobp.dossier()));

    ui_->lineEdit_parcours_tissue->setText(QString::number(sobp.parcours(), 'f', 2));
    ui_->lineEdit_penumbra_tissue->setText(QString::number(sobp.penumbra(), 'f', 2));
    ui_->lineEdit_mod98_tissue->setText(QString::number(sobp.mod98(), 'f', 2));
    ui_->lineEdit_mod100_tissue->setText(QString::number(sobp.mod100(), 'f', 2));

    ui_->lineEdit_parcours_plexi->setText(QString::number(material::ToPlexiglas(sobp.parcours(), material::MATERIAL::TISSUE), 'f', 2));
    ui_->lineEdit_penumbra_plexi->setText(QString::number(material::ToPlexiglas(sobp.penumbra(), material::MATERIAL::TISSUE), 'f', 2));
    ui_->lineEdit_mod98_plexi->setText(QString::number(material::ToPlexiglas(sobp.mod98(), material::MATERIAL::TISSUE) , 'f', 2));
    ui_->lineEdit_mod100_plexi->setText(QString::number(material::ToPlexiglas(sobp.mod100(), material::MATERIAL::TISSUE) , 'f', 2));
}

QWidget* SOBPPrintDialog::Widget() {
    return ui_->SOBPWidget;
}

void SOBPPrintDialog::Print(QPrinter* printer) {
    printer->setOrientation(QPrinter::Portrait);
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(false);

    QCPPainter painter(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    int plotWidth = ui_->SOBPWidget->width();
    int plotHeight = ui_->SOBPWidget->height();
    double w_scale = pageRect.width() / (double)plotWidth;
    double h_scale = pageRect.height() / (double)plotHeight;

    painter.scale(w_scale, h_scale);
    ui_->SOBPWidget->render(&painter);
    painter.end();
}
