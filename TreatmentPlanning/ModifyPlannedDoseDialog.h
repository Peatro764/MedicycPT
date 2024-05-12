#ifndef MODIFYPLANNEDDOSEDIALOG_H
#define MODIFYPLANNEDDOSEDIALOG_H

#include <QDialog>

#include "PTRepo.h"



namespace Ui {
class ModifyPlannedDoseDialog;
}

class ModifyPlannedDoseDialog : public QDialog
{
    Q_OBJECT

public:
    ModifyPlannedDoseDialog(QWidget *parent, PTRepo *repo, int dossier);

public slots:
    void ModifyDose();
    void FillPlannedDoseComboBox();

private:
    Ui::ModifyPlannedDoseDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

