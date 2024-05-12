#ifndef PROFILEPRINTDIALOG_H
#define PROFILEPRINTDIALOG_H

#include <QDialog>
#include <QPrinter>
#include <memory>

#include "QARepo.h"
#include "PTRepo.h"
#include "LinePlot.h"

namespace Ui {
class ProfilePrintDialog;
}

class ProfilePrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProfilePrintDialog(QWidget *parent, std::shared_ptr<QARepo> repo);
    ~ProfilePrintDialog();
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

    Ui::ProfilePrintDialog *ui_;
    std::shared_ptr<QARepo> repo_;
    std::unique_ptr<LinePlot> profile_graph_;
};

#endif
