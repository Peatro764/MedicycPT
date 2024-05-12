#ifndef CONTROLEQUALITEPRINTDIALOG_H
#define CONTROLEQUALITEPRINTDIALOG_H

#include <QDialog>
#include <QPrinter>

#include "PTRepo.h"



namespace Ui {
class ControleQualitePrintDialog;
}

class ControleQualitePrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ControleQualitePrintDialog(QWidget *parent, PTRepo* repo, int dossier);
    ~ControleQualitePrintDialog();
    void Print(QPrinter* printer);
    QWidget* Widget();
    QPrinter::Orientation Orientation() const {
        return QPrinter::Landscape;
    }

public slots:

private:
    void SetupConnections();
    void Fill();
    void FillDossier();
    void FillDose(Treatment& treatment);
    void FillInstrumentation();
    Ui::ControleQualitePrintDialog *ui_;
    PTRepo *repo_;
    int dossier_;
};

#endif
