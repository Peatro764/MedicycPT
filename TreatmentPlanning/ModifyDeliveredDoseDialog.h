#ifndef MODIFYDELIVEREDDOSEDIALOG_H
#define MODIFYDELIVEREDDOSEDIALOG_H

#include <QDialog>

#include "PTRepo.h"



namespace Ui {
class ModifyDeliveredDoseDialog;
}

class ModifyDeliveredDoseDialog : public QDialog
{
    Q_OBJECT

public:
    ModifyDeliveredDoseDialog(QWidget *parent, PTRepo *repo, int dossier);

public slots:
    void ModifyDose();
    void FillDeliveredDoseComboBox();

private:
    Ui::ModifyDeliveredDoseDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

