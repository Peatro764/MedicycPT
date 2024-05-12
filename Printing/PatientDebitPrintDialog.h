#ifndef PatientDebitPrintDialog_H
#define PatientDebitPrintDialog_H

#include <QDialog>
#include <QPrinter>

#include "PTRepo.h"

namespace Ui {
class PatientDebitPrintDialog;
}

class PatientDebitPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PatientDebitPrintDialog(QWidget *parent, PTRepo* repo);
    ~PatientDebitPrintDialog();
    void LoadLastDossier();
    void Load(int dossier);
    void Print(QPrinter* printer);
    QWidget* Widget();
    QPrinter::Orientation Orientation() const {
        return QPrinter::Portrait;
    }

public slots:

private:
    void SetupConnections();
    void SetupTable();
    void Clear();
    void FillDossier(int dossier);
    void FillConfig(Dosimetry& dos, int dossier);
    void FillMeasurement(Dosimetry& dos);

    Ui::PatientDebitPrintDialog *ui_;
    PTRepo* repo_;
    enum class DOSCOLS : char { DURATION = 0, UMPRE = 1, UMDEL = 2, CSCF9 = 3, CT1CT2 = 4,
                                CHARGE = 5, DEBIT = 6};
};

#endif
