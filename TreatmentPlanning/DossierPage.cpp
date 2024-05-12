#include "DossierPage.h"

#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>
#include <QProgressDialog>

#include "ui_MTP.h"
#include "MTP.h"
#include "Treatment.h"
#include "Calc.h"
#include "Util.h"
#include "ModifyPatientNameDialog.h"
#include "ChangeModulateurDialog.h"
#include "ChangeDegradeurDialog.h"
#include "AddSeanceDialog.h"
#include "DeleteSeanceDialog.h"
#include "ModifyDeliveredDoseDialog.h"
#include "ModifyPlannedDoseDialog.h"
#include "CreateDossierDialog.h"
#include "ShowDeliveredDoseDialog.h"
#include "DeleteSeanceDialog.h"
#include "CollimateurDialog.h"
#include "CompensateurTransparantDialog.h"
#include "CommentsDialog.h"
#include "MillingMachineProgramDialog.h"
#include "ModulateurDialog.h"
#include "ChangeTreatmentTypeDialog.h"

DossierPage::DossierPage(MTP* parent)
    : parent_(parent)
{
    SetupLineEditValidators();
    ConnectSignals();
    ConfigureTable();
    FillComboboxes();
    SetDefaults();
}

DossierPage::~DossierPage()
{
}

void DossierPage::SetupLineEditValidators() {
}

void DossierPage::ConnectSignals() {
    QObject::connect(parent_->ui()->listDossier_queryPushButton, SIGNAL(clicked()), this, SLOT(GetDossiers()));
    QObject::connect(parent_->ui()->listDossier_dossierTableWidget, SIGNAL(cellClicked(int,int)),
                     this, SLOT(ShowContextMenu(int,int)));
    QObject::connect(parent_->ui()->listDossier_createNewDossierPushButton, SIGNAL(clicked()), this, SLOT(LaunchCreateNewDossierDialog()));
    QObject::connect(parent_->ui()->listDossier_dossierTableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(DossierRowClicked(int)));
}

void DossierPage::DossierRowClicked(int row) {
    try {
        if (!parent_->ui()->listDossier_dossierTableWidget->item(row, 1)) {
            qWarning() << "DossierPage::ShowContextMenu Not item in cell";
            return;
        }
        int dossier(GetDossierNumber(row));
        emit(PrintDossier(dossier));
    }
    catch (std::exception& exc) {
        QMessageBox::information(parent_, "MTP", "No dossier found");
        return;
    }
}

void DossierPage::FillComboboxes() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        parent_->ui()->dossierPage_comboBox_filter_treatmenttype->clear();
        std::vector<TreatmentType> types = parent_->repo()->GetTreatmentTypes();
        parent_->ui()->dossierPage_comboBox_filter_treatmenttype->addItem("Tout");
        for (auto type : types) {
            parent_->ui()->dossierPage_comboBox_filter_treatmenttype->addItem(type.Name());
        }
        parent_->ui()->dossierPage_comboBox_filter_treatmenttype->setCurrentText("Tout");
    } catch (std::exception& exc) {
        qWarning() << "DossierPage::FillComboboxes Exception " << exc.what();
    }
}

void DossierPage::ConfigureTable() {
    QStringList headerLabels;
    headerLabels << "Dossier" << "Patient" << "Type de\ntraitement" << "Modulateur" << "Degradeur(s)" << "Ep. totale\n [mm plexi]" << "Collimateur" << "Compensateur" << "D.Mesuré\n(cGy/UM)\nREF Cond" << "D.MCNPX\n(cGy/UM)\nREF Cond"
                 << "D.DEBROCA\n(cGy/UM)\nREF Cond" << "MCNPX\n/\nDEBROCA" << "Seances" << "Dose (Gy)" << "X-rays\nN / ESD [mGy]" << "Commentaires";
    parent_->ui()->listDossier_dossierTableWidget->setHorizontalHeaderLabels(headerLabels);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::DOSSIER, 60);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::PATIENT, 170);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::TREATMENT_TYPE, 145);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::MODULATEUR, 83);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::DEGRADEUR, 83);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::EPAISSEUR, 70);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::COLLIMATEUR, 77);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::COMPENSATEUR, 91);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::DMESURE, 75);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::DMCNPX, 75);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::DDEBROCA, 75);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::DRATIO, 65);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::SEANCES, 50);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::DOSE, 85);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::XRAYS, 85);
    parent_->ui()->listDossier_dossierTableWidget->setColumnWidth((int)COLUMNS::COMMENTS, 90);
}

