#ifndef DoseRateMeasurementDIALOG_H
#define DoseRateMeasurementDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>

class QLineEdit;
class QPushButton;

class DoseRateMeasurementDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DoseRateMeasurementDialog();
    double GetCharge() { return charge_; }

signals:
    
public slots:
    void ProcessInput();

private:
    QPushButton *confirmButton_;
    QPushButton *abortButton_;
    QLineEdit *chargeLineEdit_;
    double charge_;
    QDialogButtonBox* buttonBox_;
    
};

#endif // DoseRateMeasurementDIALOG_H

