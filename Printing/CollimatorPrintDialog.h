#ifndef COLLIMATORPRINTDIALOG_H
#define COLLIMATORPRINTDIALOG_H

#include <QDialog>
#include <QPrinter>

#include "PTRepo.h"



namespace Ui {
class CollimatorPrintDialog;
}

class CollimatorPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CollimatorPrintDialog(QWidget *parent, PTRepo* repo, int dossier);
    ~CollimatorPrintDialog();
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
    void FillCoordinates(Collimator& coll);
    Ui::CollimatorPrintDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif
