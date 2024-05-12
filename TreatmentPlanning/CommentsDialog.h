#ifndef COMMENTSDIALOG_H
#define COMMENTSDIALOG_H

#include <QDialog>

#include "PTRepo.h"

namespace Ui {
class CommentsDialog;
}

class CommentsDialog : public QDialog
{
    Q_OBJECT

public:
    CommentsDialog(QWidget *parent, PTRepo *repo, int dossier);

private slots:
    void Fill();
    void Save();

private:
    Ui::CommentsDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif

