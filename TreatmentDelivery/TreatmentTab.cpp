#include "TreatmentTab.h"

#include <QMessageBox>
#include <QDateTime>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>

#include "Calc.h"
#include "Util.h"
#include "Algorithms.h"
#include "MTD.h"
#include "BeamCalibrationDialog.h"
#include "BeamDeliveryDialog.h"
#include "CollimateurDialog.h"
#include "CompensateurTransparantDialog.h"

TreatmentTab::TreatmentTab(int dossierNumber, MTD* parent)
    : dossierNumber_(dossierNumber),
      parent_(parent),
      total_dose_graph_(parent->ui()->totalDosePieChartView),
      seance_dose_graph_(parent->ui()->seanceDoseChartView),
      seance_progress_graph_(parent->ui()->seanceProgressChartView, true, true)
{
    ConnectSignals();
    SetupLineEditValidators();
    UpdateDurationLabel();

    try {
        if (dossierNumber != 10000) {
            treatment_ = parent_->repo()->GetTreatment(dossierNumber_);
            WriteDefaults();
            WriteConfig();
            WriteResults();
            qDebug() << "TreatmentTab::TreatmentTab Dossier = " << QString::number(dossierNumber_);
        }
    }
    catch (std::exception& exc) {
        qWarning() << QString("TreatmentTab::TreatmentTab Exception thrown: ") + exc.what();

    }
}

TreatmentTab::~TreatmentTab() {}

void TreatmentTab::ConnectSignals() {
    QObject::connect(parent_->ui()->tr_umPrevuLineEdit, SIGNAL(editingFinished()), this, SLOT(UMChanged()));
    QObject::connect(parent_->ui()->startTreatmentButton, SIGNAL(clicked()), this, SLOT(DoTreatment()));
    QObject::connect(parent_->ui()->beamCalibrationMoreButton, SIGNAL(clicked()), this, SLOT(LaunchBeamCalibrationDialog()));
    QObject::connect(parent_->ui()->tr_durationCalculatedCheckBox, SIGNAL(clicked()), this, SLOT(UpdateDurationLabel()));
    QObject::connect(&seance_dose_graph_, SIGNAL(ShowSeanceInfoReq(int)), this, SLOT(ShowSeanceInfo(int)));
    QObject::connect(parent_->ui()->collimateurPushButton, SIGNAL(clicked()), this, SLOT(ShowCollimateur()));
    QObject::connect(parent_->ui()->compensateurPushButton, SIGNAL(clicked()), this, SLOT(ShowCompensateur()));
}

void TreatmentTab::SetupLineEditValidators() {
    parent_->ui()->tr_umPrevuLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{1,}"), parent_->ui()->tr_umPrevuLineEdit));
    parent_->ui()->tr_deltaUmPrevuLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{1,}"), parent_->ui()->tr_deltaUmPrevuLineEdit));
    parent_->ui()->tr_dureLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->tr_dureLineEdit));
}

