#ifndef SOBPPRINTDIALOG_H
#define SOBPRINTDIALOG_H

#include <QDialog>
#include <QPrinter>
#include <memory>

#include "QARepo.h"
#include "LinePlot.h"

namespace Ui {
class SOBPPrintDialog;
}

class SOBPPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SOBPPrintDialog(QWidget *parent, QARepo* repo);
    ~SOBPPrintDialog();
    void Print(QPrinter* printer);
    QWidget* Widget();
    QPrinter::Orientation Orientation() const {
        return QPrinter::Portrait;
    }
    void LoadLastSOBP();
    void Load(int dossier);

public slots:

private:
    void SetupConnections();
    void SetupGraphs();
    void Fill(const SOBP& sobp);

    Ui::SOBPPrintDialog *ui_;
    QARepo* repo_;
    std::unique_ptr<LinePlot> sobp_graph_;
    QString plexi_graph_ = "Plexi";
    QString tissue_graph_ = "Tissue";
};

#endif