void DossierPage::SetDefaults() {
    parent_->ui()->listDossier_fromTimeDateTimeEdit->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->listDossier_toTimeDateTimeEdit->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->listDossier_fromTimeDateTimeEdit->setCurrentSection(QDateTimeEdit::MonthSection);
    parent_->ui()->listDossier_toTimeDateTimeEdit->setCurrentSection(QDateTimeEdit::MonthSection);
    QDateTime current_date(QDateTime::currentDateTime());
    parent_->ui()->listDossier_fromTimeDateTimeEdit->setDate(current_date.date().addDays(-14));
    parent_->ui()->listDossier_toTimeDateTimeEdit->setDate(current_date.date());
}

void DossierPage::GetDossiers() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        parent_->ui()->listDossier_dossierTableWidget->clearContents();
        auto from_date(parent_->ui()->listDossier_fromTimeDateTimeEdit->date());
        auto to_date(parent_->ui()->listDossier_toTimeDateTimeEdit->date());
        auto dossier(parent_->ui()->dossierPage_dossierLineEdit->text());
        auto last_name(parent_->ui()->dossierPage_lastNameLineEdit->text());
        auto treatment_type(parent_->ui()->dossierPage_comboBox_filter_treatmenttype->currentText());
        std::vector<int> dossiers(parent_->repo()->GetDossiers(from_date, to_date, dossier, QString(""), last_name, treatment_type));
        QProgressDialog progress("Fetching dossiers...", "Abort", 0, static_cast<int>(dossiers.size()), parent_);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        int row(0);
        for (auto d : dossiers) {
            progress.setValue(row);
            QCoreApplication::processEvents();
            FillRow(row++, d);
            if (progress.wasCanceled()) {
                return;
            }
        }
    }
    catch (std::exception& exc) {
         qWarning() << "DossierPage::GetDossiers Caught exception: " << exc.what();
         QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

// Can leak exceptions
void DossierPage::FillRow(int row, int dossier) {
    Treatment tr(parent_->repo()->GetTreatment(dossier));
    Patient patient(parent_->repo()->GetPatient(dossier));
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::DOSSIER, new AlignedQTableWidgetItem(QString::number(dossier), Qt::AlignCenter));
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::PATIENT, new QTableWidgetItem(patient.GetLastName().toUpper() + " " + patient.GetFirstName()));
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::TREATMENT_TYPE, new QTableWidgetItem(tr.TreatmentType()));
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::COMMENTS, new QTableWidgetItem((tr.Comments().length() > 15 ? tr.Comments().left(12) + "..." : tr.Comments())));
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::MODULATEUR, new AlignedQTableWidgetItem(tr.GetModulateur().id(), Qt::AlignCenter));
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::DEGRADEUR, new AlignedQTableWidgetItem(tr.GetDegradeurSet().mm_plexis().join(", "), Qt::AlignCenter));
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::EPAISSEUR, new AlignedQTableWidgetItem(QString::number(((double)(tr.GetDegradeurSet().um_plexi_total() + tr.GetModulateur().RangeShift()) / 1000.0), 'f', 1), Qt::AlignCenter));
    try {
        parent_->repo()->GetCompensateur(dossier);
        parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::COMPENSATEUR, new AlignedQTableWidgetItem(QString("OUI"), Qt::AlignCenter));
    }
    catch (...) {}

    try {
        parent_->repo()->GetCollimator(dossier);
        parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::COLLIMATEUR, new AlignedQTableWidgetItem(QString("OUI"), Qt::AlignCenter));
    }
    catch (...) {}

    try {
        Dosimetry mc_dos(parent_->repo()->GetMCDosimetry(dossier));
        if (mc_dos.IsValid()) {
            parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::DMCNPX, new AlignedQTableWidgetItem(QString::number(mc_dos.GetDebitMean(), 'f', 4), Qt::AlignCenter));
            double debit_debroca(util::DebitDebroca(tr.dref(), tr.GetModulateur(), tr.GetDegradeurSet()));
            parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::DDEBROCA, new AlignedQTableWidgetItem(QString::number(debit_debroca, 'f', 4), Qt::AlignCenter));
            parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::DRATIO, new AlignedQTableWidgetItem(QString::number(mc_dos.GetDebitMean() / debit_debroca, 'f', 2), Qt::AlignCenter));
        }
    }
    catch (...) {}

    try {
        Dosimetry measured_dos(parent_->repo()->GetMeasuredDosimetry(dossier));
        if (measured_dos.IsValid()) {
            Dosimetry ref_dos(parent_->repo()->GetReferenceDosimetry(measured_dos.GetTimestamp()));
            if (measured_dos.IsValid() && ref_dos.IsValid()) {
                parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::DMESURE, new AlignedQTableWidgetItem(QString::number(measured_dos.GetDebitMean() * tr.dref() / ref_dos.GetDebitMean(), 'f', 4), Qt::AlignCenter));
            }
        }
    }
    catch (...) {}

    const int n_seances((int)tr.GetAllSeances().size());
    const int n_done((int)tr.GetPastSeances().size());
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::SEANCES, new AlignedQTableWidgetItem(QString::number(n_done) + " / " + QString::number(n_seances), Qt::AlignCenter));
    parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::DOSE, new AlignedQTableWidgetItem(QString::number(tr.GetDeliveredDose(), 'f', 2) + " / " +
                                                                                                         QString::number(tr.GetPrescribedDose(), 'f', 2), Qt::AlignCenter));
    try {
        std::pair<int, double> xray_exposure = parent_->repo()->GetXRayExposure(dossier);
        parent_->ui()->listDossier_dossierTableWidget->setItem(row, (int)COLUMNS::XRAYS, new AlignedQTableWidgetItem(QString::number(xray_exposure.first) + QString(" / ") + QString::number(xray_exposure.second, 'f', 1), Qt::AlignCenter));
    }
    catch (...) {}
}

