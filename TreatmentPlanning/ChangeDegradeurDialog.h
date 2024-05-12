#ifndef CHANGEDEGRADEURDIALOG_H
#define CHANGEDEGRADEURDIALOG_H

#include <QDialog>

#include "PTRepo.h"
#include "Degradeur.h"



namespace Ui {
class ChangeDegradeurDialog;
}

class ChangeDegradeurDialog : public QDialog
{
    Q_OBJECT

public:
    ChangeDegradeurDialog(QWidget *parent, PTRepo *repo, int dossier);

public slots:
    void ChangeDegradeur();

private:
    void FillDegradeurs();
    void SetActiveDegradeur();
    DegradeurSet GetSelectedDegradeurSet();
    Ui::ChangeDegradeurDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

