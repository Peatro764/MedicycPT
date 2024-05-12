#include "ModifyPlannedDoseDialog.h"
#include "ui_ModifyPlannedDoseDialog.h"

#include "Calc.h"

ModifyPlannedDoseDialog::ModifyPlannedDoseDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ModifyPlannedDoseDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle("Modifier la dose prévue");

    ui_->newDoseLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), ui_->newDoseLineEdit));
    FillPlannedDoseComboBox();

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->updatePushButton, SIGNAL(clicked()), this, SLOT(ModifyDose()));
}

void ModifyPlannedDoseDialog::ModifyDose() {
    if (ui_->newDoseLineEdit->text().isEmpty() || ui_->newDoseLineEdit->text().isNull()) {
        QMessageBox::information(this, "MTP", "Veuillez donner une dose valide");
        return;
    }

    try {
        const double new_dose = ui_->newDoseLineEdit->text().toDouble();
        const int seance_db_id = ui_->seanceComboBox->currentData().toInt();
        const int seance_number = ui_->seanceComboBox->currentIndex() + 1;

        QMessageBox msg_box;
        msg_box.setText("Mettre à jour la dose prévue de séance " + QString::number(seance_number) + " avec " +
                        QString::number(new_dose, 'f', 3) + " Gy?");
        msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg_box.setDefaultButton(QMessageBox::No);
        int ret = msg_box.exec();
        switch (ret) {
        case QMessageBox::Yes:
            repo_->UpdatePlannedDose(seance_db_id, dossier_, new_dose);
            ui_->newDoseLineEdit->clear();
            FillPlannedDoseComboBox();
//            accept();
            break;
        case QMessageBox::No:
            break;
        default:
            qWarning() << "ModifyPlannedDoseDialog::ModifyDose Not covered case";
            QMessageBox::warning(this, "MTP", "La dose n'a pas pu être mise à jour");
            break;
        }
    }
    catch (std::exception& exc) {
        qWarning() << "ModifyPlannedDoseDialog::ModifyDose Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }

}

void ModifyPlannedDoseDialog::FillPlannedDoseComboBox() {
    try {
        ui_->seanceComboBox->clear();
        std::vector<Seance> seances = repo_->GetSeances(dossier_);

        for (int s_idx = 0; s_idx < (int)seances.size(); ++s_idx) {
            QString id(QString::number(s_idx + 1));
            ui_->seanceComboBox->addItem(id + ": " + QString::number(seances.at(s_idx).GetDosePrescribed(), 'f', 3), seances.at(s_idx).GetSeanceId());
        }
    }
    catch (std::exception& exc) {
        qWarning() << "ModifyPlannedDoseDialog::FillPlannnedDoseComboBox Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}



