#include "ProgramTypeDialog.h"
#include "ui_ProgramTypeDialog.h"

#include "Collimator.h"
#include "Compensateur.h"

ProgramTypeDialog::ProgramTypeDialog(QWidget *parent, int id, QString program_type) :
    QDialog(parent),
    ui_(new Ui::ProgramTypeDialog),
    id_(id)
{
    ui_->setupUi(this);
    setWindowTitle("Fraiseuse");
    LoadProgramTypes();

    ui_->dossierLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{1,6}"), ui_->dossierLineEdit));
    ui_->dossierLineEdit->setText(QString::number(id));
    if (!program_type.isEmpty()) {
        ui_->programTypeComboBox->setCurrentText(program_type);
    }

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->okPushButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui_->programTypeComboBox, SIGNAL(currentTextChanged(QString)), SLOT(UpdateIdLabel()));
}

ProgramTypeDialog::~ProgramTypeDialog() {
    delete ui_;
}

void ProgramTypeDialog::UpdateIdLabel() {
    QString current_type(ui_->programTypeComboBox->currentText());
    if (current_type == Collimator::program_type || current_type == Compensateur::program_type) {
        ui_->dossierLabel->setText("Dossier");
    } else {
        ui_->dossierLabel->setText("Id");
    }
}

void ProgramTypeDialog::LoadProgramTypes() {
    ui_->programTypeComboBox->addItem(Collimator::program_type);
//    ui_->programTypeComboBox->addItem(QString("MODULATEUR"));
    ui_->programTypeComboBox->addItem(Compensateur::program_type);
    ui_->programTypeComboBox->addItem(QString("MISC"));
}

int ProgramTypeDialog::Id() const {
    return ui_->dossierLineEdit->text().toInt();
}

QString ProgramTypeDialog::ProgramType() const {
    return ui_->programTypeComboBox->currentText();
}
