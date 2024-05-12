#ifndef SHOWDELIVEREDDOSEDIALOG_H
#define SHOWDELIVEREDDOSEDIALOG_H

#include <QDialog>

#include "PTRepo.h"
#include "SeanceDoseGraph.h"



namespace Ui {
class ShowDeliveredDoseDialog;
}

class ShowDeliveredDoseDialog : public QDialog
{
    Q_OBJECT

public:
    ShowDeliveredDoseDialog(QWidget *parent, PTRepo *repo, int dossier);
    ~ShowDeliveredDoseDialog();
    void InitGraph();

public slots:
    void ShowSeanceInfo(int seance_idx);

private:
    Ui::ShowDeliveredDoseDialog *ui_;
    PTRepo *repo_;
    int dossier_;
    SeanceDoseGraph *seance_dose_graph_;
};

#endif

