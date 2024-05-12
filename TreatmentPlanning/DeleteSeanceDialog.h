#ifndef DELETESEANCEDIALOG_H
#define DELETESEANCEDIALOG_H

#include <QDialog>

#include "PTRepo.h"



namespace Ui {
class DeleteSeanceDialog;
}

class DeleteSeanceDialog : public QDialog
{
    Q_OBJECT

public:
    DeleteSeanceDialog(QWidget *parent, PTRepo *repo, int dossier);

public slots:
    void DeleteSeance();
    void FillSeanceComboBox();

private:
    Ui::DeleteSeanceDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

