#include "PatientDebitPrintDialog.h"
#include "ui_PatientDebitPrintDialog.h"

#include <QDateTime>

#include "Dosimetry.h"
#include "Calc.h"

PatientDebitPrintDialog::PatientDebitPrintDialog(QWidget *parent, PTRepo* repo) :
    QDialog(parent),
    ui_(new Ui::PatientDebitPrintDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Dose Rate"));
    SetupConnections();
    SetupTable();
}

PatientDebitPrintDialog::~PatientDebitPrintDialog()
{
    delete ui_;
}

void PatientDebitPrintDialog::Clear() {
    ui_->dateLineEdit->clear();
    ui_->dossierLineEdit->clear();
    ui_->nameLineEdit->clear();
    ui_->dosimetrieDateLineEdit->clear();
    ui_->modulateurLineEdit->clear();
    ui_->degradeurLineEdit->clear();
    ui_->compensateurLineEdit->clear();
    ui_->filtreLineEdit->clear();
    ui_->seancesTableWidget->clearContents();
    ui_->chambreKqq0LineEdit->clear();
    ui_->chambreNdLineEdit->clear();
    ui_->chambreNomLineEdit->clear();
    ui_->temperatureLineEdit->clear();
    ui_->pressureLineEdit->clear();
    ui_->ftpLineEdit->clear();
    ui_->debitMesureLineEdit->clear();
}

void PatientDebitPrintDialog::LoadLastDossier() {
    Clear();
    Dosimetry dos(repo_->GetLastMeasuredPatientDosimetry());
    const int dossier = repo_->GetDossierForDosimetry(dos.GetTimestamp());
    FillDossier(dossier);
    FillConfig(dos, dossier);
    FillMeasurement(dos);
}

void PatientDebitPrintDialog::Load(int dossier) {
    Clear();
    Dosimetry dos = repo_->GetMeasuredDosimetry(dossier);
    FillDossier(dossier);
    FillConfig(dos, dossier);
    FillMeasurement(dos);
}

void PatientDebitPrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void PatientDebitPrintDialog::SetupTable() {
    QStringList headerLabels;
    headerLabels << "Duration\n(s)" << QString::fromUtf8("UM\nprévue") <<
                    "UM\ndélivrée" << "CS/CF9\n(nA)" << "CT1/CT2\n(nA)" << "Charge\n(nC)" << "Débit\n(cGy/UM)";
    ui_->seancesTableWidget->setHorizontalHeaderLabels(headerLabels);
}

void PatientDebitPrintDialog::FillDossier(int dossier) {
     ui_->dateLineEdit->setText(QDateTime::currentDateTime().toString());
     ui_->dossierLineEdit->setText(QString::number(dossier));
     Patient patient(repo_->GetPatient(dossier));
     ui_->nameLineEdit->setText(patient.GetLastName().toUpper() + " " + patient.GetFirstName());
}

void PatientDebitPrintDialog::FillConfig(Dosimetry& dos, int dossier) {
    ui_->dosimetrieDateLineEdit->setText(dos.GetTimestamp().toString());
    ui_->modulateurLineEdit->setText(repo_->GetModulateurForDossier(dossier).id());
    ui_->degradeurLineEdit->setText(repo_->GetDegradeurSetForDossier(dossier).mm_plexis().join(", "));
    try {
        repo_->GetCompensateur(dossier);
        ui_->compensateurLineEdit->setText("Oui");
    }
    catch (...) {
        ui_->compensateurLineEdit->setText("No");
    }

    ui_->filtreLineEdit->setText("Not impl");
    ui_->chambreNomLineEdit->setText(dos.GetChambre().name());
    ui_->chambreNdLineEdit->setText(QString::number(dos.GetChambre().nd(), 'f', 3));
    ui_->chambreKqq0LineEdit->setText(QString::number(dos.GetChambre().kqq0(), 'f', 3));


    ui_->temperatureLineEdit->setText(QString::number(dos.GetTemperature(), 'f', 1));
    ui_->pressureLineEdit->setText(QString::number(dos.GetPressure(), 'f', 1));
    ui_->ftpLineEdit->setText(QString::number(dos.GetFtp(), 'f', 2));
}

