#include "TreatmentPrintDialog.h"
#include "ui_TreatmentPrintDialog.h"

#include <QDateTime>
#include <algorithm>

#include "Treatment.h"
#include "Calc.h"
#include "Defaults.h"

TreatmentPrintDialog::TreatmentPrintDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::TreatmentPrintDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Fichier Traitement"));
    SetupConnections();
    SetupTable();
    Fill();
}

TreatmentPrintDialog::~TreatmentPrintDialog()
{
    delete ui_;
}

void TreatmentPrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void TreatmentPrintDialog::SetupTable() {
    QStringList headerLabels;
    headerLabels << "Seance" << "Date" << "TOP\nMi\n(cGy/UM)" << "Débit\nDi\n(cGy/UM)" << QString::fromUtf8("UM\nprévue") <<
                    "CS\nCF9\n(nA)" << "CT1\nCT2\n(nA)" << "UM1\\2\ndélivrée" << "Tps\n(s)" << "Dose\nfraction\n(Gy)" << "Dose\ntotale\n(Gy)";
    ui_->seancesTableWidget->setHorizontalHeaderLabels(headerLabels);
    ui_->seancesTableWidget->setColumnWidth(0, 50);
    ui_->seancesTableWidget->setColumnWidth(1, 50 + 12*5);            ;
    ui_->seancesTableWidget->setColumnWidth(2, 50);
    ui_->seancesTableWidget->setColumnWidth(3, 50);
    ui_->seancesTableWidget->setColumnWidth(4, 50);
    ui_->seancesTableWidget->setColumnWidth(5, 50);
    ui_->seancesTableWidget->setColumnWidth(6, 50);
    ui_->seancesTableWidget->setColumnWidth(7, 50);
    ui_->seancesTableWidget->setColumnWidth(8, 50);
    ui_->seancesTableWidget->setColumnWidth(9, 50);
    ui_->seancesTableWidget->setColumnWidth(10, 50);
    ui_->seancesTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui_->seancesTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void TreatmentPrintDialog::Fill() {
    ui_->dateLineEdit->setText(QDateTime::currentDateTime().toString());
    FillDossier();
    Treatment treatment(repo_->GetTreatment(dossier_));
    FillSeances(treatment);
}

void TreatmentPrintDialog::FillDossier() {
    Patient patient(repo_->GetPatient(dossier_));
    ui_->nameLineEdit->setText(patient.GetLastName().toUpper() + " " + patient.GetFirstName());
    ui_->dossierLineEdit->setText(QString::number(dossier_));
}

void TreatmentPrintDialog::FillSeances(Treatment& treatment) {
    std::vector<Seance> seances(treatment.GetAllSeances());
//    ui_->seancesTableWidget->setRowCount((int)seances.size());
    int seance(0);
    int row(0);
    double dose_total(0.0);
    for (Seance &s: seances) {
        int record(0);
        for (SeanceRecord &r : s.GetSeanceRecords()) {
            QString number(QString::number(seance + 1) + "." + QString::number(record + 1));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::NUMBER, new QTableWidgetItem(number));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::DATE, new QTableWidgetItem(r.GetTimestamp().toString()));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::DEBIT, new QTableWidgetItem(QString::number(r.GetDebit(), 'f', 4)));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::UMPREVU, new QTableWidgetItem(QString::number(r.GetUMPrevu())));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::CSCF9, new QTableWidgetItem(QString::number(r.GetIStripper(), 'f', 1) + "\n" +
                                                                                             QString::number(r.GetICF9(), 'f', 1)));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::CT1CT2, new QTableWidgetItem(QString::number(calc::Mean(r.GetIChambre1()), 'f', 1) + "\n" +
                                                                                              QString::number(calc::Mean(r.GetIChambre2()), 'f', 1)));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::UMDEL, new QTableWidgetItem(QString::number(r.GetTotalUM1Delivered()) + "\n" +
                                                                                             QString::number(r.GetTotalUM2Delivered())));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::TPS, new QTableWidgetItem(QString::number(r.GetTotalDuration(), 'f', 1)));
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::DOSEFRACTION, new QTableWidgetItem(QString::number(r.GetDoseDelivered(), 'f', 2)));
            dose_total += r.GetDoseDelivered();
            ui_->seancesTableWidget->setItem(row, (int)GRIDCOLS::DOSETOTALE, new QTableWidgetItem(QString::number(dose_total, 'f', 2)));
            record++;
            row++;
        }
        seance++;
    }
}

QWidget* TreatmentPrintDialog::Widget() {
    return ui_->treatmentWidget;
}

void TreatmentPrintDialog::Print(QPrinter* printer) {
    printer->setOrientation(QPrinter::Portrait);
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(false);

    QCPPainter painter(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    int plotWidth = ui_->treatmentWidget->width();
    int plotHeight = ui_->treatmentWidget->height();
    double w_scale = pageRect.width() / (double)plotWidth;
    double h_scale = pageRect.height() / (double)plotHeight;

    painter.scale(w_scale, h_scale);
    ui_->treatmentWidget->render(&painter);
    painter.end();
}
