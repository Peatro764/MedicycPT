#ifndef ADDSEANCEDIALOG_H
#define ADDSEANCEDIALOG_H

#include <QDialog>

#include "PTRepo.h"



namespace Ui {
class AddSeanceDialog;
}

class AddSeanceDialog : public QDialog
{
    Q_OBJECT

public:
    AddSeanceDialog(QWidget *parent, PTRepo *repo, int dossier);

public slots:
    void AddSeance();

private:
    Ui::AddSeanceDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

