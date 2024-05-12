#ifndef QUALITYASSURANCE_ShowSOBPDialog_H
#define QUALITYASSURANCE_ShowSOBPDialog_H

#include <QDialog>
#include <memory>

#include "SOBP.h"
#include "LinePlot.h"

namespace Ui {
class ShowSOBPDialog;
}

class ShowSOBPDialog : public QDialog
{
    Q_OBJECT

public:
    ShowSOBPDialog(QWidget *parent, SOBP sobp);
    ~ShowSOBPDialog();

private slots:
    void ExportToCSV();
    void Print();

private:
    Ui::ShowSOBPDialog *ui_;
    std::shared_ptr<LinePlot> sobp_graph_;
    SOBP sobp_;
};

#endif

