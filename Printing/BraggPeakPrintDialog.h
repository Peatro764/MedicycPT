#ifndef BRAGGPEAKPRINTDIALOG_H
#define BRAGGPEAKPRINTDIALOG_H

#include <QDialog>
#include <QPrinter>
#include <memory>

#include "QARepo.h"
#include "LinePlot.h"

namespace Ui {
class BraggPeakPrintDialog;
}

class BraggPeakPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BraggPeakPrintDialog(QWidget *parent, std::shared_ptr<QARepo> repo);
    ~BraggPeakPrintDialog();
    void Print(QPrinter* printer);
    QWidget* Widget();
    QPrinter::Orientation Orientation() const {
        return QPrinter::Portrait;
    }

public slots:

private:
    void SetupConnections();
    void SetupGraphs();
    void Fill();

    Ui::BraggPeakPrintDialog *ui_;
    std::shared_ptr<QARepo> repo_;
    std::unique_ptr<LinePlot> braggPeak_graph_;
    QString plexi_graph_ = "Plexi";
    QString tissue_graph_ = "Tissue";
};

#endif
