#include "ModulateurDialog.h"
#include "ui_ModulateurDialog.h"

#include <QDebug>

ModulateurDialog::ModulateurDialog(QWidget *parent, const Modulateur& modulateur) :
    QDialog(parent),
    ui_(new Ui::ModulateurDialog),
    modulateur_(modulateur)
{
    ui_->setupUi(this);
    setWindowTitle("Modulateur");
    QObject::connect(ui_->okPushButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
//    ui->collimatorWidget->EnableDropShadowEffect();
    modulateur_graph_ = std::unique_ptr<ModulateurGraph>(new ModulateurGraph(ui_->customplot));
    modulateur_graph_->Plot(modulateur_.GetAngularSizes());
    ui_->lineEdit_id->setText(modulateur_.id());
    ui_->lineEdit_steps->setText(QString::number(modulateur_.steps()));
    ui_->lineEdit_base->setText(QString::number(modulateur_.RangeShift() / 1000));
    ui_->lineEdit_range->setText(QString::number(modulateur_.parcours(), 'f', 2));
    ui_->lineEdit_mod98->setText(QString::number(modulateur_.mod98(), 'f', 2));
    ui_->lineEdit_mod100->setText(QString::number(modulateur_.mod100(), 'f', 2));
}

ModulateurDialog::~ModulateurDialog()
{
    delete ui_;
}
