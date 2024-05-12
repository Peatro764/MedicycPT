#ifndef TREATMENTTAB_H
#define TREATMENTTAB_H

#include <QWidget>

#include "PTRepo.h"
#include "IO.h"
#include "Treatment.h"
#include "TotalDoseGraph.h"
#include "SeanceDoseGraph.h"
#include "SeanceProgressGraph.h"

class MTD;

class TreatmentTab : public QObject
{
    Q_OBJECT
public:
    TreatmentTab(int dossierNumber, MTD *parent);
    ~TreatmentTab();

public slots:
    void UMChanged();
    void DoTreatment();
    void LaunchBeamCalibrationDialog();
    void UpdateDurationLabel();
    void ShowSeanceInfo(int seance_idx);
    void ShowCollimateur();
    void ShowCompensateur();

private:
    void ConnectSignals();
    void SetupLineEditValidators();
    bool QueryUserIfSeanceFinished(int um_pre, int um_del);
    void IncrementActiveSeance();
    void WriteConfig();
    void WriteCurrentSeance();
    void WriteBeamCalibration();
    void WriteBeamShaping();
    void WriteResults();
    void WriteDefaults();
    bool GetDeltaUM(int *delta_um);
    double GetEstimatedStripperCurrent();
    void NotifyDbTreatmentStarted();
    void NotifyDbTreatmentCancelled();

    int dossierNumber_;
    MTD* parent_;
    Treatment treatment_;
    TotalDoseGraph total_dose_graph_;
    SeanceDoseGraph seance_dose_graph_;
    SeanceProgressGraph seance_progress_graph_;
};

#endif // DOSIMETRYTAB_H
