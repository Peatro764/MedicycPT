#include "ShowSOBPDialog.h"
#include "ui_ShowSOBPDialog.h"
#include "Axis.h"
#include "Hardware.h"

#include <vector>


ShowSOBPDialog::ShowSOBPDialog(QWidget *parent, SOBP sobp)
    : QDialog(parent),
      ui_(new Ui::ShowSOBPDialog),
      sobp_(sobp) {
    ui_->setupUi(this);
    setWindowTitle("SOBP");

    QObject::connect(ui_->pushButton_close, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui_->pushButton_export_csv, SIGNAL(clicked()), this, SLOT(ExportToCSV()));
    QObject::connect(ui_->pushButton_print, SIGNAL(clicked()), this, SLOT(Print()));

    sobp_graph_ = std::shared_ptr<LinePlot>(new LinePlot(ui_->customPlot_sobp));
    sobp_graph_->InitCurve(AxisToString(Axis::Z), Axis::Z, 1, Qt::SolidLine);
    sobp_graph_->SetRange(0, 40, 0, 10);

    MeasurementCurrents currents = sobp.depth_dose().GetCurrents();
    sobp_graph_->SetCurves(AxisToString(Axis::Z), sobp.depth_dose());
    ui_->lineEdit_stripper->setText(QString::number(currents.stripper(), 'f', 1));
    ui_->lineEdit_cf9->setText(QString::number(currents.cf9(), 'f', 1));
    ui_->lineEdit_chambre1->setText(QString::number(currents.chambre1(), 'f', 1));
    ui_->lineEdit_chambre2->setText(QString::number(currents.chambre2(), 'f', 1));
    ui_->lineEdit_diode->setText(QString::number(currents.diode(), 'f', 1));

    ui_->lineEdit_hardware->setText(HardwareToString(sobp.depth_dose().Hardware()));

    ui_->lineEdit_date->setText(sobp.depth_dose().GetTimestamp().toString("yyyy:MM:dd-hh:mm"));

    try {
        ui_->lineEdit_parcours->setText(QString::number(sobp.parcours(), 'f', 2));
        ui_->lineEdit_penumbra->setText(QString::number(sobp.penumbra(), 'f', 2));
        ui_->lineEdit_mod98->setText(QString::number(sobp.mod98(), 'f', 2));
        ui_->lineEdit_mod100->setText(QString::number(sobp.mod100(), 'f', 2));
    }
    catch (std::exception& exc) {
        qWarning() << "ShowSOBPDialog Exception thrown: " << exc.what();
    }
}

ShowSOBPDialog::~ShowSOBPDialog() {}

void ShowSOBPDialog::ExportToCSV() {
    try {
        QString directory = QFileDialog::getSaveFileName(this, "Please choose a directory", "Export", "", 0, 0);
        sobp_.depth_dose().Export(directory);
        QMessageBox::information(this, tr("QA"), "Data successfully exported");
    }
    catch (std::exception& exc) {
        qWarning() << "ShowSOBPDialog::ExportToCSV Exception thrown " << exc.what();
        QMessageBox::warning(this, tr("QA"), QString::fromStdString(exc.what()));
    }
}

void ShowSOBPDialog::Print() {
    QMessageBox::information(this, tr("QA"), "Not yet implemented");
}
