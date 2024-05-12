#ifndef TREATMENTPRINTDIALOG_H
#define TREATMENTPRINTDIALOG_H

#include <QDialog>
#include <QPrinter>

#include "PTRepo.h"



namespace Ui {
class TreatmentPrintDialog;
}

class TreatmentPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TreatmentPrintDialog(QWidget *parent, PTRepo* repo, int dossier);
    ~TreatmentPrintDialog();
    void Print(QPrinter* printer);
    QWidget* Widget();
    QPrinter::Orientation Orientation() const {
        return QPrinter::Portrait;
    }

public slots:

private:
    void SetupConnections();
    void SetupTable();
    void Fill();
    void FillDossier();
    void FillSeances(Treatment& treatment);
    Ui::TreatmentPrintDialog *ui_;
    PTRepo *repo_;
    int dossier_;

    enum class GRIDCOLS : char { NUMBER = 0, DATE = 1, TOP = 2, DEBIT = 3, UMPREVU = 4,
                                 CSCF9 = 5, CT1CT2 = 6, UMDEL = 7, TPS = 8, DOSEFRACTION = 9, DOSETOTALE = 10 };
};

#endif
