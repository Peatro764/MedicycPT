#ifndef ASSOCIATEDIALOG_H
#define ASSOCIATEDIALOG_H

#include <QDialog>

#include "PTRepo.h"
#include "Degradeur.h"
#include "Modulateur.h"



namespace Ui {
class AssociateDialog;
}

class AssociateDialog : public QDialog
{
    Q_OBJECT

public:
    AssociateDialog(QWidget *parent, PTRepo *repo,
                    const DegradeurSet& deg_set, const Modulateur& mod, double mcnpx_debit);

public slots:
    void Associate();

private:
    Ui::AssociateDialog *ui_;
    PTRepo *repo_;
    DegradeurSet deg_set_;
    Modulateur mod_;
    double mcnpx_debit_ = 0.0;
};

#endif

