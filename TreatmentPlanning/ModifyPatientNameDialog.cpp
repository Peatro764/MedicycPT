#include "ModifyPatientNameDialog.h"
#include "ui_ModifyPatientNameDialog.h"

ModifyPatientNameDialog::ModifyPatientNameDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ModifyPatientNameDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->updatePushButton, SIGNAL(clicked()), this, SLOT(ModifyName()));
    setWindowTitle("Modifier le nom du patient");

    try {
        Patient patient(repo_->GetPatient(dossier_));
        ui_->firstNameLineEdit->setText(patient.GetFirstName());
        ui_->lastNameLineEdit->setText(patient.GetLastName());
    }
    catch (...) {
        QMessageBox::warning(this, "MTP", QString("An exception occurred"));
    }
}

void ModifyPatientNameDialog::ModifyName() {
    try {
        Patient patient(repo_->GetPatient(dossier_));
        const QString first_name(ui_->firstNameLineEdit->text());
        const QString last_name(ui_->lastNameLineEdit->text());
        repo_->UpdatePatient(Patient(patient.GetId(), first_name, last_name));
        accept();
    }
    catch (std::exception& exc) {
        qWarning() << QString("ModifyPatientNameDialog::ModifyName Exception thrown: ") + exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}



