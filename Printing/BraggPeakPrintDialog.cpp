#include "BraggPeakPrintDialog.h"
#include "ui_BraggPeakPrintDialog.h"

#include <QDateTime>

#include "Calc.h"
#include "Material.h"
#include "Hardware.h"

BraggPeakPrintDialog::BraggPeakPrintDialog(QWidget *parent, std::shared_ptr<QARepo> repo) :
    QDialog(parent),
    ui_(new Ui::BraggPeakPrintDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setWindowTitle(QString("BraggPeak"));
    SetupConnections();
    SetupGraphs();
    Fill();
}

BraggPeakPrintDialog::~BraggPeakPrintDialog() {
    delete ui_;
}

void BraggPeakPrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void BraggPeakPrintDialog::SetupGraphs() {
    braggPeak_graph_ = std::unique_ptr<LinePlot>(new LinePlot(ui_->customPlot_BraggPeak));
    braggPeak_graph_->SetLightStyle();
    braggPeak_graph_->SetAxisLabels("Position [mm]", "Intensity [a.u]");
    braggPeak_graph_->InitCurve(tissue_graph_, Axis::Z, 2, Qt::SolidLine);
    braggPeak_graph_->InitCurve(plexi_graph_, Axis::Z, 2, Qt::DashLine);
    braggPeak_graph_->SetRange(0.0, 35.0, 0.0, 0.1);
}

void BraggPeakPrintDialog::Fill() {
    BraggPeak bp = repo_->GetLastBraggPeak();
    DepthDoseMeasurement dd = bp.depth_dose();

    braggPeak_graph_->SetCurves(tissue_graph_, dd);
    braggPeak_graph_->SetCurves(plexi_graph_, dd.ScaleAxis(Axis::Z, material::ToPlexiglas(1.0, material::MATERIAL::TISSUE)));

    ui_->lineEdit_date->setText(dd.GetTimestamp().toString("yyyy:MM:dd-hh:mm"));
    ui_->lineEdit_hardware->setText(HardwareToString(dd.Hardware()));

    ui_->lineEdit_w50_tissue->setText(QString::number(bp.width50(), 'f', 2));
    ui_->lineEdit_l90_tissue->setText(QString::number(bp.parcours(), 'f', 2));
    ui_->lineEdit_penumbra_tissue->setText(QString::number(bp.penumbra(), 'f', 2));

    ui_->lineEdit_w50_plexi->setText(QString::number(material::ToPlexiglas(bp.width50(), material::MATERIAL::TISSUE), 'f', 2));
    ui_->lineEdit_l90_plexi->setText(QString::number(material::ToPlexiglas(bp.parcours(), material::MATERIAL::TISSUE) , 'f', 2));
    ui_->lineEdit_penumbra_plexi->setText(QString::number(material::ToPlexiglas(bp.penumbra(), material::MATERIAL::TISSUE), 'f', 2));
}

QWidget* BraggPeakPrintDialog::Widget() {
    return ui_->braggPeakWidget;
}

void BraggPeakPrintDialog::Print(QPrinter* printer) {
    printer->setOrientation(QPrinter::Portrait);
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(false);

    QCPPainter painter(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    int plotWidth = ui_->braggPeakWidget->width();
    int plotHeight = ui_->braggPeakWidget->height();
    double w_scale = pageRect.width() / (double)plotWidth;
    double h_scale = pageRect.height() / (double)plotHeight;

    painter.scale(w_scale, h_scale);
    ui_->braggPeakWidget->render(&painter);
    painter.end();
}
