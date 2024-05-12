#include "SaveScanner3DDialog.h"
#include "ui_SaveScanner3DDialog.h"

SaveScanner3DDialog::SaveScanner3DDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::SaveScanner3DDialog)
{
    ui_->setupUi(this);

    SetupValidators();

    QObject::connect(ui_->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->pushButton_ok, SIGNAL(clicked()), this, SLOT(GetParameters()));
    QObject::connect(ui_->pushButton_ok, SIGNAL(clicked()), this, SLOT(accept()));
    setWindowTitle("Save SOBP");
}

void SaveScanner3DDialog::GetParameters() {
    comment_ = ui_->lineEdit_comment->text();
}

void SaveScanner3DDialog::SetupValidators() {
}
