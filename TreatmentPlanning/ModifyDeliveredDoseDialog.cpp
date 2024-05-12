#include "ModifyDeliveredDoseDialog.h"
#include "ui_ModifyDeliveredDoseDialog.h"

#include "Calc.h"

ModifyDeliveredDoseDialog::ModifyDeliveredDoseDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ModifyDeliveredDoseDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle("Modifier la dose délivrée");

    ui_->newDoseLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), ui_->newDoseLineEdit));
    ui_->doseUnitComboBox->addItem("UM");
    ui_->doseUnitComboBox->addItem("GY");
    FillDeliveredDoseComboBox();

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->updatePushButton, SIGNAL(clicked()), this, SLOT(ModifyDose()));
    QObject::connect(ui_->doseUnitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(FillDeliveredDoseComboBox()));

}

void ModifyDeliveredDoseDialog::ModifyDose() {
    if (ui_->newDoseLineEdit->text().isEmpty()) {
        QMessageBox::information(this, "MTP", "Veuillez donner une dose valide");
        return;
    }

    QString dose_unit(ui_->doseUnitComboBox->currentText());
    if (dose_unit != QString("UM") && dose_unit != QString("GY")) {
        qWarning() << "ModifyDeliveredDoseDialog::ModifyDose Non valid dose unit: " + dose_unit;
        return;
    }

    const QDateTime seance_record_timestamp(ui_->seanceComboBox->currentData().toDateTime());
    qDebug() << "ModifyDeliveredDoseDialog::ModifyDose Seance record timestamp " << seance_record_timestamp;

    try {
        auto record = repo_->GetSeanceRecord(seance_record_timestamp);
        int new_dose(0);
        if (dose_unit == QString("UM")) {
            new_dose = ui_->newDoseLineEdit->text().toInt();
        } else {
            new_dose = std::rint(calc::DoseToUM(record.GetDebit(), ui_->newDoseLineEdit->text().toDouble()));
        }

        QMessageBox msg_box;
        msg_box.setText("Mise à jour de la séance effectuée le " + record.GetTimestamp().toString() + " avec la nouvelle dose " +
                        QString::number(new_dose) + " UM (" + QString::number(calc::UMToDose(record.GetDebit(), new_dose), 'f', 3) + " Gy)?");
        msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg_box.setDefaultButton(QMessageBox::No);
        int ret = msg_box.exec();
        switch (ret) {
        case QMessageBox::Yes:
            repo_->UpdateDeliveredDose(seance_record_timestamp, new_dose);
            ui_->newDoseLineEdit->clear();
            accept();
            break;
        case QMessageBox::No:
            break;
        default:
            qWarning() << "ModifyDeliveredDoseDialog::ModifyDose Not covered case";
            break;
        }
    }
    catch (std::exception& exc) {
        qWarning() << "ModifyDeliveredDoseDialog::UpdateDose Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }

}

void ModifyDeliveredDoseDialog::FillDeliveredDoseComboBox() {
    try {
        ui_->seanceComboBox->clear();
        auto treatment = repo_->GetTreatment(dossier_);
        auto past_seances = treatment.GetAllSeances();

        QString dose_unit(ui_->doseUnitComboBox->currentText());
        if (dose_unit != QString("UM") && dose_unit != QString("GY")) {
            qWarning() << "ModifyDeliveredDoseDialog::FillDeliveredDoseComboBox Non valid dose unit: " + dose_unit;
            return;
        }

        if (dose_unit == QString("UM")) {
            ui_->newDoseLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{0,}"), ui_->newDoseLineEdit));
        } else {
            ui_->newDoseLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), ui_->newDoseLineEdit));
        }

        for (int s_idx = 0; s_idx < (int)past_seances.size(); ++s_idx) {
            auto seance_records = past_seances.at(s_idx).GetSeanceRecords();
//            for (int r_idx = 0; r_idx < (int)seance_records.size(); ++r_idx) {
            int r_idx = 0;
            for (auto rec : seance_records) {
//                auto rec = seance_records.at(r_idx);
                QString dose = (dose_unit == QString("GY")) ? QString::number(rec.GetDoseDelivered(), 'f', 3) : QString::number(rec.GetTotalUMDelivered());
                QString id(QString::number(s_idx + 1) + "." + QString::number(r_idx + 1));
                ui_->seanceComboBox->addItem(id + ": " + dose + " " + dose_unit, rec.GetTimestamp());
                ++r_idx;
            }
        }
    }
    catch (std::exception& exc) {
        qWarning() << "ModifyDeliveredDoseDialog::FillDeliveredDoseComboBox Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}



