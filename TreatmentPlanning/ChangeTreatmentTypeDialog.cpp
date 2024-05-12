#include "ChangeTreatmentTypeDialog.h"
#include "ui_ChangeTreatmentTypeDialog.h"

#include "Calc.h"

ChangeTreatmentTypeDialog::ChangeTreatmentTypeDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ChangeTreatmentTypeDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle("Changer le type de traitement");

    FillTreatmentTypes();

    QObject::connect(ui_->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->pushButton_update, SIGNAL(clicked()), this, SLOT(ChangeTreatmentType()));
}

void ChangeTreatmentTypeDialog::ChangeTreatmentType() {
    const QString new_treatment_type = ui_->comboBox_new_type->currentText();
    if (new_treatment_type.isEmpty() || new_treatment_type.isNull()) {
        QMessageBox::warning(this, "MTP", "Pas un type de traitement valide");
        return;
    }

    try {
        repo_->UpdateTreatmentType(dossier_, new_treatment_type);
        accept();
    }
    catch (std::exception& exc) {
        qWarning() << "ModifyDeliveredDoseDialog::UpdateDose Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

void ChangeTreatmentTypeDialog::FillTreatmentTypes() {
    try {
        ui_->comboBox_new_type->clear();
        const auto dossier_treatment_type = repo_->GetTreatment(dossier_).TreatmentType();
        const auto available_treatment_types = repo_->GetTreatmentTypes();

        for (auto type : available_treatment_types) {
            ui_->comboBox_new_type->addItem(type.Name());
        }
        if (ui_->comboBox_new_type->findText(dossier_treatment_type)) {
            ui_->comboBox_new_type->setCurrentText(dossier_treatment_type);
        } else {
            qWarning() << "ChangeTreatmentTypeDialog::FillTreatmentTypes the treatment type set in the dossier does not exist in the list of available types";
        }
    }
    catch (std::exception& exc) {
        qWarning() << "ChangeTreatmentTypeDialog::FillTreatmentTypes Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}