void TreatmentTab::DoTreatment() {

    if (!parent_->io()->IsConnected()) {
        QMessageBox::critical(parent_, "Treatment cancelled", "No connection to hardware. Try reconnecting by clicking on IO button in services box");
        return;
    }

    if (parent_->ui()->lineEdit_debit_actuelle->text() == "NA" || parent_->ui()->lineEdit_debit_actuelle->text().isEmpty()) {
        QMessageBox::warning(parent_, "Treatment cancelled", "The beam calibration has not been done");
        return;
    }

    if (parent_->ui()->tr_umPrevuLineEdit->text().isEmpty()) {
        QMessageBox::warning(parent_, "Treatment cancelled", "UM prevu not given");
        return;
    }

    if (parent_->ui()->tr_dureLineEdit->text().isEmpty()) {
        QMessageBox::warning(parent_, "Treatment cancelled", "Duration not given");
        return;
    }

    if (!treatment_.GetActiveSeance()) {
        QMessageBox::warning(parent_, "Treatment cancelled", "There are no more seances scheduled for this dossier");
        return;
    }

    const int um_prevu = parent_->ui()->tr_umPrevuLineEdit->text().toInt();
    int delta_um(0);
    if (!GetDeltaUM(&delta_um)) {
        QMessageBox::warning(parent_, "Treatment cancelled", "Failed calculating dUM (stripper current error)");
        return;
    }
    parent_->ui()->tr_deltaUmPrevuLineEdit->setText(QString::number(delta_um));

    double duration(0.0);
    double duration_factor(1.0);
    if (parent_->ui()->tr_durationCalculatedCheckBox->isChecked()) {
        duration_factor = parent_->ui()->tr_dureLineEdit->text().toDouble();
    } else {
        duration = parent_->ui()->tr_dureLineEdit->text().toDouble();
    }

    if (parent_->ui()->tr_baliseCheckBox->isChecked()) NotifyDbTreatmentStarted();

    BeamDeliveryDialog beamDeliveryDialog(parent_->io(), &seance_progress_graph_);
    beamDeliveryDialog.setParent(parent_, Qt::Dialog);
    if (!beamDeliveryDialog.Start(um_prevu, delta_um,
                                  duration, duration_factor, parent_->ui()->tr_durationCalculatedCheckBox->isChecked())) {
        qWarning() << "TreatmentTab::DoTreatment Beam start was aborted";
        if (parent_->ui()->tr_baliseCheckBox->isChecked()) NotifyDbTreatmentCancelled();
        return;
    }

    if (beamDeliveryDialog.exec() != QDialog::Accepted) {
        qWarning() << "TreatmentTab::DoTreatment Beam delivery was aborted";
        if (parent_->ui()->tr_baliseCheckBox->isChecked()) NotifyDbTreatmentCancelled();
        return;
    }

    SeanceRecord record(QDateTime::currentDateTime(),
                        treatment_.GetBeamCalibration(),
                        beamDeliveryDialog.GetDuration(),
                        beamDeliveryDialog.GetIChambre1(),
                        beamDeliveryDialog.GetIChambre2(),
                        beamDeliveryDialog.GetIStripper(),
                        beamDeliveryDialog.GetICF9(),
                        um_prevu,
                        beamDeliveryDialog.GetDeliveredUM());

    treatment_.GetActiveSeance()->AddSeanceRecord(record);
    try {
        parent_->repo()->SaveSeanceRecord(treatment_.GetActiveSeance()->GetSeanceId(), record);
        qDebug() << "TreatmentTab::DoTreatment The seance record was saved to db";
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "Db error", QString("The seance record could not be saved to database. This treatment will thus not be considered when calculating the dose for the next seance. Please contact Petter Hofverberg before continuing this treatment."));
        qWarning() << QString("TreatmentTab::DoTreatment There was an error saving the seance record to db: ") + exc.what();
    }

    WriteResults();
    if (record.GetTotalUMDelivered() > (um_prevu - 5)) {
        IncrementActiveSeance();
    } else {
        if (QueryUserIfSeanceFinished(um_prevu, record.GetTotalUMDelivered())) {
            IncrementActiveSeance();
        }
    }
    WriteCurrentSeance();
}

bool TreatmentTab::QueryUserIfSeanceFinished(int um_pre, int um_del) {
    bool seance_finished(false);
    QMessageBox msgBox;
    QString message = "Only a partial dose was delivered (" + QString::number(um_del) + " of " + QString::number(um_pre) + " UM)";
    msgBox.setText(message);
    msgBox.setInformativeText("Do you want to continue the current seance?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        seance_finished = false;
        break;
    case QMessageBox::No:
        seance_finished = true;
        break;
    default:
        qWarning() << "TreatmentTab::DoTreatment Not covered case";
        break;
    }
    return seance_finished;
}

void TreatmentTab::IncrementActiveSeance() {
    try {
        treatment_.IncrementSeance();
        parent_->repo()->UpdateTreatment(treatment_);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "Db error", "The seance counter could not be incremented in the database");
        qWarning() << QString("TreatmentTab::DoTreatment The seance counter could not be incremented in the database : ") + exc.what();
    }
}

