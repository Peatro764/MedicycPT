#include "CollimatorPrintDialog.h"
#include "ui_CollimatorPrintDialog.h"

#include <QDateTime>
#include <algorithm>

#include "Collimator.h"
#include "Calc.h"

CollimatorPrintDialog::CollimatorPrintDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::CollimatorPrintDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Collimateur"));
    SetupConnections();
    Fill();
}

CollimatorPrintDialog::~CollimatorPrintDialog()
{
    delete ui_;
}

void CollimatorPrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void CollimatorPrintDialog::Fill() {
    ui_->dateLineEdit->setText(QDateTime::currentDateTime().toString());
    FillDossier();
    Collimator coll(repo_->GetCollimator(dossier_));
    FillCoordinates(coll);
    ui_->collimator_drawing->PaintCollimator(coll);
}

void CollimatorPrintDialog::FillDossier() {
    Patient patient(repo_->GetPatient(dossier_));
    ui_->nameLineEdit->setText(patient.GetLastName().toUpper() + " " + patient.GetFirstName());
    ui_->dossierLineEdit->setText(QString::number(dossier_));
}

void CollimatorPrintDialog::FillCoordinates(Collimator& coll) {
    for (auto coord : coll.InternalEdges()) {
        ui_->coordinatesPlainTextEdit->appendPlainText(QString::number(coord.x()) + ", " +
                                                       QString::number(coord.y()));
    }
}

QWidget* CollimatorPrintDialog::Widget() {
    return ui_->collimatorWidget;
}

void CollimatorPrintDialog::Print(QPrinter* printer) {
    printer->setOrientation(QPrinter::Portrait);
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(false);

    QCPPainter painter(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    int plotWidth = ui_->collimatorWidget->width();
    int plotHeight = ui_->collimatorWidget->height();
    double w_scale = pageRect.width() / (double)plotWidth;
    double h_scale = pageRect.height() / (double)plotHeight;

    painter.scale(w_scale, h_scale);
    ui_->collimatorWidget->render(&painter);
    painter.end();
}
