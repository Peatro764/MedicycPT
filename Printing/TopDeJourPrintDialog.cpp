#include "TopDeJourPrintDialog.h"
#include "ui_TopDeJourPrintDialog.h"

#include <QDateTime>

#include "Dosimetry.h"
#include "Calc.h"

TopDeJourPrintDialog::TopDeJourPrintDialog(QWidget *parent, std::shared_ptr<PTRepo> repo) :
    QDialog(parent),
    ui_(new Ui::TopDeJourPrintDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Dose Rate"));
    SetupConnections();
    SetupTable();
}

TopDeJourPrintDialog::~TopDeJourPrintDialog()
{
    delete ui_;
}

void TopDeJourPrintDialog::Clear() {
    ui_->dateLineEdit->clear();
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

void TopDeJourPrintDialog::Load() {
    Clear();
    Fill();
}

void TopDeJourPrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void TopDeJourPrintDialog::SetupTable() {
    QStringList headerLabels;
    headerLabels << "Duration\n(s)" << QString::fromUtf8("UM\nprévue") <<
                    "UM\ndélivrée" << "CS/CF9\n(nA)" << "CT1/CT2\n(nA)" << "Charge\n(nC)" << "Débit\n(cGy/UM)";
    ui_->seancesTableWidget->setHorizontalHeaderLabels(headerLabels);
}

void TopDeJourPrintDialog::Fill() {
    Dosimetry dos(repo_->GetTopDeJourDosimetry(QDate::currentDate()));
    FillDossier();
    FillConfig(dos);
    FillMeasurement(dos);
}

void TopDeJourPrintDialog::FillDossier() {
     ui_->dateLineEdit->setText(QDateTime::currentDateTime().toString());
}

void TopDeJourPrintDialog::FillConfig(Dosimetry& dos) {
    ui_->dosimetrieDateLineEdit->setText(dos.GetTimestamp().toString());
    ui_->modulateurLineEdit->setText(repo_->GetModulateurForDossier(dossier_).id());
    ui_->degradeurLineEdit->setText(repo_->GetDegradeurSetForDossier(dossier_).mm_plexis().join(", "));
    try {
        repo_->GetCompensateur(dossier_);
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

void TopDeJourPrintDialog::FillMeasurement(Dosimetry &dos) {
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
}

QWidget* TopDeJourPrintDialog::Widget() {
    return ui_->dosimetryWidget;
}

void TopDeJourPrintDialog::Print(QPrinter* printer) {
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
