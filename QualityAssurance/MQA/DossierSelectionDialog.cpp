#include "DossierSelectionDialog.h"
#include "ui_DossierSelectionDialog.h"

#include <QMessageBox>
#include <QTableWidget>

DossierSelectionDialog::DossierSelectionDialog(QWidget *parent, PTRepo* repo) :
    QDialog(parent),
    ui_(new Ui::DossierSelectionDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setWindowTitle("Choisir un dossier");
    SetupTable();
    FillTable();
    ConnectSignals();
}

DossierSelectionDialog::~DossierSelectionDialog()
{
    delete ui_;
}

void DossierSelectionDialog::ConnectSignals() {
    QObject::connect(ui_->pushButton_cancel, &QPushButton::clicked, this, [=]() { done(2); });
    QObject::connect(ui_->pushButton_ok, SIGNAL(clicked()), this, SLOT(GetSelectedDossier()));
}

void DossierSelectionDialog::SetupTable() {
    QStringList headerLabels;
    headerLabels << "Dossier" << "Nom" << "Séance";
    ui_->tableWidget->setHorizontalHeaderLabels(headerLabels);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::DOSSIER), 80);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::NOM), 220);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::SEANCE), 80);
    ui_->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void DossierSelectionDialog::GetSelectedDossier() {
    QTableWidgetItem *item = ui_->tableWidget->item(ui_->tableWidget->currentRow(), static_cast<int>(COLUMNS::DOSSIER));
    if (item) {
        dossier_ = item->data(Qt::DisplayRole).toInt();
        accept();
    } else {
        QMessageBox::warning(this, "MTP", QString("Dossier not found: "));
    }
}

void DossierSelectionDialog::FillTable() {
    try {
        int row = 0;
        ui_->tableWidget->clearContents();
        const std::vector<int> non_finished_dossiers = repo_->GetNonFinishedDossiers();
        ui_->tableWidget->setRowCount(static_cast<int>(non_finished_dossiers.size()));
        for (int dossier : non_finished_dossiers) {
            FillRow(row++, dossier);
        }
        ui_->tableWidget->setCurrentCell(0, 0);
    }
    catch (std::exception& exc) {
        qWarning() << QString("DossierSelectionDialog::FillTable Excpetion caught: ") + exc.what();
        QMessageBox::warning(this, "MTD", "Failed retrieving dossiers from database");
    }
}

void DossierSelectionDialog::FillRow(int row, int dossier) {
    try {
        Patient patient = repo_->GetPatient(dossier);
        ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::DOSSIER), new QTableWidgetItem(QString::number(dossier)));
        ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::NOM), new QTableWidgetItem(patient.GetLastName().toUpper() + " " + patient.GetFirstName()));
        ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::SEANCE), new QTableWidgetItem(QString::number(repo_->GetActiveSeanceNumber(dossier) + 1)  +
                                                                                               " (" + QString::number(static_cast<int>(repo_->GetSeances(dossier).size())) + ")"));
    }
    catch (std::exception& exc) {
        qWarning() << QString("DossierSelectionDialog::FillRow Excpetion caught: ") + exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

