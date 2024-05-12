#ifndef MODIFYPATIENTNAMEDIALOG_H
#define MODIFYPATIENTNAMEDIALOG_H

#include <QDialog>

#include "PTRepo.h"



namespace Ui {
class ModifyPatientNameDialog;
}

class ModifyPatientNameDialog : public QDialog
{
    Q_OBJECT

public:
    ModifyPatientNameDialog(QWidget *parent, PTRepo *repo, int dossier);

public slots:
    void ModifyName();

private:
    Ui::ModifyPatientNameDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

