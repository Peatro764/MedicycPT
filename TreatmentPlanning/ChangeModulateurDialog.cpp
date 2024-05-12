#include "ChangeModulateurDialog.h"
#include "ui_ChangeModulateurDialog.h"
#include "Modulateur.h"

ChangeModulateurDialog::ChangeModulateurDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ChangeModulateurDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);

    FillModulateurs();
    SetActiveModulateur();

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->updatePushButton, SIGNAL(clicked()), this, SLOT(ChangeModulateur()));
    setWindowTitle("Change modulateur");
}

void ChangeModulateurDialog::FillModulateurs() {
    try {
        auto modulateurs(repo_->GetAvailableModulateurs());
        ui_->modulateurComboBox->addItems(modulateurs);
        ui_->modulateurComboBox->setCurrentIndex(0);
    }
    catch (std::exception& exc) {
        qWarning() << QString("ChangeModulateurDialog::FillModulateurs Exception when retreiving modulateurs from db: ") + exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

void ChangeModulateurDialog::SetActiveModulateur() {
    try {
        auto modulateur(repo_->GetModulateurForDossier(dossier_));
        ui_->modulateurComboBox->setCurrentText(modulateur.id());
    }
    catch (std::exception& exc) {
        qWarning() << "ChangeModulateurDialog::SetActiveModulateur Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

void ChangeModulateurDialog::ChangeModulateur() {
    try {
        const int modulateur_id(ui_->modulateurComboBox->currentText().toInt());
        repo_->ChangeModulateur(dossier_, modulateur_id);
        accept();
    }
    catch (std::exception& exc) {
        qWarning() << "ChangeModulateurDialog::ChangeModulateur Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}



