#ifndef CHANGETREATMENTTYPEDIALOG_H
#define CHANGETREATMENTTYPEDIALOG_H

#include <QDialog>

#include "PTRepo.h"

namespace Ui {
class ChangeTreatmentTypeDialog;
}

class ChangeTreatmentTypeDialog : public QDialog
{
    Q_OBJECT

public:
    ChangeTreatmentTypeDialog(QWidget *parent, PTRepo *repo, int dossier);

private slots:
    void ChangeTreatmentType();

private:
    void FillTreatmentTypes();
    Ui::ChangeTreatmentTypeDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

