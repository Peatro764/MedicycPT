#ifndef TopDeJourGui_H
#define TopDeJourGui_H

#include <QWidget>
#include <QStateMachine>
#include <QSettings>
#include <QtDBus/QDBusConnection>

#include "DevFactory.h"
#include "GuiBase.h"
#include "DebitGraph.h"
#include "Dosimetry.h"
#include "PTRepo.h"
#include "QARepo.h"
#include "DosimetryRecord.h"
#include "SeanceConfig.h"
#include "NJ1.h"

#include "radiationmonitor_interface.h"

class MTD;

class TopDeJourGui : public GuiBase
{
    Q_OBJECT
public:
    TopDeJourGui(MQA *parent, std::shared_ptr<PTRepo> pt_repo, std::shared_ptr<QARepo> qa_repo);
    ~TopDeJourGui();

signals:
    void SIGNAL_ICChargeGiven();
    void SIGNAL_UpdateResultsDone();
    void SIGNAL_SeanceStarted();
    void SIGNAL_SeanceFinished();
    void SIGNAL_SeanceTimeout();
    void SIGNAL_Abort();
    void SIGNAL_AxisDeviation(Axis axis, double value);

public slots:
    void TemperatureChanged();
    void PressureChanged();
    void ChambreChanged(const QString& chambre);

    void WriteResults();
    void RemoveAllMeasurements();
    void RemoveMeasurement(int row);
    void SaveCurrentDosimetry();
    void SaveDosimetry(Dosimetry& dosimetry);
    void LaunchDosimetrySettingsDialog();
    void CheckForUnSavedChanges();

    void ConfigureLabJack();
    void Configure() override;
    void CleanUp() override;

    void SeanceStartedReceived(const SeancePacket& packet);
    void SeancePacketReceived(const SeancePacket& packet);
    void SeanceFinishedReceived(const SeancePacket& packet);
    void SeanceTimeoutReceived();

private slots:
    void FillAcquisitionPageGraphs();
    void GetICCharge();
    void SaveAirICCharge(double charge);
    void ClearAirICCharge();
    void UpdateResults();
    void UpdateDeliveryConfig();
    void UploadDeliveryConfig();
    void TabWidgetPageChanged(int index);
    void LaunchManualMotorControlDialog(Axis axis, std::shared_ptr<IMotor> motor);
    void MoveToZHalf();
    void TurnOnConnectedButton();
    void TurnOffConnectedButton();

private:
    void SetupDBus();
    void InitTables();
    void ClearData();
    void InitDateSelectors();
    void SetupGraphs();
    void InitChambres();
    void RemoveSelectorChambre();
    void ConnectSignals();
    void SetupLineEditValidators();
    void SetDefaultMeasurementConfig();
    void WriteDeliveryParameters();
    void SetupStateMachine();
    void PrintStateChanges(QAbstractState *state, QString name);
    bool VerifyAxesCalibrations();
    void VerifyAxisCalibration(Axis axis);

    std::unique_ptr<QSettings> settings_;

    std::shared_ptr<PTRepo> pt_repo_;
    std::shared_ptr<QARepo> qa_repo_;
    Dosimetry dosimetry_;
    std::shared_ptr<DebitGraph> debit_graph_;

    enum class TABWIDGETPAGE : int { ACQUISITION = 0, AMBIENTCHARGE = 1 };
    enum class CURRENT_GRIDCOLS : char { UM = 0, ICHAMBRE1 = 1, TRANSMISSION = 2, IC_CHARGE = 3, AIR_IC_CHARGE = 4, DEBIT = 5, DMOYEN = 6 };
    enum class HISTORICAL_GRIDCOLS : char { DATE = 0, CHAMBRE = 1, DEBIT = 2 };
    const int dossier_number_ = 10000;
    SeanceConfig seance_config_;
    nj1::NJ1 nj1_;
    medicyc::cyclotron::RadiationMonitorInterface* radiationmonitor_interface_;
    std::vector<SeancePacket> seance_packets_;
    QStateMachine sm_;
    double ic_charge_ = 0.0;
    double air_ic_charge_ = 0.0;
    QString chambre_list_default_text_ = "Sélectionner";
    std::shared_ptr<DevFactory> dev_factory_;
    QA_HARDWARE hardware_ = QA_HARDWARE::SCANNER3D;
};

#endif
