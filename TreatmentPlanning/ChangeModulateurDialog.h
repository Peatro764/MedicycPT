#ifndef CHANGEMODULATEURDIALOG_H
#define CHANGEMODULATEURDIALOG_H

#include <QDialog>

#include "PTRepo.h"



namespace Ui {
class ChangeModulateurDialog;
}

class ChangeModulateurDialog : public QDialog
{
    Q_OBJECT

public:
    ChangeModulateurDialog(QWidget *parent, PTRepo *repo, int dossier);

public slots:
    void ChangeModulateur();

private:
    void FillModulateurs();
    void SetActiveModulateur();
    Ui::ChangeModulateurDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

