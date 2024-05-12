#include "CreateDossierDialog.h"
#include "ui_CreateDossierDialog.h"
#include "SelectPatientDialog.h"

#include "Calc.h"

CreateDossierDialog::CreateDossierDialog(QWidget *parent, PTRepo* repo) :
    QDialog(parent),
    ui_(new Ui::CreateDossierDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setWindowTitle("Créer un dossier");

    SetupLineEditValidators();
    ConnectSignals();
    InitTreatmentTypesTable();
    FetchTreatmentTypesData();
}

CreateDossierDialog::~CreateDossierDialog() {
}

void CreateDossierDialog::SetupLineEditValidators() {
    ui_->dossierLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{1,6}"), ui_->dossierLineEdit));
    ui_->lineEdit_seance_dose->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), ui_->lineEdit_seance_dose));
}

void CreateDossierDialog::ConnectSignals() {
    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->createDossierPushButton, SIGNAL(clicked()), this, SLOT(CreateDossier()));

    //QObject::connect(ui_->selectExistingPatientPushButton, SIGNAL(clicked()), this, SLOT(SelectExistingPatient()));

    QObject::connect(ui_->pushButton_addSeance, &QPushButton::clicked, this, &CreateDossierDialog::AddSeance);
    QObject::connect(ui_->pushButton_addBoost, &QPushButton::clicked, this, &CreateDossierDialog::AddBoost);
    QObject::connect(ui_->pushButton_resetTable, &QPushButton::clicked, this, &CreateDossierDialog::FetchTreatmentTypesData);
}

//void CreateDossierDialog::SelectExistingPatient() {
//    try {
//        SelectPatientDialog dialog(this, repo_);
//        if (dialog.exec() == QDialog::Accepted) {
//            Patient patient = dialog.GetPatient();
//            ui_->firstNameLineEdit->setText(patient.GetFirstName());
//            ui_->lastNameLineEdit->setText(patient.GetLastName().toUpper());
//            ui_->patientIdLineEdit->setText(QString::number(patient.GetId()));
//            ui_->firstNameLineEdit->setReadOnly(true);
//            ui_->lastNameLineEdit->setReadOnly(true);
//        }
//    }
//    catch (std::exception& exc) {
//        QMessageBox::warning(this, "MTP", "There was a problem fetching patients");
//    }
//}

int CreateDossierDialog::CreateNewPatient(QString first_name, QString last_name) {
    if (first_name.isEmpty() || last_name.isEmpty()) {
         QMessageBox::warning(this, "MTP", "Prénom ou nom non valide");
         throw std::runtime_error("Patient name empty");
    }

    Patient patient(first_name, last_name);
    const int patient_id = repo_->SavePatient(patient);
    qDebug() << "CreateDossierDialog::CreateNewPatient Patient inserted in db, id = " << patient_id;
    return patient_id;
}

void CreateDossierDialog::CreateDossier() {
    bool dossier_number_ok(false);
    const int dossier_number(ui_->dossierLineEdit->text().toInt(&dossier_number_ok));
    if (!dossier_number_ok) {
        QMessageBox::warning(this, "MTP", "Le numéro de dossier donné est incorrect");
        return;
    }

    if (ui_->firstNameLineEdit->text().isEmpty() || ui_->lastNameLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "MTP", "Le nom du patient n'est pas valide");
        return;
    }

    QString treatment_type;
    try {
        treatment_type = GetSelectedTreatmentType();
    }
    catch (...) {
        QMessageBox::warning(this, "MTP", QString("Le type de traitement n'a pas pu etre vérifié"));
        return;
    }

    QStringList prescribed_doses_stringlist;
    std::vector<double> prescribed_doses = types_.at(treatment_type).SeanceDoses();
    for (double dose : prescribed_doses) {
      prescribed_doses_stringlist << QString::number(dose, 'f', 2);
    }

    if (prescribed_doses.empty()) {
       QMessageBox::warning(this, "MTP", "Un dossier doit avoir au moins une séance");
       qWarning() << "CreateDossierDialog::CreateDossier No seances, create dossier aborted";
       return;
    }

    try {
        const int modulateur_id(100);
        const DegradeurSet degradeur_set;

        QMessageBox msg_box;
        msg_box.setWindowTitle("Créer le dossier?");
        QString message("Dossier:            \t" + QString::number(dossier_number) + "\n"
                        "Patient:            \t" + ui_->lastNameLineEdit->text().toUpper() + " " + ui_->firstNameLineEdit->text() + "\n"
                        "Type de traitement: \t" + treatment_type + "\n"
                        "Séances:            \t" + QString::number(prescribed_doses.size()) + " (" + prescribed_doses_stringlist.join(",") + " (Gy))\n");
        msg_box.setText(message);
        msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg_box.setDefaultButton(QMessageBox::Yes);
        const int ret = msg_box.exec();
        int patient_id(-1);
        switch (ret) {
        case QMessageBox::Yes:
//            if (ui_->patientIdLineEdit->text().isEmpty()) {
//                patient_id = CreateNewPatient(ui_->firstNameLineEdit->text(),
//                                              ui_->lastNameLineEdit->text());
//            } else {
//                patient_id = ui_->patientIdLineEdit->text().toInt();
//            }
            patient_id = CreateNewPatient(ui_->firstNameLineEdit->text(),
                                          ui_->lastNameLineEdit->text());
            if (degradeur_set.single()) {
                repo_->CreateTreatment(dossier_number, patient_id, prescribed_doses, modulateur_id, degradeur_set.ids().at(0).toInt(), treatment_type);
            } else {
                repo_->CreateTreatment(dossier_number, patient_id, prescribed_doses, modulateur_id, degradeur_set.ids().at(0).toInt(), degradeur_set.ids().at(1).toInt(), treatment_type);
            }
            accept();
            break;
        case QMessageBox::No:
            break;
        default:
            qWarning() << "CreateDossierDialog::CreateDossier Not covered case";
            break;
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "MTP", QString("Le dossier n'a pas pu etre créee: ") + exc.what());
    }
}

