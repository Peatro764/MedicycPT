#ifndef QA_PATIENTDEBITGUI_H
#define QA_PATIENTDEBITGUI_H

#include <QWidget>
#include <QSettings>
#include <QtDBus/QDBusConnection>

#include "GuiBase.h"
#include "Dosimetry.h"
#include "PTRepo.h"
#include "QARepo.h"
#include "DosimetryRecord.h"
#include "SeanceConfig.h"
#include "NJ1.h"
#include "radiationmonitor_interface.h"
#include "DevFactory.h"

class MTD;

class PatientDebitGui : public GuiBase
{
    Q_OBJECT
public:
    PatientDebitGui(MQA *parent, std::shared_ptr<PTRepo> pt_repo, std::shared_ptr<QARepo> qa_repo);
    ~PatientDebitGui();

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
    void TurnOnConnectedButton();
    void TurnOffConnectedButton();

signals:
    void SIGNAL_ICChargeGiven();
    void SIGNAL_SeanceStarted();
    void SIGNAL_SeanceFinished();
    void SIGNAL_SeanceTimeout();
    void SIGNAL_Abort();
    void SIGNAL_AxisDeviation(Axis axis, double value);

private slots:
    void ChoosePatient();
    void UpdateResults();
    void UpdateDeliveryConfig();
    void UploadDeliveryConfig();
    void GetICCharge();
    void SaveAirICCharge(double charge);
    void ClearAirICCharge();
    void LaunchManualMotorControlDialog(Axis axis, std::shared_ptr<IMotor> motor);
    void MoveToZHalf();

private:
    void SetupDBus();
    void LoadPatient(int dossier);
    void FillSavedDosimetryTable();
    void InitProgressBar();
    int GetDossier() const;
    bool DossierIsLoaded() const;
    void InitTables();
    void ClearData();
    void InitChambres();
    void RemoveSelectorChambre();
    void ConnectSignals();
    void SetupLineEditValidators();
    double GetEstimatedDebit(int dossier) const;
    QString AdjustForTop(QDateTime timestamp, double debit) const;
    void WriteDeliveryParameters();
    SeanceConfig GetSeanceConfig(Treatment& treatment) const;
    void SetupStateMachine();
    void PrintStateChanges(QAbstractState *state, QString name);
    bool VerifyAxesCalibrations();
    void VerifyAxisCalibration(Axis axis);

    std::unique_ptr<QSettings> settings_;

    std::shared_ptr<PTRepo> pt_repo_;
    std::shared_ptr<QARepo> qa_repo_;
    Dosimetry dosimetry_;
    SeanceConfig seance_config_;
    std::vector<SeancePacket> seance_packets_;
    nj1::NJ1 nj1_;
    medicyc::cyclotron::RadiationMonitorInterface* radiationmonitor_interface_;
    QStateMachine sm_;
    double ic_charge_ = 0.0;
    double air_ic_charge_ = 0.0;
    QString chambre_list_default_text_ = "Sélectionner";

    enum class CURRENT_GRIDCOLS : char { UM = 0, ICHAMBRE1 = 1, ICHAMBRE2 = 2, IC_CHARGE = 3, AIR_IC_CHARGE = 4, DEBIT = 5, DMOYEN = 6 };
    enum class SAVED_GRIDCOLS : char { TIMESTAMP = 0, TYPE = 1, DEBIT = 2 };

    std::shared_ptr<DevFactory> dev_factory_;
    QA_HARDWARE hardware_ = QA_HARDWARE::SCANNER3D;
};

#endif
