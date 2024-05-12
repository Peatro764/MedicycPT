#include "CollimateurDialog.h"
#include "ui_CollimateurDialog.h"

CollimateurDialog::CollimateurDialog(const Collimator& collimator, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CollimateurDialog)
{
    ui->setupUi(this);
    setWindowTitle("Collimateur");
    QObject::connect(ui->okPushButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
//    ui->collimatorWidget->EnableDropShadowEffect();
    ui->collimatorWidget->PaintCollimator(collimator);
}

CollimateurDialog::~CollimateurDialog()
{
    delete ui;
}
