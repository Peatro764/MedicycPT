#include "DoseRateSettingsDialog.h"
#include "ui_DoseRateSettingsDialog.h"

DoseRateSettingsDialog::DoseRateSettingsDialog(const Chambre& chambre, QWidget *parent) :
    QDialog(parent),
    chambre_(chambre),
    ui(new Ui::DoseRateSettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Chambre");

    QObject::connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()));

    WriteAll();
}

DoseRateSettingsDialog::~DoseRateSettingsDialog()
{
    delete ui;
}

void DoseRateSettingsDialog::WriteAll() {
    ui->dateLineEdit->setText(chambre_.timestamp().toString());
    ui->dosimetrySettingsChambreLineEdit->setText(chambre_.name());
    ui->kqq0LineEdit->setText(QString::number(chambre_.kqq0(), 'f', 3));
    ui->ndLineEdit->setText(QString::number(chambre_.nd(), 'f', 3));    
    ui->yoffsetLineEdit->setText(QString::number(chambre_.yoffset(), 'f', 1));
    ui->zoffsetLineEdit->setText(QString::number(chambre_.zoffset(), 'f', 1));
}
