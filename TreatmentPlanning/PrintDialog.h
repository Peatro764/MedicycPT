#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>

#include "PTRepo.h"
#include "QARepo.h"
#include "PatientDebitPrintDialog.h"
#include "TreatmentPrintDialog.h"
#include "CompensateurTransparantDialog.h"
#include "CollimatorPrintDialog.h"
#include "ControleDeLaDosePrintDialog.h"
#include "ControleQualitePrintDialog.h"
#include "SOBPPrintDialog.h"

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent, PTRepo* pt_repo, QARepo* qa_repo, int dossier);
    ~PrintDialog();

public slots:
    void LaunchSOBPDialog();
    void LaunchDebitMesureDialog();
    void LaunchTreatmentDialog();
    void LaunchCompensateurDialog();
    void LaunchCollimatorDialog();
    void LaunchControleDoseDialog();
    void LaunchControleQualiteDialog();
    void PrintSelected();

private:
    void SetupConnections();
    void CreateDialogs();
    Ui::PrintDialog *ui_;
    QPrinter printer_;
    PTRepo *pt_repo_;
    QARepo *qa_repo_;
    int dossier_;
    SOBPPrintDialog* sobp_dialog_;
    PatientDebitPrintDialog* patientDebit_dialog_;
    TreatmentPrintDialog* treatment_dialog_;
    CompensateurTransparantDialog* compensateur_dialog_;
    CollimatorPrintDialog* collimator_dialog_;
    ControleDeLaDosePrintDialog* controle_dose_dialog_;
    ControleQualitePrintDialog* controle_qualite_dialog_;

};

#endif
