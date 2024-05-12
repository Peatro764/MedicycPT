#ifndef QUALITYASSURANCE_MANUALMOTORCONTROLDIALOG_H
#define QUALITYASSURANCE_MANUALMOTORCONTROLDIALOG_H

#include <QDialog>
#include <memory>

#include "IMotor.h"
#include "Axis.h"

#include <memory>

namespace Ui {
class ManualMotorControlDialog;
}

class ManualMotorControlDialog : public QDialog
{
    Q_OBJECT

public:
    ManualMotorControlDialog(QWidget *parent, Axis axis, std::shared_ptr<IMotor> motor);
    ~ManualMotorControlDialog();

public slots:

private slots:
    void StartMovement();
    void StopMovement();
    void SetToZero();

private:
    Ui::ManualMotorControlDialog *ui_;
    Axis axis_;
    std::shared_ptr<IMotor> motor_;
    void DisplayError(QString msg);
    void Setup();
    void ConnectSignals();
    void SetupValidators();
};

#endif

