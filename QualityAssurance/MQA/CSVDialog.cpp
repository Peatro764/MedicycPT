#include "CSVDialog.h"
#include "ui_CSVDialog.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QDateTime>

#include "Calc.h"
#include "Material.h"

CSVDialog::CSVDialog(QWidget *parent, std::shared_ptr<QARepo> repo) :
    QDialog(parent),
    ui_(new Ui::CSVDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setWindowTitle(QString("CSV Export"));
    SetupConnections();
    EnableButtons();
}

CSVDialog::~CSVDialog()
{
    delete ui_;
}

void CSVDialog::SetupConnections() {
    QObject::connect(ui_->pushButton_export, SIGNAL(clicked()), this, SLOT(ExportSelected()));
    QObject::connect(ui_->pushButton_quitte, SIGNAL(clicked()), this, SLOT(reject()));
}

void CSVDialog::EnableButtons() {
    ui_->pushButton_showDebitPatient->setEnabled(false);
    ui_->pushButton_showTopDeJour->setEnabled(false);
    ui_->pushButton_showPicDeBragg->setEnabled(false);
    ui_->pushButton_showProfile->setEnabled(false);
    ui_->pushButton_showScan3D->setEnabled(false);
    ui_->pushButton_showSOBP->setEnabled(false);
    try {
        repo_->GetLastBraggPeak();
        ui_->pushButton_showPicDeBragg->setEnabled(true);
    }
    catch(std::exception& exc) {
        (void)exc;
    }

    try {
        repo_->GetLastBeamProfile(Axis::X);
        repo_->GetLastBeamProfile(Axis::Y);
        ui_->pushButton_showProfile->setEnabled(true);
    }
    catch(std::exception& exc) {
        (void)exc;
    }

    try {
        repo_->GetLastSOBP();
        ui_->pushButton_showSOBP->setEnabled(true);
    }
    catch(std::exception& exc) {
        (void)exc;
    }
}

void CSVDialog::ExportSelected() {
    if (!ui_->pushButton_showDebitPatient->isChecked() &&
            !ui_->pushButton_showTopDeJour->isChecked() &&
            !ui_->pushButton_showPicDeBragg->isChecked() &&
            !ui_->pushButton_showProfile->isChecked() &&
            !ui_->pushButton_showScan3D->isChecked() &&
            !ui_->pushButton_showSOBP->isChecked()) {
        QMessageBox::warning(this, tr("QA"), "No objects selected");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Please give a file basename", "Export", "", 0, 0);

    try {
//        if (ui_->pushButton_showDebitPatient->isChecked()) {  dosimetry_dialog_->Print(&printer); }
//        if (ui_->pushButton_showTopDeJour->isChecked()) {  treatment_dialog_->Print(&printer); }
        if (ui_->pushButton_showPicDeBragg->isChecked()) { Export(filename, repo_->GetLastBraggPeak()); }
        if (ui_->pushButton_showProfile->isChecked()) { Export(filename, repo_->GetLastBeamProfile(Axis::X), Axis::X);
                                                     Export(filename, repo_->GetLastBeamProfile(Axis::Y), Axis::Y); }
//        if (ui_->radioButton_scan3D->isChecked()) { controle_dose_dialog_->Print(&printer); }
        if (ui_->pushButton_showSOBP->isChecked()) { Export(filename, repo_->GetLastSOBP()); }
        accept();
    }
    catch (std::exception const& exc) {
        qWarning() << "CSVDialog::ExportSelected catched an exception: " << exc.what();
        QMessageBox::warning(this, tr("CSV Export"), "Export failed");
    }
}

void CSVDialog::Export(QString basename, const BraggPeak& braggpeak) const {
    QFile data(basename + "-braggpeak.csv");
    if (!data.open(QFile::WriteOnly | QFile::Truncate)) {
        throw std::runtime_error("Could not open file");
    }
    QTextStream output(&data);
    std::vector<MeasurementPoint> mmtissue = braggpeak.depth_dose().GetPoints();
    std::vector<MeasurementPoint> mmplexi = braggpeak.depth_dose().ScaleAxis(Axis::Z,  material::ToPlexiglas(1.0, material::MATERIAL::TISSUE)).GetPoints();
    if (mmtissue.size() != mmplexi.size()) {
        throw std::runtime_error("Different size vectors");
    }

    output << "mm tissue" << ", mm plexi, " << "Intensity" << "\n";
    for (int idx = 0; idx < static_cast<int>(mmtissue.size()); ++idx) {
        output << QString::number(mmtissue.at(idx).pos(Axis::Z), 'f', 3) << " , "
               << QString::number(mmplexi.at(idx).pos(Axis::Z), 'f', 3) << " , "
               << QString::number(mmtissue.at(idx).intensity(braggpeak.depth_dose().GetSignalNoise()), 'f', 3) << "\n";
    }
}

void CSVDialog::Export(QString basename, const SOBP& sobp) const {
    QFile data(basename + "-sobp.csv");
    if (!data.open(QFile::WriteOnly | QFile::Truncate)) {
        throw std::runtime_error("Could not open file");
    }
    QTextStream output(&data);
    std::vector<MeasurementPoint> mmtissue = sobp.depth_dose().GetPoints();
    std::vector<MeasurementPoint> mmplexi = sobp.depth_dose().ScaleAxis(Axis::Z, material::ToPlexiglas(1.0, material::MATERIAL::TISSUE)).GetPoints();
    if (mmtissue.size() != mmplexi.size()) {
        throw std::runtime_error("Different size vectors");
    }

    output << "mm tissue" << ", mm plexi, " << "Intensity" << "\n";
    for (int idx = 0; idx < static_cast<int>(mmtissue.size()); ++idx) {
        output << QString::number(mmtissue.at(idx).pos(Axis::Z), 'f', 3) << " , "
               << QString::number(mmplexi.at(idx).pos(Axis::Z), 'f', 3) << " , "
               << QString::number(mmtissue.at(idx).intensity(sobp.depth_dose().GetSignalNoise()), 'f', 3) << "\n";
    }
}

void CSVDialog::Export(QString basename, const BeamProfile& beamprofile, Axis axis) const {
    QFile data(basename + "-profile" + "-" + AxisToString(axis) + "-" + ".csv");
    if (!data.open(QFile::WriteOnly | QFile::Truncate)) {
        throw std::runtime_error("Could not open file");
    }
    QTextStream output(&data);
    std::vector<MeasurementPoint> mmtissue = beamprofile.GetPoints();

    output << "mm," << " Intensity" << "\n";
    for (int idx = 0; idx < static_cast<int>(mmtissue.size()); ++idx) {
        output << QString::number(mmtissue.at(idx).pos(axis), 'f', 3) << " , "
               << QString::number(mmtissue.at(idx).intensity(beamprofile.GetSignalNoise()), 'f', 3) << "\n";
    }
}
