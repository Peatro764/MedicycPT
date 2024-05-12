#ifndef QUALITYASSURANCE_PRINTDIALOG_H
#define QUALITYASSURANCE_PRINTDIALOG_H

#include <QDialog>

#include "QARepo.h"
#include "PTRepo.h"
#include "ProfilePrintDialog.h"
#include "BraggPeakPrintDialog.h"
#include "SOBPPrintDialog.h"
#include "TopDeJourPrintDialog.h"
#include "PatientDebitPrintDialog.h"

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent, std::shared_ptr<QARepo> qa_repo, std::shared_ptr<PTRepo> pt_repo);
    ~PrintDialog();

public slots:
    void LaunchTopDeJourDialog();
    void LaunchDebitPatientDialog();
    void Launch3DScanDialog();
    void LaunchProfileDialog();
    void LaunchPicDeBraggDialog();
    void LaunchSOBPDialog();
    void PrintSelected();

private:
    void SetupConnections();
    void CreateDialogs();
    Ui::PrintDialog *ui_;
    QPrinter printer_;
    std::shared_ptr<QARepo> qa_repo_;
    std::shared_ptr<PTRepo> pt_repo_;
    std::unique_ptr<ProfilePrintDialog> profile_dialog_;
    std::unique_ptr<BraggPeakPrintDialog> braggPeak_dialog_;
    std::unique_ptr<SOBPPrintDialog> sobp_dialog_;
    std::unique_ptr<TopDeJourPrintDialog> topDeJour_dialog_;
    std::unique_ptr<PatientDebitPrintDialog> patientDebit_dialog_;
    const QString mm_tissue_graph_ = QString("mm tissue");
    const QString mm_plexi_graph_ = QString("mm plexi");
};

#endif
