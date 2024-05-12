#ifndef CONTROLEDELADOSEPRINTDIALOG_H
#define CONTROLEDELADOSEPRINTDIALOG_H

#include <QDialog>
#include <QPrinter>

#include "PTRepo.h"



namespace Ui {
class ControleDeLaDosePrintDialog;
}

class ControleDeLaDosePrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ControleDeLaDosePrintDialog(QWidget *parent, PTRepo* repo, int dossier);
    ~ControleDeLaDosePrintDialog();
    void Print(QPrinter* printer);
    QWidget* Widget();
    QPrinter::Orientation Orientation() const {
        return QPrinter::Portrait;
    }

public slots:

private:
    void SetupConnections();
    void Fill();
    void FillDossier();
    void FillDose(Treatment& treatment);
    void FillReferenceDosimetry(Treatment& treatment);
    void FillMonteCarlo(Treatment& treatment);
    void FillMeasured(Treatment& treatment);
    void FillUMParameters(Treatment& treatment);
    Ui::ControleDeLaDosePrintDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif
