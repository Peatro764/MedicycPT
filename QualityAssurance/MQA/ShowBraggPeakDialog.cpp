#include "ShowBraggPeakDialog.h"
#include "ui_ShowBraggPeakDialog.h"
#include "Axis.h"
#include "Hardware.h"

#include <vector>


ShowBraggPeakDialog::ShowBraggPeakDialog(QWidget *parent, BraggPeak bp)
    : QDialog(parent),
      ui_(new Ui::ShowBraggPeakDialog),
      braggpeak_(bp) {
    ui_->setupUi(this);
    setWindowTitle("Bragg Peak");

    QObject::connect(ui_->pushButton_close, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui_->pushButton_export_csv, SIGNAL(clicked()), this, SLOT(ExportToCSV()));
    QObject::connect(ui_->pushButton_print, SIGNAL(clicked()), this, SLOT(Print()));

    bp_graph_ = std::shared_ptr<LinePlot>(new LinePlot(ui_->customPlot_braggPeak));
    bp_graph_->InitCurve(AxisToString(Axis::Z), Axis::Z, 1, Qt::SolidLine);
    bp_graph_->SetRange(0, 40, 0, 10);

    MeasurementCurrents currents = bp.depth_dose().GetCurrents();
    bp_graph_->SetCurves(AxisToString(Axis::Z), bp.depth_dose());
    ui_->lineEdit_stripper->setText(QString::number(currents.stripper(), 'f', 1));
    ui_->lineEdit_cf9->setText(QString::number(currents.cf9(), 'f', 1));
    ui_->lineEdit_chambre1->setText(QString::number(currents.chambre1(), 'f', 1));
    ui_->lineEdit_chambre2->setText(QString::number(currents.chambre2(), 'f', 1));
    ui_->lineEdit_diode->setText(QString::number(currents.diode(), 'f', 1));

    ui_->lineEdit_hardware->setText(HardwareToString(bp.depth_dose().Hardware()));

    ui_->lineEdit_date->setText(bp.depth_dose().GetTimestamp().toString("yyyy:MM:dd-hh:mm"));

    try {
        ui_->lineEdit_width50->setText(QString::number(braggpeak_.width50() , 'f', 2));
        ui_->lineEdit_parcours->setText(QString::number(braggpeak_.parcours(), 'f', 2));
        ui_->lineEdit_penumbra->setText(QString::number(braggpeak_.penumbra(), 'f', 2));
    }
    catch (std::exception& exc) {
        qWarning() << "ShowBraggPeakDialog Exception thrown: " << exc.what();
    }
}

ShowBraggPeakDialog::~ShowBraggPeakDialog() {}

void ShowBraggPeakDialog::ExportToCSV() {
    try {
        QString directory = QFileDialog::getSaveFileName(this, "Please choose a directory", "Export", "", 0, 0);
        braggpeak_.depth_dose().Export(directory);
        QMessageBox::information(this, tr("QA"), "Data successfully exported");
    }
    catch (std::exception& exc) {
        qWarning() << "ShowBeamProfileDialog::ExportToCSV Exception thrown " << exc.what();
        QMessageBox::warning(this, tr("QA"), QString::fromStdString(exc.what()));
    }
}

void ShowBraggPeakDialog::Print() {
    QMessageBox::information(this, tr("QA"), "Not yet implemented");
}