void TreatmentTab::UMChanged() {
    if (!treatment_.BeamCalibrationDone()) {
       qWarning() << "TreatmentTab::UMChanged ignoring input since beam calibration hasnt been done";
       QMessageBox::warning(parent_, "Input ignored", "UM prevu can only be changed when a complete beam calibration has been done");
       parent_->ui()->tr_umPrevuLineEdit->clear();
       WriteCurrentSeance();
       return;
    }

    // validator on line edit -> can be sure the conversion to int works
    const int um = parent_->ui()->tr_umPrevuLineEdit->text().toInt();
    parent_->ui()->tr_dosePrevuLineEdit->setText(QString::number(calc::UMToDose(treatment_.GetBeamCalibration(), um), 'f', 3));
    const double current_stripper = GetEstimatedStripperCurrent();
    parent_->ui()->tr_stripperCurrentLineEdit->setText(QString::number(std::round(current_stripper)));
    const int um_correction = util::GetUMCorrection(treatment_.GetBeamCalibration(), current_stripper);
    parent_->ui()->tr_deltaUmPrevuLineEdit->setText(QString::number(um_correction));
}

void TreatmentTab::WriteConfig() {
    WriteBeamCalibration();
    WriteBeamShaping();
    WriteCurrentSeance();
}

void TreatmentTab::WriteCurrentSeance() {
    auto activeSeanceNumber = treatment_.GetActiveSeanceNumber();
    if (activeSeanceNumber != -1) {
        parent_->ui()->tr_numeroDeSeanceLineEdit->setText(QString::number(activeSeanceNumber + 1));
        parent_->ui()->tr_dosePrevuLineEdit->setText(QString::number(treatment_.GetDosePrevuActiveSeance(), 'f', 3));
        if (treatment_.BeamCalibrationDone()) {
            parent_->ui()->tr_umPrevuLineEdit->setText(QString::number(treatment_.GetUMPrevuActiveSeance()));
            const double current_stripper = GetEstimatedStripperCurrent();
            parent_->ui()->tr_stripperCurrentLineEdit->setText(QString::number(std::round(current_stripper)));
            const int um_correction = util::GetUMCorrection(treatment_.GetBeamCalibration(), current_stripper);
            parent_->ui()->tr_deltaUmPrevuLineEdit->setText(QString::number(um_correction));
            qDebug() << "TreatmentTab::WriteCurrentSeance dUM = " << QString::number(um_correction)
                     << " i_stripper = " << QString::number(current_stripper, 'f', 3);
        } else {
            parent_->ui()->tr_umPrevuLineEdit->setText("0");
            parent_->ui()->tr_deltaUmPrevuLineEdit->setText("0");
            parent_->ui()->tr_stripperCurrentLineEdit->setText("0");
        }
    } else {
        parent_->ui()->tr_numeroDeSeanceLineEdit->setText("NA");
        parent_->ui()->tr_dosePrevuLineEdit->setText("NA");
        parent_->ui()->tr_umPrevuLineEdit->setText("0");
        parent_->ui()->tr_deltaUmPrevuLineEdit->setText("0");
        parent_->ui()->tr_stripperCurrentLineEdit->setText("0");
    }
}

double TreatmentTab::GetEstimatedStripperCurrent() {
     if (!treatment_.BeamCalibrationDone()) {
         qWarning() << "TreatmentTab::GetEstimatedStripperCurrent Cannot estimate stripper current when beam calibration hasnt been done";
         return 0.0;
     }
     try {
         auto defaults = parent_->repo()->GetDefaults();
         bool ok(false);
         const double dose_gy(parent_->ui()->tr_dosePrevuLineEdit->text().toDouble(&ok));
         if (!ok) {
             qWarning() << "TreatmentTab::GetEstimatedStripperCurrent Could not convert this value to dose in gy: " << parent_->ui()->tr_dosePrevuLineEdit->text();
             return 0.0;
         }
         const double current_stripper(util::GetStripperCurrent(treatment_.GetBeamCalibration(), dose_gy,
                                                                defaults.GetStripperResponseCorrection(),
                                                                defaults.GetSeanceDuration()));
         return current_stripper;
     }
     catch (std::exception& exc) {
         qWarning() << "TreatmentTab::GetEstimatedStripperCurrent Exception when retreiving default values: " << exc.what();
         return 0.0;
     }
}

