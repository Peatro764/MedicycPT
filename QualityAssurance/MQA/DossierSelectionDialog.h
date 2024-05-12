#ifndef DOSSIERSELECTIONDIALOG_H
#define DOSSIERSELECTIONDIALOG_H

#include <QDialog>

#include "Treatment.h"
#include "PTRepo.h"

namespace Ui {
class DossierSelectionDialog;
}

class DossierSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DossierSelectionDialog(QWidget *parent, PTRepo *repo);
    ~DossierSelectionDialog();
    int dossier() const { return dossier_; }

public slots:

private slots:
    void GetSelectedDossier();

private:
    void ConnectSignals();
    void SetupTable();
    void FillTable();
    void FillRow(int row, int dossier);
    Ui::DossierSelectionDialog *ui_;
    enum class COLUMNS : int { DOSSIER = 0, NOM = 1, SEANCE = 2 };
    PTRepo *repo_;
    int dossier_ = 0;
};

#endif
