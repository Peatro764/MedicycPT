#ifndef SELECTPATIENTDIALOG_H
#define SELECTPATIENTDIALOG_H

#include <QDialog>

#include "PTRepo.h"



namespace Ui {
class SelectPatientDialog;
}

class SelectPatientDialog : public QDialog
{
    Q_OBJECT

public:
    SelectPatientDialog(QWidget *parent, PTRepo *repo);

public slots:
    Patient GetPatient() const; // can throw
    void UpdatePatientList();

private:
    Ui::SelectPatientDialog *ui_;
    PTRepo *repo_;
};

#endif

