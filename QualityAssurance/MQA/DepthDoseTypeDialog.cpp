#include "DepthDoseTypeDialog.h"
#include "ui_DepthDoseTypeDialog.h"

DepthDoseTypeDialog::DepthDoseTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::DepthDoseTypeDialog)
{
    ui_->setupUi(this);
    SetupValidators();

    QObject::connect(ui_->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->pushButton_save, SIGNAL(clicked()), this, SLOT(ValidateParameters()));
    QObject::connect(ui_->radioButton_sobp_patient, SIGNAL(toggled(bool)), ui_->groupBox_sobp_patient, SLOT(setEnabled(bool)));
    setWindowTitle("Depth dose type");
}

bool DepthDoseTypeDialog::BraggPeakChosen() const {
    return ui_->radioButton_braggpeak->isChecked();
}

bool DepthDoseTypeDialog::SOBPJourChosen() const {
    return ui_->radioButton_sobp_jour->isChecked();
}

bool DepthDoseTypeDialog::SOBPPatientChosen() const {
    return ui_->radioButton_sobp_patient->isChecked();
}

void DepthDoseTypeDialog::SetupValidators() {
    ui_->lineEdit_dossier->setValidator(new QRegExpValidator(QRegExp("\\d{0,}"), ui_->lineEdit_dossier));
    ui_->lineEdit_modulateur->setValidator(new QRegExpValidator(QRegExp("\\d{0,}"), ui_->lineEdit_modulateur));
    ui_->lineEdit_degradeur_mm->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), ui_->lineEdit_degradeur_mm));
}

void DepthDoseTypeDialog::ValidateParameters() {
    if (!ui_->radioButton_sobp_patient->isChecked()) {
        accept();
    } else {
        if (ui_->lineEdit_degradeur_mm->text().isEmpty() || ui_->lineEdit_modulateur->text().isEmpty()) {
            QMessageBox::warning(this, tr("Patient SOBP"), "A modulator id and range shifter thickness must be given");
        } else {
            modulateur_ = ui_->lineEdit_modulateur->text().isEmpty() ? 0 : ui_->lineEdit_modulateur->text().toInt();
            degradeur_mm_ = ui_->lineEdit_degradeur_mm->text().toDouble();
            dossier_ = ui_->lineEdit_dossier->text().toInt();
            comment_ = ui_->lineEdit_comment->text();
            accept();
        }
    }
}
