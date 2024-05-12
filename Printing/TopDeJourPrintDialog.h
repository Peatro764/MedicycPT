#ifndef TopDeJourPrintDialog_H
#define TopDeJourPrintDialog_H

#include <QDialog>
#include <QPrinter>

#include "PTRepo.h"

namespace Ui {
class TopDeJourPrintDialog;
}

class TopDeJourPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TopDeJourPrintDialog(QWidget *parent, std::shared_ptr<PTRepo> repo);
    ~TopDeJourPrintDialog();
    void Load();
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
    void Fill();
    void FillDossier();
    void FillConfig(Dosimetry& dos);
    void FillMeasurement(Dosimetry& dos);

    Ui::TopDeJourPrintDialog *ui_;
    std::shared_ptr<PTRepo> repo_;
    enum class DOSCOLS : char { DURATION = 0, UMPRE = 1, UMDEL = 2, CSCF9 = 3, CT1CT2 = 4,
                                CHARGE = 5, DEBIT = 6};
    const int dossier_ = 10000;
};

#endif
