#include "NumericInputDialog.h"
#include "ui_NumericInputDialog.h"

NumericInputDialog::NumericInputDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::NumericInputDialog)
{
    ui_->setupUi(this);

    QObject::connect(ui_->pushButton_0, &QPushButton::clicked, this, [&]() { AddNumber(0); });
    QObject::connect(ui_->pushButton_1, &QPushButton::clicked, this, [&]() { AddNumber(1); });
    QObject::connect(ui_->pushButton_2, &QPushButton::clicked, this, [&]() { AddNumber(2); });
    QObject::connect(ui_->pushButton_3, &QPushButton::clicked, this, [&]() { AddNumber(3); });
    QObject::connect(ui_->pushButton_4, &QPushButton::clicked, this, [&]() { AddNumber(4); });
    QObject::connect(ui_->pushButton_5, &QPushButton::clicked, this, [&]() { AddNumber(5); });
    QObject::connect(ui_->pushButton_6, &QPushButton::clicked, this, [&]() { AddNumber(6); });
    QObject::connect(ui_->pushButton_7, &QPushButton::clicked, this, [&]() { AddNumber(7); });
    QObject::connect(ui_->pushButton_8, &QPushButton::clicked, this, [&]() { AddNumber(8); });
    QObject::connect(ui_->pushButton_9, &QPushButton::clicked, this, [&]() { AddNumber(9); });
    QObject::connect(ui_->pushButton_clear, &QPushButton::clicked, ui_->label_dossier, &QLabel::clear);
    QObject::connect(ui_->pushButton_ok, &QPushButton::clicked, this, &QDialog::accept);
    QObject::connect(ui_->pushButton_cancel, &QPushButton::clicked, this, &QDialog::reject);
    setWindowTitle("Entrez le numéro de dossier");
}

void NumericInputDialog::AddNumber(int number) {
    ui_->label_dossier->setText(ui_->label_dossier->text() + QString::number(number));
    dossier_ = ui_->label_dossier->text().toInt();
}
