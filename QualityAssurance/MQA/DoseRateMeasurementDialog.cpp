#include "DoseRateMeasurementDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QRegExpValidator>

DoseRateMeasurementDialog::DoseRateMeasurementDialog() :
    QDialog(nullptr),
    charge_(0.0)
{
    QLabel *chargeLabel = new QLabel(tr("Veuillez saisir la charge intégrée mesurée:"));
    QLabel *unitLabel = new QLabel(tr("[nC]"));
    chargeLineEdit_ = new QLineEdit;
    chargeLineEdit_->setAlignment(Qt::AlignHCenter);

    confirmButton_ = new QPushButton(tr("&Ok"));
    abortButton_ = new QPushButton(tr("&Quitter"));

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(chargeLabel);
    inputLayout->addWidget(chargeLineEdit_);
    inputLayout->addWidget(unitLabel);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(inputLayout, 0, 0);
    buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox_, 1, 0);

    setLayout(mainLayout);
    setWindowTitle(tr("Mesure débit"));

    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(ProcessInput()));
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
    this->chargeLineEdit_->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), this->chargeLineEdit_));

    this->chargeLineEdit_->setFocus();
}

void DoseRateMeasurementDialog::ProcessInput()
{
    if (chargeLineEdit_->text().isEmpty()) {
        QMessageBox::information(this, tr("Empty Field"),
            tr("Please enter a value"));
        return;
    } else {
        // The validator ensures that toDouble will work
        charge_ = chargeLineEdit_->text().toDouble();
        chargeLineEdit_->clear();
        hide();
        accept();
    }
}