int DossierPage::GetDossierNumber(int table_row) {
    QTableWidgetItem *item(parent_->ui()->listDossier_dossierTableWidget->item(table_row, (int)COLUMNS::DOSSIER));
    if (!item) {
        qWarning() << "DossierPage::GetDossierNumber No such cell";
        return -1;
    }
    bool conversion_ok(false);
    int dossier(item->text().toInt(&conversion_ok));
    if (!conversion_ok) {
        qWarning() << "DossierPage::GetDossierNumber Non valid dossier number";
        return -1;
    }
    return dossier;
}

void DossierPage::ShowContextMenu(int row, int col) {
    if (!parent_->ui()->listDossier_dossierTableWidget->item(row, col)) {
        qWarning() << "DossierPage::ShowContextMenu Not item in cell";
        return;
    }

    QMenu contextMenu(tr("Dossier table context menu"), parent_);
    switch (col) {
    case (int)COLUMNS::DOSSIER:
        break;
    case (int)COLUMNS::PATIENT:
        contextMenu.addAction("Modifier le nom du patient", [this, row](){ ModifyPatientName(row); });
        break;
    case (int)COLUMNS::TREATMENT_TYPE:
        contextMenu.addAction("Changer le type de traitement", [this, row](){ ChangeTreatmentType(row); });
        break;
    case (int)COLUMNS::COMMENTS:
        ShowComments(row);
        break;
    case (int)COLUMNS::MODULATEUR:
        contextMenu.addAction("Afficher le modulateur", [this, row]() { ShowModulateur(row); });
        contextMenu.addAction("Afficher le programme de fraisage", [this, row]() { ShowModulateurMillingProgram(row); });
//        ShowModulateur(row);
        break;
    case (int)COLUMNS::COLLIMATEUR:
//        ShowCollimator(row);
        contextMenu.addAction("Afficher le collimateur", [this, row]() { ShowCollimator(row); });
        contextMenu.addAction("Afficher le programme de fraisage", [this, row]() { ShowCollimatorMillingProgram(row); });
        break;
    case (int)COLUMNS::COMPENSATEUR:
//        ShowCompensateur(row);
        contextMenu.addAction("Afficher la transparence du compensateur", [this, row]() { ShowCompensateur(row); });
        contextMenu.addAction("Afficher le programme de fraisage", [this, row]() { ShowCompensateurMillingProgram(row); });
        break;
    case (int)COLUMNS::DMESURE:
        break;
    case (int)COLUMNS::DMCNPX:
        break;
    case (int)COLUMNS::DDEBROCA:
        break;
    case (int)COLUMNS::SEANCES:
        contextMenu.addAction("Passer à la prochaine séance", [this, row](){ IncrementActiveSeance(row); });
        contextMenu.addAction("Ajouter une séance", this, [this, row](){AddSeance(row); });
        contextMenu.addAction("Supprimer une séance", this, [this, row](){DeleteSeance(row); });
        break;
    case (int)COLUMNS::DOSE:
        contextMenu.addAction("Afficher la dose délivrée", this, [this, row](){ ShowDeliveredDose(row); });
        contextMenu.addAction("Modifier la dose délivrée", this, [this, row](){ ModifyDeliveredDose(row); });
        contextMenu.addAction("Modifier la dose planifiée", this, [this, row](){ ModifyPlannedDose(row); });
        break;
    default:
        qWarning() << "DossierPage::ShowContextMenu Non valid column index";
        break;
    }

    if (contextMenu.actions().empty()) {
        qWarning() << "DossierPage::ShowContextMenu No actions loaded in context menu";
        return;
    }

    contextMenu.exec(QCursor::pos());
}

