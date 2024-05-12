#include "ControleQualitePrintDialog.h"
#include "ui_ControleQualitePrintDialog.h"

#include <QDateTime>
#include <algorithm>

#include "Treatment.h"
#include "Calc.h"
#include "Defaults.h"

ControleQualitePrintDialog::ControleQualitePrintDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ControleQualitePrintDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Contröle de qualité"));
    SetupConnections();
    Fill();
}

ControleQualitePrintDialog::~ControleQualitePrintDialog()
{
    delete ui_;
}

void ControleQualitePrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ControleQualitePrintDialog::Fill() {
    ui_->dateLineEdit->setText(QDateTime::currentDateTime().toString());
    FillDossier();
    Treatment treatment(repo_->GetTreatment(dossier_));
    FillDose(treatment);
    FillInstrumentation();
}

void ControleQualitePrintDialog::FillDossier() {
    Patient patient(repo_->GetPatient(dossier_));
    ui_->nameLineEdit->setText(patient.GetLastName().toUpper() + " " + patient.GetFirstName());
    ui_->dossierLineEdit->setText(QString::number(dossier_));
}

void ControleQualitePrintDialog::FillDose(Treatment& treatment) {
    std::vector<Seance> seances(treatment.GetAllSeances());
    if (!seances.empty()) {
        const double std_dose(seances.at(0).GetDosePrescribed());
        const int n_std(std::count_if(seances.begin(), seances.end(), [&std_dose](Seance s){return calc::AlmostEqual(std_dose, s.GetDosePrescribed(), 0.001);}));
        const int n_boost((int)seances.size() - n_std);
        const double boost_dose(n_boost > 0 ? seances.back().GetDosePrescribed() : 0.0);
        ui_->grandChampLineEdit->setText(QString::number(std_dose * n_std, 'f', 2));
        ui_->boostLineEdit->setText(QString::number(n_boost * boost_dose, 'f', 2));
        ui_->doseFractionGChpLineEdit->setText(QString::number(std_dose, 'f', 2));
        ui_->doseFractionBoostLineEdit->setText(QString::number(boost_dose, 'f', 2));
        ui_->nbreDeFractionsGChpLineEdit->setText(QString::number(n_std));
        ui_->nbreDeFractionsBoostLineEdit->setText(QString::number(n_boost));
        ui_->doseTotaleLineEdit->setText(QString::number(treatment.GetPrescribedDose(), 'f', 2));
    } else {
        ui_->doseTotaleLineEdit->setText("NA");
    }
}

void ControleQualitePrintDialog::FillInstrumentation() {
    ui_->modulateurLineEdit->setText(repo_->GetModulateurForDossier(dossier_).id());
    ui_->degradeurLineEdit->setText(repo_->GetDegradeurSetForDossier(dossier_).mm_plexis().join(", "));
}

QWidget* ControleQualitePrintDialog::Widget() {
    return ui_->controleQualiteWidget;
}

void ControleQualitePrintDialog::Print(QPrinter* printer) {
    printer->setOrientation(QPrinter::Landscape);
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(false);

    QCPPainter painter(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    int plotWidth = ui_->controleQualiteWidget->width();
    int plotHeight = ui_->controleQualiteWidget->height();
    double w_scale = pageRect.width() / (double)plotWidth;
    double h_scale = pageRect.height() / (double)plotHeight;

    painter.scale(w_scale, h_scale);
    ui_->controleQualiteWidget->render(&painter);
    painter.end();
}
