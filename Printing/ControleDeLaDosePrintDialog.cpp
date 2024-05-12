#include "ControleDeLaDosePrintDialog.h"
#include "ui_ControleDeLaDosePrintDialog.h"

#include <QDateTime>
#include <algorithm>

#include "Treatment.h"
#include "Calc.h"
#include "Util.h"
#include "Defaults.h"

ControleDeLaDosePrintDialog::ControleDeLaDosePrintDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ControleDeLaDosePrintDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Controle de la dose"));
    SetupConnections();
    Fill();
}

ControleDeLaDosePrintDialog::~ControleDeLaDosePrintDialog()
{
    delete ui_;
}

void ControleDeLaDosePrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ControleDeLaDosePrintDialog::Fill() {
    ui_->dateLineEdit->setText(QDateTime::currentDateTime().toString());
    FillDossier();
    Treatment treatment(repo_->GetTreatment(dossier_));
    ui_->typeTraitementLineEdit->setText(treatment.TreatmentType());
    FillDose(treatment);
    FillReferenceDosimetry(treatment);
    if (treatment.GetPatientDosimetry().IsMonteCarlo()) {
        FillMonteCarlo(treatment);
    } else {
        FillMeasured(treatment);
    }
    FillUMParameters(treatment);

}

void ControleDeLaDosePrintDialog::FillDossier() {
    Patient patient(repo_->GetPatient(dossier_));
    ui_->nameLineEdit->setText(patient.GetLastName().toUpper() + " " + patient.GetFirstName());
    ui_->dossierLineEdit->setText(QString::number(dossier_));
}

void ControleDeLaDosePrintDialog::FillDose(Treatment& treatment) {
    std::vector<Seance> seances(treatment.GetAllSeances());
    if (!seances.empty()) {
        const double std_dose(seances.at(0).GetDosePrescribed());
        const int n_std(std::count_if(seances.begin(), seances.end(), [&std_dose](Seance s){return calc::AlmostEqual(std_dose, s.GetDosePrescribed(), 0.001);}));
        const int n_boost((int)seances.size() - n_std);
        const double boost_dose(n_boost > 0 ? seances.back().GetDosePrescribed() : 0.0);
        ui_->serie1DoseLineEdit->setText(QString::number(std_dose, 'f', 2));
        ui_->serie1SeancesLineEdit->setText(QString::number(n_std));
        if (n_boost > 0) {
            ui_->serie2DoseLineEdit->setText(QString::number(boost_dose, 'f', 2));
            ui_->serie2SeancesLineEdit->setText(QString::number(n_boost));
        }
        ui_->totalDoseLineEdit->setText(QString::number(treatment.GetPrescribedDose(), 'f', 2));
    }
}

void ControleDeLaDosePrintDialog::FillReferenceDosimetry(Treatment& treatment) {
    ui_->topReferenceLineEdit->setText(QString::number(treatment.dref(), 'f', 4));
}

void ControleDeLaDosePrintDialog::FillMonteCarlo(Treatment &treatment) {
    if (!treatment.GetPatientDosimetry().IsValid()) {
        return;
    }

    // mcnpx
    double mcnpx_debit(treatment.GetPatientDosimetry().GetDebitMean());
    ui_->debitMCNPXLineEdit->setText(QString::number(mcnpx_debit, 'f', 4));

    // debroca
    const double debroca_debit(util::DebitDebroca(treatment.dref(), treatment.GetModulateur(), treatment.GetDegradeurSet()));
    ui_->debitDebrocaLineEdit->setText(QString::number(debroca_debit, 'f', 4));

    // ratio
    ui_->ratioDebitCalculesLineEdit->setText(QString::number(mcnpx_debit / debroca_debit, 'f', 2));
}

void ControleDeLaDosePrintDialog::FillMeasured(Treatment &treatment) {
    auto dos(treatment.GetPatientDosimetry());
    if (!dos.IsValid()) {
         return;
    }
    auto ref(treatment.GetPatientRefDosimetry());
    if (!ref.IsValid()) {
        return;
    }
    ui_->debitMesurerLineEdit->setText(QString::number(dos.GetDebitMean() * treatment.dref() / ref.GetDebitMean(),
                                                       'f', 4));
}

void ControleDeLaDosePrintDialog::FillUMParameters(Treatment& treatment) {
    auto dos(treatment.GetPatientDosimetry());
    if (!dos.IsValid()) {
         return;
    }
    double dose(treatment.GetAllSeances().empty() ? 0.0 : treatment.GetSeance(0).GetDosePrescribed());
    double debit(dos.GetDebitMean());

    ui_->umLineEdit->setText(QString::number(calc::DoseToUM(debit, dose), 'f', 0));
    double i_stripper = 0.0;
    double duration = 0.0;
    util::GetStripperCurrentAndSeanceDuration(debit, dose, 10.0, &i_stripper, &duration);
    ui_->courantStripperLineEdit->setText(QString::number(i_stripper, 'f', 0));
    ui_->umRetirerLineEdit->setText(QString::number(util::GetUMCorrection(debit, i_stripper), 'f', 0));
}

QWidget* ControleDeLaDosePrintDialog::Widget() {
    return ui_->controleDeLaDoseWidget;
}

void ControleDeLaDosePrintDialog::Print(QPrinter* printer) {
    printer->setOrientation(QPrinter::Portrait);
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(false);

    QCPPainter painter(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    int plotWidth = ui_->controleDeLaDoseWidget->width();
    int plotHeight = ui_->controleDeLaDoseWidget->height();
    double w_scale = pageRect.width() / (double)plotWidth;
    double h_scale = pageRect.height() / (double)plotHeight;

    painter.scale(w_scale, h_scale);
    ui_->controleDeLaDoseWidget->render(&painter);
    painter.end();
}
