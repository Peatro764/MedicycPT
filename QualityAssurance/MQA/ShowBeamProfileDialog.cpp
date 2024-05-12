#include "ShowBeamProfileDialog.h"
#include "ui_ShowBeamProfileDialog.h"
#include "Axis.h"
#include "Hardware.h"

#include <vector>


ShowBeamProfileDialog::ShowBeamProfileDialog(QWidget *parent, BeamProfile profile)
    : QDialog(parent),
      ui_(new Ui::ShowBeamProfileDialog),
      profile_(profile) {
    ui_->setupUi(this);
    setWindowTitle("Beam Profile");

    QObject::connect(ui_->pushButton_close, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui_->pushButton_export_csv, SIGNAL(clicked()), this, SLOT(ExportToCSV()));
    QObject::connect(ui_->pushButton_print, SIGNAL(clicked()), this, SLOT(Print()));

    profile_graph_ = std::shared_ptr<LinePlot>(new LinePlot(ui_->customPlot_beamProfile));
    profile_graph_->InitCurve(AxisToString(profile.GetAxis()), profile.GetAxis(), 1, Qt::SolidLine);
    profile_graph_->SetRange(-25, 25, 0, 0.1);

    MeasurementCurrents currents = profile.GetCurrents();
    profile_graph_->SetCurves(AxisToString(profile.GetAxis()), profile);
    ui_->lineEdit_axis->setText(AxisToString(profile.GetAxis()));
    ui_->lineEdit_stripper->setText(QString::number(currents.stripper(), 'f', 1));
    ui_->lineEdit_cf9->setText(QString::number(currents.cf9(), 'f', 1));
    ui_->lineEdit_chambre1->setText(QString::number(currents.chambre1(), 'f', 1));
    ui_->lineEdit_chambre2->setText(QString::number(currents.chambre2(), 'f', 1));
    ui_->lineEdit_diode->setText(QString::number(currents.diode(), 'f', 1));

    ui_->lineEdit_hardware->setText(HardwareToString(profile.Hardware()));
    if (profile.Hardware() == QA_HARDWARE::SCANNER3D) {
        ui_->lineEdit_z->setText(QString::number(profile.AveragePosition(Axis::Z), 'f', 2));
    } else {
        ui_->lineEdit_z->setText("0.0");
    }
    ui_->lineEdit_date->setText(profile.GetTimestamp().toString("yyyy:MM:dd-hh:mm"));

    try {
        BeamProfileResults results = profile.GetResults();
        ui_->lineEdit_centre->setText(QString::number(results.centre(), 'f', 2));
        ui_->lineEdit_width50->setText(QString::number(results.width_50(), 'f', 2));
        ui_->lineEdit_width90->setText(QString::number(results.width_90(), 'f', 2));
        ui_->lineEdit_width95->setText(QString::number(results.width_95(), 'f', 2));
        ui_->lineEdit_penumbra->setText(QString::number(results.penumbra(), 'f', 2));
        ui_->lineEdit_flatness->setText(QString::number(results.flatness(), 'f', 2));
    }
    catch (std::exception& exc) {
        qWarning() << "ShowBeamProfileDialog Exception thrown: " << exc.what();
    }
}

ShowBeamProfileDialog::~ShowBeamProfileDialog() {}

void ShowBeamProfileDialog::ExportToCSV() {
    try {
        QString directory = QFileDialog::getSaveFileName(this, "Please choose a directory", "Export", "", 0, 0);
        profile_.Export(directory);
        QMessageBox::information(this, tr("QA"), "Data successfully exported");
    }
    catch (std::exception& exc) {
        qWarning() << "ShowBeamProfileDialog::ExportToCSV Exception thrown " << exc.what();
        QMessageBox::warning(this, tr("QA"), QString::fromStdString(exc.what()));
    }
}

void ShowBeamProfileDialog::Print() {
    QMessageBox::information(this, tr("QA"), "Not yet implemented");
}
