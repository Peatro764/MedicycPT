#include "CollimateurPage.h"

#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <unistd.h>

#include "ui_MTP.h"
#include "MTP.h"
#include "Calc.h"
#include "MillingMachineProgramDialog.h"
#include "MillingMachine.h"

CollimateurPage::CollimateurPage(MTP* parent)
    : parent_(parent),
      settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MTP.ini"), QStandardPaths::LocateFile),
                QSettings::IniFormat)
{
    SetupLineEditValidators();
    SetupDefaults();
    ConnectSignals();
}

CollimateurPage::~CollimateurPage()
{
}

void CollimateurPage::SetupLineEditValidators() {
}

void CollimateurPage::ConnectSignals() {
    QObject::connect(parent_->ui()->coll_saveToDatabasePushButton, SIGNAL(clicked()), this, SLOT(SaveToDatabase()));
    QObject::connect(parent_->ui()->coll_fetchFromDatabasePushButton, SIGNAL(clicked()), this, SLOT(GetFromDatabase()));
    QObject::connect(parent_->ui()->coll_showFraiseuseProgramPushButton, SIGNAL(clicked()), this, SLOT(ShowFraiseuseProgram()));
    QObject::connect(parent_->ui()->coll_uploadEyePlanFilePushButton, SIGNAL(clicked()), this, SLOT(UploadEyePlanCoordinates()));
}

void CollimateurPage::SetupDefaults() {
    parent_->ui()->coll_uploadEyePlanFilePushButton->setFocus();
}

void CollimateurPage::SaveToDatabase() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        UpdateCollimator();
        MillingMachine milling_machine;
        if (!milling_machine.CreateProgram(collimator_)) {
            throw std::runtime_error("Impossible de créer le programme de fraisage du collimateur");
        }
        parent_->repo()->SaveCollimateurItems(collimator_, milling_machine.GetProgram());
        QMessageBox::information(parent_, "MTP", "Le collimateur a été enregistré dans la base de données");
    }
    catch (std::exception& exc) {
        qWarning() << QString("CollimateurPage::SaveToDatabase Exception thrown: ") + exc.what();
        QString message = QString("Echec de l'enregistrement du collimateur dans la base de données\n\n") + QString(exc.what());
        QMessageBox::warning(parent_, "MTP", message);
    }
}

void CollimateurPage::GetFromDatabase() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        bool user_input_ok(false);
        const int dossier = QInputDialog::getInt(parent_, tr("Veuillez donner le numéro de dossier"), "Dossier: ",
                                                 0, 0, 1000000, 1, &user_input_ok);
        if (!user_input_ok) {
            qWarning() << "CollimateurPage::GetFromDatabase QInputDialog returned an error";
            return;
        }
        collimator_ = parent_->repo()->GetCollimator(dossier);
        WriteCollimatorData();
        DrawCollimator(collimator_);
    }
    catch (std::exception& exc) {
      qWarning() << QString("CollimateurPage::GetFromDatabase Exception thrown: ") + exc.what();
      QMessageBox::warning(parent_, "MTP", QString(exc.what()));
    }
}

void CollimateurPage::WritePatientDatabaseName() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        const int dossier = collimator_.Dossier();
        const Patient patient = parent_->repo()->GetPatient(dossier);
        parent_->ui()->label_db_patient_firstname->setText(patient.GetFirstName());
        parent_->ui()->label_db_patient_lastname->setText(patient.GetLastName());
    }
    catch (std::exception& exc) {
      qWarning() << QString("CollimateurPage::WritePatientDatabaseName Exception thrown: ") + exc.what();
      QMessageBox::warning(parent_, "MTP", QString(exc.what()));
    }
}

void CollimateurPage::ClearPatientDatabaseName() {
    parent_->ui()->label_db_patient_firstname->clear();
    parent_->ui()->label_db_patient_lastname->clear();
}

void CollimateurPage::UploadEyePlanCoordinates() {
    ClearPatientDatabaseName();
    QString filename("");
    QString directory = settings_.value("collimateur/directory", "unknown").toString();
//    if (!QueryUserForFile("D:\\Mes Documents\\eyex\\Colli", &filename)) {
    if (!QueryUserForFile(directory, &filename)) {
        QMessageBox::warning(parent_, "MTP", "Un fichier valide n'a pas été sélectionné");
        return;
    }

    QFile eyePlanFile(filename);
    if (!eyePlanFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(parent_, "MTP", "Échec de l'ouverture du fichier sélectionné");
        return;
    }

    QTextStream stream(&eyePlanFile);
    collimator_ = Collimator(stream);
    if (!collimator_.IsValid()) {
        QMessageBox::warning(parent_, "MTP", "Le format de fichier n'est pas correct");
    }

    WriteCollimatorData();
    if (collimator_.IsValid()) {
        DrawCollimator(collimator_);
    }
    WritePatientDatabaseName();
}

void CollimateurPage::UpdateCollimator() {
    collimator_.SetDossier(parent_->ui()->coll_dossierLabel->text().toInt());
    collimator_.SetLastName(parent_->ui()->coll_patientLastNameLabel->text().simplified());
    collimator_.SetFirstName(parent_->ui()->coll_patientFirstNameLabel->text().simplified());
}

void CollimateurPage::DrawCollimator(const Collimator &collimator) {
    parent_->ui()->coll_collimatorWidget->PaintCollimator(collimator);
}

void CollimateurPage::WriteCollimatorData() {
    parent_->ui()->coll_dossierLabel->setText(QString::number(collimator_.Dossier()));
    parent_->ui()->coll_patientLastNameLabel->setText(collimator_.patient().GetLastName().toUpper());
    parent_->ui()->coll_patientFirstNameLabel->setText(collimator_.patient().GetFirstName());
}

bool CollimateurPage::QueryUserForFile(QString default_dir, QString *filename) {
    QFileDialog dialog(parent_);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(default_dir);
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
    }

    if (fileNames.size() == 1) {
        *filename = fileNames.at(0);
        return true;
    } else {
        *filename = "";
        return false;
    }
}

void CollimateurPage::ShowFraiseuseProgram() {
    try {
       MillingMachine milling_machine;
       if (!milling_machine.CreateProgram(collimator_)) {
           throw std::runtime_error("Impossible de créer le programme d'usinage du collimateur");
       }
       MillingMachineProgramDialog dialog(parent_, milling_machine.GetProgram());
       dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << QString("CollimateurPage::ShowFraiseuseProgram Exception thrown: ") + exc.what();
        QMessageBox::warning(parent_, "MTP", QString(exc.what()));
    }
}