void CreateDossierDialog::InitTreatmentTypesTable() {
    QStringList headerLabels_current;
    headerLabels_current << "Nom" << "Dose intégrée" << "Séances" << "Boost";
    ui_->tableWidget_treatment_types->setColumnWidth(0, 200);
    ui_->tableWidget_treatment_types->setColumnWidth(1, 200);
    ui_->tableWidget_treatment_types->setColumnWidth(2, 300);
    ui_->tableWidget_treatment_types->setColumnWidth(3, 100);
    ui_->tableWidget_treatment_types->setHorizontalHeaderLabels(headerLabels_current);
    ui_->tableWidget_treatment_types->scrollToTop();
    ui_->tableWidget_treatment_types->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableWidget_treatment_types->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableWidget_treatment_types->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void CreateDossierDialog::FetchTreatmentTypesData() {
    try {
        types_.clear();
        std::vector<TreatmentType> types_vector(repo_->GetTreatmentTypes());
        for (int row = 0; row < static_cast<int>(types_vector.size()); ++row) {
            types_.emplace(types_vector.at(row).Name(), types_vector.at(row));
        }
        ui_->tableWidget_treatment_types->setRowCount(static_cast<int>(types_vector.size()));
        FillTreatmentTypesTable(types_);
        ui_->tableWidget_treatment_types->setCurrentCell(0, 0);
    }
    catch (std::exception& exc) {
        qWarning() << QString("CreateDossierDialog::UpdateTreatmentTypesTableSelectors Excpetion caught: ") + exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

void CreateDossierDialog::FillTreatmentTypesTable(const std::map<QString, TreatmentType>& types) {
    try {
        ui_ ->tableWidget_treatment_types->clearContents();
        int row = 0;
        for (auto &t : types) {
            ui_ ->tableWidget_treatment_types->setItem(row, 0, new QTableWidgetItem(t.first));
            ui_ ->tableWidget_treatment_types->setItem(row, 1, new QTableWidgetItem(t.second.TotalSummary()));
            ui_ ->tableWidget_treatment_types->setItem(row, 2, new QTableWidgetItem(t.second.StandardSummary()));
            ui_ ->tableWidget_treatment_types->setItem(row, 3, new QTableWidgetItem(t.second.BoostSummary()));
            ++row;
        }
    }
    catch (std::exception& exc) {
        qWarning() << QString("CreateDossierDialog::UpdateTreatmentTypesTable Excpetion caught: ") + exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

QString CreateDossierDialog::GetSelectedTreatmentType() const {
    int row = ui_->tableWidget_treatment_types->currentRow();
    QTableWidgetItem *item = ui_->tableWidget_treatment_types->item(row, 0);
    if (!item) throw std::runtime_error("No treatment type on selected row");
    return item->data(Qt::DisplayRole).toString();
}

int CreateDossierDialog::GetTreatmentTypeRow(QString type) const {
    QStringList types;
    for (int row = 0; row < ui_->tableWidget_treatment_types->rowCount(); ++row) {
        QTableWidgetItem *item = ui_->tableWidget_treatment_types->item(row, 0);
        if (!item) throw std::runtime_error("No data in row");
        types.push_back(item->data(Qt::DisplayRole).toString());
    }
    int idx = types.indexOf(type);
    if (idx == -1) throw std::runtime_error("No such treatment type in table");
    return idx;
}

void CreateDossierDialog::AddSeance() {
    try {
       if (ui_->lineEdit_seance_dose->text().isEmpty()) throw std::runtime_error("S'il vous plaît donner une dose");
       types_.at(GetSelectedTreatmentType()).AddStandard(ui_->lineEdit_seance_dose->text().toDouble());
       QString selected = GetSelectedTreatmentType();
       FillTreatmentTypesTable(types_);
       ui_->tableWidget_treatment_types->setCurrentCell(GetTreatmentTypeRow(selected), 0);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "MTP", QString::fromStdString(exc.what()));
    }
}

void CreateDossierDialog::AddBoost() {
    try {
        if (ui_->lineEdit_seance_dose->text().isEmpty()) throw std::runtime_error("S'il vous plaît donner une dose");
        types_.at(GetSelectedTreatmentType()).AddBoost(ui_->lineEdit_seance_dose->text().toDouble());
        QString selected = GetSelectedTreatmentType();
        FillTreatmentTypesTable(types_);
        ui_->tableWidget_treatment_types->setCurrentCell(GetTreatmentTypeRow(selected), 0);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "MTP", QString::fromStdString(exc.what()));
    }
}

