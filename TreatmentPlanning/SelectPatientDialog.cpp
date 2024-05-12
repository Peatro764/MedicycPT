#include "SelectPatientDialog.h"
#include "ui_SelectPatientDialog.h"

SelectPatientDialog::SelectPatientDialog(QWidget *parent, PTRepo* repo) :
    QDialog(parent),
    ui_(new Ui::SelectPatientDialog),
    repo_(repo)
{
    ui_->setupUi(this);

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->okPushButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui_->applyFilterPushButton, SIGNAL(clicked()), this, SLOT(UpdatePatientList()));

    setWindowTitle("Select patient");

    UpdatePatientList();
}

// can throw
Patient SelectPatientDialog::GetPatient() const {
    const int patient_id(ui_->patientComboBox->itemData(ui_->patientComboBox->currentIndex()).toInt());
    const auto patient = repo_->GetPatientById(patient_id);
    return patient;
}

void SelectPatientDialog::UpdatePatientList() {
    try {
        auto patients = repo_->GetPatients(ui_->firstNameLineEdit->text(),
                                           ui_->lastNameLineEdit->text());
        ui_->patientComboBox->clear();
        for (auto patient : patients) {
            if (patient.GetId() != 0) { // dont include tdp patient
                ui_->patientComboBox->addItem(QString::number(patient.GetId()) + " " +
                                              patient.GetFirstName() + " " +
                                              patient.GetLastName(),
                                              patient.GetId());
            }
        }
    }
    catch (std::exception& exc) {
        qWarning() << QString("SelectPatientDialog::UpdatePatientList Exception thrown: ") + exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
        reject();
    }
}




