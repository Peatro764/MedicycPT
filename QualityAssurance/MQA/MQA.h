#ifndef QUALITYASSURANCE_MQA_H
#define QUALITYASSURANCE_MQA_H

#include <QSettings>
#include <QWidget>
#include <memory>

#include "BeamProfilerGui.h"
#include "BraggPeakGui.h"
#include "Scanner3DGui.h"
#include "TopDeJourGui.h"
#include "PatientDebitGui.h"
#include "BeamLineGui.h"
#include "QARepo.h"
#include "PTRepo.h"

namespace Ui {
class MQA;
}

class MQA : public QWidget
{
    Q_OBJECT

public:
    explicit MQA(QWidget *parent = 0);
    ~MQA();
    Ui::MQA* ui() { return ui_; }
    std::shared_ptr<QARepo> qa_repo() { return qa_repo_; }
    std::shared_ptr<PTRepo> pt_repo() { return pt_repo_; }

public slots:
    void GotoTopMenu();
    void GotoLeftMenu();
    void GotoRightMenu();
    void SetupDb();

    void ShowBeamProfilerWidget();
    void ShowBraggPeakWidget();
    void ShowScanner3DWidget();
    void ShowTopDeJourWidget();
    void ShowPatientDebitWidget();
    void ShowBeamLineWidget();
    void UpdateTitle(int index);
    void ConfigurePage(int index);

private slots:
    void LaunchPrintDialog();
    void LaunchCSVDialog();
    void DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon);
    void BeamProfileHardwareChoiceChanged(bool cuve_enabled);
    void BraggPeakHardwareChoiceChanged(bool cuve_enabled);

private:
    void SetupConnections();
    void SetupLineValidators();
    void SetupDefaults();
    void SetupPushButtonGraphics();
    void TurnOffDbButton();
    void TurnOnDbButton();
    QString ButtonStyleSheet(QString image);

    Ui::MQA *ui_;
    std::shared_ptr<QSettings> settings_;
    std::unique_ptr<BeamProfilerGui> beam_profiler_gui_;
    std::unique_ptr<BraggPeakGui> bragg_peak_gui_;
    std::unique_ptr<Scanner3DGui> scanner3D_gui_;
    std::unique_ptr<TopDeJourGui> tdj_gui_;
    std::unique_ptr<PatientDebitGui> patient_debit_gui_;
    std::unique_ptr<BeamLineGui> beamline_gui_;
    std::shared_ptr<QARepo> qa_repo_;
    std::shared_ptr<PTRepo> pt_repo_;

    bool logged_in_;

    enum TabWidget { START, BEAMPROFILE, BRAGGPEAK, SCANNER3D, TOPDEJOUR, PATIENTDEBIT, BEAMLINE };
};


#endif