void TreatmentTab::WriteBeamCalibration() {
    try {
        parent_->ui()->todaysTopDuJourCheckBox->setChecked(false);
        parent_->ui()->patientDosimetryCheckBox->setChecked(false);
        parent_->ui()->patientTopDuJourCheckBox->setChecked(false);
        parent_->ui()->lineEdit_debit_actuelle->setText("NA");

        Dosimetry patientDosi = treatment_.GetPatientDosimetry();
        Dosimetry patientTopDuJour = treatment_.GetPatientRefDosimetry();
        Dosimetry topDuJour = treatment_.GetRefDosimetryToday();

        parent_->ui()->todaysTopDuJourCheckBox->setChecked(topDuJour.IsValid());
        parent_->ui()->patientDosimetryCheckBox->setChecked(patientDosi.IsValid());
        bool patientRefDosimetryOk(patientTopDuJour.IsValid() || patientDosi.IsMonteCarlo());
        parent_->ui()->patientTopDuJourCheckBox->setChecked(patientRefDosimetryOk);

        if (treatment_.BeamCalibrationDone()) {
            QString debit(QString::number(treatment_.GetBeamCalibration(), 'f', 4));
            QString debitStdDev(QString::number(treatment_.GetBeamCalibrationStdDev(), 'f', 4));
            parent_->ui()->lineEdit_debit_actuelle->setText(debit + " \u00B1 " + debitStdDev);
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "Internal error", "There was an error calculating the beam calibration");
        qWarning() << QString("TreatmentTab::WriteConfig There was en exception when computing the beam calibration: ") + exc.what();
    }
}

void TreatmentTab::WriteBeamShaping() {
    parent_->ui()->degradeurLineEdit->setText(treatment_.GetDegradeurSet().mm_plexis().join(","));
    parent_->ui()->modulateurLineEdit->setText(treatment_.GetModulateur().id());
}

void TreatmentTab::WriteResults() {
    total_dose_graph_.SetData(treatment_.GetDeliveredDose(), treatment_.GetPrescribedDose());
    seance_dose_graph_.SetData(treatment_.GetAllSeances());
}

void TreatmentTab::LaunchBeamCalibrationDialog() {
    BeamCalibrationDialog beamCalibrationDialog(&treatment_);
    beamCalibrationDialog.exec();
}

void TreatmentTab::WriteDefaults() {
    try {
        auto defaults = parent_->repo()->GetDefaults();
        parent_->ui()->tr_dureLineEdit->setText(QString::number(defaults.GetDurationFactor(), 'f', 2));
    }
    catch (std::exception& exc) {
        parent_->ui()->tr_dureLineEdit->setText(QString::number(1.0f, 'f', 2));
        qWarning() << QString("TreatmentTab::WriteDefaults Exception when retreiving default values: ") + exc.what();
        QMessageBox::warning(parent_, "Database error", "Failed retreiving default values");
    }
}

void TreatmentTab::UpdateDurationLabel() {
    if (parent_->ui()->tr_durationCalculatedCheckBox->isChecked()) {
        parent_->ui()->treatmentDurationLabel->setText("Durée (%)");
    } else {
        parent_->ui()->treatmentDurationLabel->setText("Durée (s)");
    }
}

