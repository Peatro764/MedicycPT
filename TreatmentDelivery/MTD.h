#ifndef TREATMENTDELIVERY_MTD_H
#define TREATMENTDELIVERY_MTD_H

#include <memory>

#include <QMainWindow>
#include <QStateMachine>
#include <QThread>
#include <QtDBus/QDBusConnection>

#include "ui_MTD.h"

#include "ConfigDialog.h"
#include "DossierSelectionDialog.h"
#include "PTRepo.h"

#include "PasswordConfirmation.h"
#include "SeanceProgressGraph.h"
#include "SeanceDoseGraph.h"
#include "SeanceConfig.h"
#include "NJ1.h"

#include "radiationmonitor_interface.h"

class MTD : public QMainWindow
{
    Q_OBJECT

public:

    MTD(QWidget *parent = 0);
    ~MTD();

public slots:

private slots:
    bool UserConfirmationIncrementSeance();
    bool AdminConfirmationIncrementSeance();
    void UserClickIncrementSeance();
    void DoIncrementSeance();
    void SetupDb();
    void TurnOnDbButton();
    void TurnOffDbButton();
    void TurnOnNJ1Button();
    void TurnOffNJ1Button();
    void ShowConfiguration();

    void UploadDeliveryConfig();
    void UpdateDeliveryConfig();
    void LoadPatient();
    void ShowCollimator();
    void ShowCompensator();
    void ShowSeanceInfo(int seance_idx);
    void UpdateResults();
    void CalculateEstimatedDose(double ambient_charge);
    void SaveMRMInfo(const SeanceRecord& seance_record);
    void ClearMRMMeasurement();
    void FailedConnectingToDatabase();
    void StateMachineMessage(QString message);
    void SeanceStartedReceived(const SeancePacket& packet);
    void SeancePacketReceived(const SeancePacket& packet);
    void SeanceFinishedReceived(const SeancePacket& packet);
    void CloseDossier();

    void DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon);

signals:
    void SIGNAL_UpdateResultsDone();
    void SIGNAL_SeanceStarted();
    void SIGNAL_SeanceFinished();
    void SIGNAL_Abort();

private:
    void SetupDBus();
    void PrintStateChanges(QState *state, QString name);
    void AskIfDossierShouldBeClosed();
    void ClearPatient();
    void InitButtons();
    void FillPatientInfo();
    void SetupGraphs();
    void ConnectSignals();
    bool DossierLoaded() const;
    void FillModulatingInfo();
    void WriteBeamCalibration();
    void WriteDeliveryParameters();
    void AddDataPointToProgressGraph();
    QString ButtonStyleSheet(QString image, int radius);
    void DisplayWaitProgressBar(QString message, int msec);
    void SetupInternalGroupBox();

    void ConfigureUMSeanceGauge();
    void ConfigureDoseSeanceGauge();
    void ConfigureDoseTotalGauge();
    void UpdateUMCounters();

    void SetupStateMachine();
    void CheckDbConnection(QString message);
    uint32_t GetUM1();
    uint32_t GetUM2();


    // Graphs
    Ui::MTD *ui_;
    std::unique_ptr<SeanceDoseGraph> seance_dose_graph_;
    std::unique_ptr<SeanceProgressGraph> seance_progress_graph_;

    std::unique_ptr<QSettings> settings_;

    // Delivery data
    Treatment treatment_;
    SeanceConfig seance_config_;
    std::vector<SeancePacket> seance_packets_;
    double mrm_measurement_ok_ = false;
    double mrm_estimated_dose_ = 0.0;
    double mrm_integrated_charge_ = 0.0;

    // Communication
    PTRepo *repo_;
    medicyc::cyclotron::RadiationMonitorInterface* radiationmonitor_interface_;
    QString dbus_name_;
    bool io_error_active_ = false;

    // misc
    QStateMachine sm_;
    nj1::NJ1 nj1_;

    int n_packets_ = 0;
};

#endif
