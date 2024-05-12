#ifndef QUALITYASSURANCE_ShowBraggPeakDialog_H
#define QUALITYASSURANCE_ShowBraggPeakDialog_H

#include <QDialog>
#include <memory>

#include "BraggPeak.h"
#include "LinePlot.h"

namespace Ui {
class ShowBraggPeakDialog;
}

class ShowBraggPeakDialog : public QDialog
{
    Q_OBJECT

public:
    ShowBraggPeakDialog(QWidget *parent, BraggPeak braggpeak);
    ~ShowBraggPeakDialog();

private slots:
    void ExportToCSV();
    void Print();

private:
    Ui::ShowBraggPeakDialog *ui_;
    std::shared_ptr<LinePlot> bp_graph_;
    BraggPeak braggpeak_;
}
;

#endif