void DossierPage::ChangeTreatmentType(int row) {
    if (!parent_->repo()->CheckConnection()) { return; }
    const int dossier(GetDossierNumber(row));
    ChangeTreatmentTypeDialog dialog(parent_, parent_->repo(), dossier);
    if (dialog.exec() == QDialog::Accepted) {
        FillRow(row, dossier);
    }
}

void DossierPage::ModifyPatientName(int row) {
    if (!parent_->repo()->CheckConnection()) { return; }
    int dossier(GetDossierNumber(row));
    ModifyPatientNameDialog dialog(parent_, parent_->repo(), dossier);
    if (dialog.exec() == QDialog::Accepted) {
         GetDossiers(); // since one patient can have several dossiers
    }
}

void DossierPage::IncrementActiveSeance(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
         Treatment treatment = parent_->repo()->GetTreatment(dossier);
         const int active_seance(treatment.GetActiveSeanceNumber());
         if (active_seance == -1) {
              QMessageBox::information(parent_, "MTP", "Il n'y a plus de séances prévues");
              return;
         }
         treatment.IncrementSeance();
         const int next_seance(treatment.GetActiveSeanceNumber());
         QMessageBox msg_box;
         if (next_seance != -1) {
              msg_box.setText(QString("Changer la séance active de ") + QString::number(active_seance + 1) + " à " + QString::number(next_seance + 1) + "?");
         } else {
             msg_box.setText(QString("Ceci terminera ce traitement et fermera le dossier. Continuer?"));
         }
         msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
         msg_box.setDefaultButton(QMessageBox::No);
         int ret = msg_box.exec();
         switch (ret) {
         case QMessageBox::Yes:
             parent_->repo()->UpdateTreatment(treatment);
             FillRow(row, dossier);
             break;
         case QMessageBox::No:
             break;
         default:
             qWarning() << "DossierPage::IncrementActiveSeance Not covered case";
             break;
         }
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::IncrementActiveSeance Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::AddSeance(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        AddSeanceDialog dialog(parent_, parent_->repo(), dossier);
        if (dialog.exec() == QDialog::Accepted) {
            FillRow(row, dossier);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::AddSeance Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::DeleteSeance(int row) {
    QMessageBox::information(parent_, "MTP", "Fonction actuellement désactivée");
    return;

    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        DeleteSeanceDialog dialog(parent_, parent_->repo(), dossier);
        if (dialog.exec() == QDialog::Accepted) {
            FillRow(row, dossier);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::DeleteSeance Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::ShowDeliveredDose(int row) {
    if (!parent_->repo()->CheckConnection()) { return; }
    const int dossier(GetDossierNumber(row));
    ShowDeliveredDoseDialog dialog(parent_, parent_->repo(), dossier);
    dialog.InitGraph();
    dialog.exec();
}

void DossierPage::ModifyDeliveredDose(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        const int dossier(GetDossierNumber(row));
        ModifyDeliveredDoseDialog dialog(parent_, parent_->repo(), dossier);
        if (dialog.exec() == QDialog::Accepted) {
            FillRow(row, dossier);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ModifyDeliveredDose Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::ModifyPlannedDose(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        ModifyPlannedDoseDialog dialog(parent_, parent_->repo(), dossier);
        dialog.exec();
        FillRow(row, dossier);
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ModifyPlannedDose Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::ShowCollimator(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        Collimator collimator(parent_->repo()->GetCollimator(dossier));
        CollimateurDialog dialog(collimator);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ShowCollimator Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::ShowCollimatorMillingProgram(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        const QString program = parent_->repo()->GetTreatmentFraiseuseProgram(dossier, Collimator::program_type);
        MillingMachineProgramDialog dialog(parent_, program);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ShowCollimatorMillingProgram Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::ShowCompensateur(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        Compensateur compensateur(parent_->repo()->GetCompensateur(dossier));
        Patient patient(parent_->repo()->GetPatient(dossier));
        CompensateurTransparantDialog dialog(parent_, compensateur, patient);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ShowCompensator Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::ShowModulateur(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        Modulateur modulateur(parent_->repo()->GetModulateurForDossier(dossier));
        ModulateurDialog dialog(parent_, modulateur);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ShowModulateur Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::ShowModulateurMillingProgram(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        const QString program = parent_->repo()->GetTreatmentFraiseuseProgram(dossier, Modulateur::program_type);
        MillingMachineProgramDialog dialog(parent_, program);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ShowModulateurMillingProgram Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }

}

void DossierPage::ShowCompensateurMillingProgram(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        const QString program = parent_->repo()->GetTreatmentFraiseuseProgram(dossier, Compensateur::program_type);
        MillingMachineProgramDialog dialog(parent_, program);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ShowCompensateurMillingProgram Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::ShowComments(int row) {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        int dossier(GetDossierNumber(row));
        CommentsDialog dialog(parent_, parent_->repo(), dossier);
        dialog.exec();
        FillRow(row, dossier);
    }
    catch (std::exception& exc) {
        qWarning() << "DossierPage::ShowComments Caught exception: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void DossierPage::LaunchCreateNewDossierDialog() {
    if (!parent_->repo()->CheckConnection()) { return; }
    CreateDossierDialog dialog(parent_, parent_->repo());
    if (dialog.exec() == QDialog::Accepted) {
         GetDossiers();
    }
}

