#include "SaveSOBPDialog.h"
#include "ui_SaveSOBPDialog.h"

SaveSOBPDialog::SaveSOBPDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::SaveSOBPDialog)
{
    ui_->setupUi(this);

    SetupValidators();

    QObject::connect(ui_->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->pushButton_ok, SIGNAL(clicked()), this, SLOT(GetParameters()));
    QObject::connect(ui_->pushButton_ok, SIGNAL(clicked()), this, SLOT(accept()));
    setWindowTitle("Save SOBP");
}

void SaveSOBPDialog::GetParameters() {
    modulateur_ = ui_->lineEdit_modulateur->text().toInt();
    degradeur_mm_ = ui_->lineEdit_degradeur_mm->text().toDouble();
    comment_ = ui_->lineEdit_comment->text();
}

void SaveSOBPDialog::SetupValidators() {
    ui_->lineEdit_modulateur->setValidator(new QRegExpValidator(QRegExp("\\d{0,}"), ui_->lineEdit_modulateur));
    ui_->lineEdit_degradeur_mm->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), ui_->lineEdit_degradeur_mm));
}
