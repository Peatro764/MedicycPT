#include "ManualMotorControlDialog.h"
#include "ui_ManualMotorControlDialog.h"

#include <vector>
#include <QDebug>
#include <QMessageBox>

ManualMotorControlDialog::ManualMotorControlDialog(QWidget *parent, Axis axis, std::shared_ptr<IMotor> motor)
    : QDialog(parent),
      ui_(new Ui::ManualMotorControlDialog),
      axis_(axis),
      motor_(motor) {
    ui_->setupUi(this);
    setWindowTitle("Manual Motor Control");
    SetupValidators();
    ConnectSignals();
    Setup();
    ui_->doubleSpinBox_des_pos->setValue(motor_->ActPos());
    ui_->doubleSpinBox_des_pos->setSingleStep(motor_->StepSize());
//    QObject::connect(ui_->pushButton_close, SIGNAL(clicked()), this, SLOT(accept()));
}

ManualMotorControlDialog::~ManualMotorControlDialog() {
    motor_->SetDefaultSpeed();
}

void ManualMotorControlDialog::SetupValidators() {
}

void ManualMotorControlDialog::Setup() {
    try {
        ui_->lineEdit_axis->setText(AxisToString(axis_));
        ui_->lineEdit_act_pos->setText(QString::number(motor_->ActPos(), 'f', 2));
    }
    catch (std::exception& exc) {
        qDebug() << "ManualMotorControl::Setup Exception thrown " << QString::fromStdString(exc.what());
        DisplayError(QString::fromStdString(exc.what()));
    }
}

void ManualMotorControlDialog::ConnectSignals() {
    try {
        QObject::connect(ui_->pushButton_close, SIGNAL(clicked(bool)), this, SLOT(reject()));
        QObject::connect(motor_.get(), &IMotor::CurrentPosition, this, [&](double pos) { ui_->lineEdit_act_pos->setText(QString::number(pos, 'f', 2)); });
        QObject::connect(ui_->pushButton_start, &QPushButton::clicked, this, &ManualMotorControlDialog::StartMovement);
        QObject::connect(ui_->pushButton_stop, &QPushButton::clicked, this, &ManualMotorControlDialog::StopMovement);
        QObject::connect(ui_->pushButton_zero, &QPushButton::clicked, this, &ManualMotorControlDialog::SetToZero);
    }
    catch (std::exception& exc) {
        qDebug() << "ManualMotorControl::ConnectSignals Exception thrown " << QString::fromStdString(exc.what());
        DisplayError(QString::fromStdString(exc.what()));
    }
}

void ManualMotorControlDialog::DisplayError(QString msg) {
    QMessageBox box;
    box.setText(msg);
    box.setIcon(QMessageBox::Critical);
    box.setModal(false);
    box.setAttribute(Qt::WA_DeleteOnClose);
    box.show();
}

void ManualMotorControlDialog::StartMovement() {
    try {
        motor_->SetFastSpeed();
        motor_->MoveToPosition(ui_->doubleSpinBox_des_pos->value());
    }
    catch (std::exception& exc) {
        qDebug() << "ManualMotorControl::StartMovement Exception thrown " << QString::fromStdString(exc.what());
        DisplayError(QString::fromStdString(exc.what()));
    }
}

void ManualMotorControlDialog::StopMovement() {
    try {
        motor_->Stop();
    }
    catch (std::exception& exc) {
        qDebug() << "ManualMotorControl::StopMovement Exception thrown " << QString::fromStdString(exc.what());
        DisplayError(QString::fromStdString(exc.what()));
    }

}

void ManualMotorControlDialog::SetToZero() {
    try {
        QMessageBox msg_box;
        msg_box.setText("Do you really want to set the current position to the start position?");
        msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg_box.setDefaultButton(QMessageBox::No);
        int ret = msg_box.exec();
        switch (ret) {
        case QMessageBox::Yes:
            motor_->SetCurrentToZeroPos();
            break;
        case QMessageBox::No:
            break;
        default:
            break;
        }
    }
    catch (std::exception& exc) {
        qDebug() << "ManualMotorControl::SetToZero Exception thrown " << QString::fromStdString(exc.what());
        DisplayError(QString::fromStdString(exc.what()));
    }
}