void PatientDebitPrintDialog::FillMeasurement(Dosimetry &dos) {
    int row(0);
    auto debits = dos.GetDebits();
    for (DosimetryRecord record : dos.GetRecords()) {
        ui_->seancesTableWidget->setItem(row, (int)DOSCOLS::DURATION, new QTableWidgetItem(QString::number(record.GetTotalDuration(), 'f', 2)));
        ui_->seancesTableWidget->setItem(row, (int)DOSCOLS::UMPRE, new QTableWidgetItem(QString::number(record.GetUMPrevu())));
        ui_->seancesTableWidget->setItem(row, (int)DOSCOLS::UMDEL, new QTableWidgetItem(QString::number(record.GetTotalUMDelivered())));
        ui_->seancesTableWidget->setItem(row, (int)DOSCOLS::CSCF9, new QTableWidgetItem(QString::number(record.GetIStripper(), 'f', 1) + "/" +
                                                                                        QString::number(record.GetICF9(), 'f', 1)));
        ui_->seancesTableWidget->setItem(row, (int)DOSCOLS::CT1CT2, new QTableWidgetItem(QString::number(calc::Mean(record.GetIChambre1()) , 'f', 1) + "/" +
                                                                                         QString::number(calc::Mean(record.GetIChambre2()) , 'f', 1)));
        ui_->seancesTableWidget->setItem(row, (int)DOSCOLS::CHARGE, new QTableWidgetItem(QString::number(record.GetCharge(), 'f', 3)));
        ui_->seancesTableWidget->setItem(row, (int)DOSCOLS::DEBIT, new QTableWidgetItem(QString::number(debits.at(row), 'f', 4)));
        row++;
    }

    ui_->debitMesureLineEdit->setText(QString::number(dos.GetDebitMean(), 'f', 4) +
                                          " \u00B1 " + QString::number(dos.GetDebitStdDev(), 'f', 4));
    try {
        Dosimetry tdj = repo_->GetReferenceDosimetry(dos.GetTimestamp());
        ui_->topDeJourLineEdit->setText(QString::number(tdj.GetDebitMean(), 'f', 4) +
                                        " \u00B1 " + QString::number(tdj.GetDebitStdDev(), 'f', 4));
        const double adjusted_debit = dos.GetDebitMean() * (repo_->GetDefaults().GetDRef() / tdj.GetDebitMean());
        const double adjusted_error = std::sqrt(std::pow(dos.GetDebitStdDev(), 2) + std::pow(tdj.GetDebitStdDev(), 2));
        ui_->debitAjusteLineEdit->setText(QString::number(adjusted_debit, 'f', 4) + " \u00B1 " + QString::number(adjusted_error, 'f', 4));
    }
    catch (std::exception& exc) {
        qDebug() << "PatientDebitPrintDialog::FillMeasurement Exception thrown " << exc.what();
        ui_->topDeJourLineEdit->setText("No Top");
        ui_->debitAjusteLineEdit->setText("-");
    }
}

QWidget* PatientDebitPrintDialog::Widget() {
    return ui_->dosimetryWidget;
}

void PatientDebitPrintDialog::Print(QPrinter* printer) {
    printer->setOrientation(QPrinter::Portrait);
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(false);

    QCPPainter painter(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    int plotWidth = ui_->dosimetryWidget->width();
    int plotHeight = ui_->dosimetryWidget->height();
    double w_scale = pageRect.width() / (double)plotWidth;
    double h_scale = pageRect.height() / (double)plotHeight;

    painter.scale(w_scale, h_scale);
    ui_->dosimetryWidget->render(&painter);
    painter.end();
}