void TreatmentTab::ShowSeanceInfo(int seance_idx) {
    try {
       auto seance = treatment_.GetSeance(seance_idx);
       QString text("");
       text.append("Dose prevue:   " + QString::number(seance.GetDosePrescribed(), 'f', 3) + " Gy\n");
       text.append("Dose delivree: " + QString::number(seance.GetDoseDelivered(), 'f', 3) + " Gy\n");
       auto records = seance.GetSeanceRecords();
       if (records.empty()) {
           text.append("No delivered beams\n");
       } else {
           int rec_nmb(1);
           for (auto rec : records) {
               text.append("\nBeam: " + QString::number(rec_nmb++) + "\n");
               text.append("Timestamp:\t" + rec.GetTimestamp().toString() + "\n");
               text.append("Duree:\t" + QString::number(rec.GetTotalDuration(), 'f', 1) + " s\n");
               text.append("I.Stripper:\t" + QString::number(rec.GetIStripper(), 'f', 0) + " nA\n");
               text.append("I.CF9:\t" + QString::number(rec.GetICF9(), 'f', 0) + " nA\n");
               text.append("I.Chambre1:\t" + QString::number(calc::Mean(rec.GetIChambre1()), 'f', 0) + " nA\n");
               text.append("I.Chambre2:\t" + QString::number(calc::Mean(rec.GetIChambre2()), 'f', 0) + " nA\n");
               text.append("Debit:\t" + QString::number(rec.GetDebit(), 'f', 4) + " cGy/UM\n");
               text.append("UM Prevu:\t" + QString::number(rec.GetUMPrevu()) + "\n");
               text.append("UM Delivree:\t" + QString::number(rec.GetTotalUMDelivered()) + "\n");
               text.append("Gy Delivree:\t" + QString::number(rec.GetDoseDelivered(), 'f', 3) + "\n");
               text.append("Balise Dose:\t");
               try {
                    BaliseCalibration balise_calib = parent_->repo()->GetBaliseCalibration();
                    BaliseRecord balise_record = parent_->repo()->GetBaliseRecord(rec);
                    double balise_dose = algorithms::CalculateIntegratedDose(balise_calib.DoseRateToH_alpha(), balise_calib.DoseRateToH_beta(), balise_calib.svPerCoulomb(),
                                                                             balise_record.GetCharge(), balise_record.GetPatientDebit(), balise_record.GetRefDebit(),
                                                                             balise_record.GetTDJDebit());
                    text.append(QString::number(balise_dose, 'f', 2) + "\n");
               }
               catch(std::exception& exc) {
                   qDebug() << "TreatmentTab::ShowSeanceInfo Exception thrown: " << exc.what();
                   text.append("NA\n");
               }
           }
       }
       QMessageBox::information(parent_, QString("Seance ") + QString::number(seance_idx + 1), text);
    }
    catch (std::exception const& exc) {
        qWarning() << QString("TreatmentTab::ShowSeanceInfo Exception thrown: ") + exc.what();
        QMessageBox::warning(parent_, QString("Seance") + QString::number(seance_idx + 1), "Data not found");
    }
}

void TreatmentTab::ShowCollimateur() {
    try {
        auto collimator = parent_->repo()->GetCollimator(dossierNumber_);
        CollimateurDialog collimateurDialog(collimator);
        collimateurDialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "TreatmentTab::ShowCollimateur Catched an exception: " << exc.what();
        QMessageBox::warning(parent_, tr("MPD"), "No collimateur found");
    }
}

void TreatmentTab::ShowCompensateur() {
    try {
        auto compensateur = parent_->repo()->GetCompensateur(dossierNumber_);
        auto patient = parent_->repo()->GetPatient(dossierNumber_);
        CompensateurTransparantDialog dialog(parent_, compensateur, patient);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "TreatmentTab::ShowCompensateur Catched an exception: " << exc.what();
        QMessageBox::warning(parent_, tr("MPD"), "No compensateur found");
    }
}

bool TreatmentTab::GetDeltaUM(int *delta_um) {
    if (parent_->ui()->trUMCorrectionCheckBox->isChecked()) {
        double i_stripper(0.0);
        if (!parent_->io()->ReadCurrentStripper(&i_stripper)) {
            qWarning() << "TreatmentTab::GetDeltaUM Failed reading current stripper";
            *delta_um = 0;
            return false;
        }
        if (i_stripper < 5.0 || i_stripper > 450.0) {
            qWarning() << "TreatmentTab::GetDeltaUM Istripper out of bounds: " << QString::number(i_stripper, 'f', 3);
            *delta_um = 0;
            return false;
        }
        *delta_um = util::GetUMCorrection(treatment_.GetBeamCalibration(), i_stripper);
         qDebug() << "TreatmentTab::GetDeltaUM Stripper " << i_stripper << " UM " << *delta_um;
    } else {
        *delta_um = parent_->ui()->tr_deltaUmPrevuLineEdit->text().toInt();
    }
    return true;
}

void TreatmentTab::NotifyDbTreatmentStarted() {
    try {
        parent_->repo()->SetActiveTreatmentStarted(true);
    }
    catch (std::exception& exc) {
        qWarning() << "TreatmentTab::NotifyDbTreatmentStarted Exception thrown: " << exc.what();
        QMessageBox::warning(parent_, "MTD", "Failed notifying balise");
    }
}

void TreatmentTab::NotifyDbTreatmentCancelled() {
    try {
        parent_->repo()->SetActiveTreatmentStarted(false);
    }
    catch (std::exception& exc) {
        qWarning() << "TreatmentTab::NotifyDbTreatmentStarted Exception thrown: " << exc.what();
        QMessageBox::warning(parent_, "MTD", "Failed notifying balise");
    }
}
