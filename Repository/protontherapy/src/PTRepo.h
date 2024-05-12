#ifndef REPOSITORY_PTREPO_H
#define REPOSITORY_PTREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>
#include "TimedStampedDataSeries.h"

#include "XRayRecord.h"
#include "Collimator.h"
#include "Compensateur.h"
#include "Debit.h"
#include "Defaults.h"
#include "DosimetryRecord.h"
#include "Eye.h"
#include "Seance.h"
#include "Operator.h"
#include "Modulateur.h"
#include "Degradeur.h"
#include "Treatment.h"
#include "DepthDoseCurve.h"
#include "MCNPXData.h"
#include "BaliseRecord.h"
#include "BaliseLevel.h"
#include "BaliseCalibration.h"
#include "BaliseConfiguration.h"
#include "TreatmentType.h"

class PTRepo : public QObject {
  Q_OBJECT
public:
    PTRepo(const QString& conn_name,
                 const QString& hostname,
                 const int& port,
                 const QString& db_name,
                 const QString& username,
                 const QString& password);

    PTRepo(const QString& conn_name,
                 const QString& hostname,
                 const int& port,
                 const QString& db_name,
                 const QString& username,
                 const QString& password,
                 const int& disconnect_on_idle_timeout);
    ~PTRepo();

    bool Connect();
    void Disconnect();
    bool SubscribeToNotification(QString channel);
    bool UnsubscribeFromNotification(QString channel);

    QString GetOperatorPassword(const QString& username) const;
    QStringList GetOperators() const;

    bool DossierExist(const int& dossier) const;

    bool PatientExist(const int& patient_id) const;
    Patient GetPatient(const int& dossier) const;
    std::vector<Patient> GetPatients(const QString& firstName, const QString& lastName) const;
    int SavePatient(const Patient& patient);
    void UpdatePatient(const Patient& patient);
    Patient GetPatientById(const int& patient_id) const;

    void CreateTreatment(const int& dossier, const int& patient_id, const std::vector<double> &dose_prescribed,
                         const int& modulateur_id, const int& pri_degradeur_id, QString treatment_type);
    void CreateTreatment(const int& dossier, const int& patient_id, const std::vector<double> &dose_prescribed,
                         const int& modulateur_id, const int& pri_degradeur_id, const int& sec_degradeur_id, QString treatment_type);
    Treatment GetTreatment(const int& dossier) const;
    void UpdateTreatment(const Treatment& treatment);
    void UpdateTreatmentType(int dossier, QString type);
    void SetDossierComments(int dossier, QString comments);
    QString GetDossierComments(int dossier);
    std::vector<int> GetDossiers(QDate from_date, QDate to_date, QString dossier, QString first_name, QString last_name, QString treatment_type);
    std::vector<int> GetNonFinishedDossiers();
    int GetActiveTreatment() const;
    bool IsActiveTreatmentStarted() const;
    void SetActiveTreatment(int dossier);
    void SetActiveTreatmentStarted(bool started);

    std::vector<TreatmentType> GetTreatmentTypes() const;
    QString GetTreatmentType(int dossier) const;

    bool DosimetryExist(const int& dossier) const;
    Dosimetry GetMeasuredDosimetry(const int& dossier) const;
    Dosimetry GetLastMeasuredPatientDosimetry() const;
    Dosimetry GetMCDosimetry(const int& dossier);
    Dosimetry GetPatientDosimetry(const int& dossier) const;
    Dosimetry GetReferenceDosimetry(const QDateTime& timestamp) const;
    Dosimetry GetTopDeJourDosimetry(const QDate& date) const;
    Dosimetry GetDosimetry(const QDateTime& timestamp) const;
    int GetDossierForDosimetry(const QDateTime& timestamp) const;

    std::vector<Debit> GetDebits(const int& dossier, const QDate& from_date, const QDate& to_date) const;

    void SaveDosimetry(const int& dossier, const Dosimetry& dosimetry);
    void SaveMonteCarloDosimetry(const int& dossier, const double& debit);

    void DeleteSeance(const int& dossier, const int& seance_id);
    void CreateSeance(const int& dossier, const double& dose_prescribed);
    void AddSeanceToExistingTreatment(const int& dossier, const double& dose_prescribed);
    void SaveSeanceRecord(const int& seance_id, const SeanceRecord& record);
    std::vector<Seance> GetSeances(const int& dossier) const;
    Seance GetActiveSeance(int dossier) const;
    void UpdateEstimatedDose(int seance_record_id, double dose);
    void UpdateDeliveredDose(const QDateTime& seancerecord_timestamp, const int& um_del);
    void UpdatePlannedDose(const int& seance_id, const int& dossier, const double& dose_gy);
    SeanceRecord GetSeanceRecord(const QDateTime &timestamp) const;
    SeanceRecord GetSeanceRecord(const int &record_id) const;
    SeanceRecord GetLastSeanceRecord(int dossier) const;
    int GetSeanceRecordId(const SeanceRecord& record) const;
    int GetActiveSeanceNumber(int dossier) const;

    void SaveBaliseRecord(const BaliseRecord& record);
    BaliseRecord GetBaliseRecord(const SeanceRecord& record) const;
    std::vector<BaliseRecord> GetBaliseRecords(const QDate& from_date, const QDate& to_date) const;
    BaliseCalibration GetBaliseCalibration() const;
    void SaveBaliseCalibration(const BaliseCalibration& calib);
    BaliseConfiguration GetBaliseConfiguration() const;

    void SaveBaliseLevel(QString table, const BaliseLevel& level);
    void SaveBaliseInstantaneousLevel(const BaliseLevel& level);
    void SaveBaliseInstantaneousLevels(const std::vector<BaliseLevel>& levels);
    void SaveBaliseIntegratedLevel(const BaliseLevel& level);
    void SaveBaliseIntegratedLevels(const std::vector<BaliseLevel>& levels);
    void SaveBaliseBufferLevel(const BaliseLevel& level);
    void SaveBaliseBufferLevels(const std::vector<BaliseLevel>& levels);
    std::vector<BaliseLevel> GetBaliseLevels(QString table, const QDate& from_date, const QDate& to_date) const;
    std::vector<BaliseLevel> GetInstantaneousBaliseLevels(const QDate& from_date, const QDate& to_date) const;
    std::vector<BaliseLevel> GetIntegratedBaliseLevels(const QDate& from_date, const QDate& to_date) const;
    std::vector<BaliseLevel> GetBufferBaliseLevels(const QDate& from_date, const QDate& to_date) const;

    void SaveXRayRecord(const XRayRecord& record);
    std::vector<XRayRecord> GetXRayRecords(int dossier) const;
    std::vector<double> GetXRayPeakDoseRate(const QDate& from, const QDate& to) const;
    std::vector<double> GetXRayIntegratedCharge(const QDate& from, const QDate& to) const;
    std::pair<int, double> GetXRayExposure(int dossier) const;

    Chambre GetChambre(const QString& name) const;
    Chambre GetChambre(const int& id) const;
    QStringList GetChambres() const;

    void SaveModulateurItems(const Modulateur& modulateur, QString fraiseuse_program, bool update_existing);
    void UpdateModulateur(const Modulateur& modulateur);
    void SaveModulateur(const Modulateur& modulateur);
    Modulateur GetModulateurForDossier(const int &dossier) const;
    Modulateur GetModulateur(const int& modulateur_id) const;
    bool ModulateurExist(int modulateur_id) const;
    void ChangeModulateur(const int& dossier, const int& modulateur_id);
    QStringList GetAvailableModulateurs() const;
    std::vector<Modulateur> GetModulateurs(const double& min_mod100, const double& max_mod100,
                                           const double& min_mod98, const double& max_mod98,
                                           const double& min_parcours, const double& max_parcours);

    DegradeurSet GetDegradeurSetForDossier(const int& dossier) const;
    void ChangeDegradeurSet(int dossier, int pri_degradeur_id, int sec_degradeur_id);
    Degradeur GetDegradeur(const int& degradeur_id) const;    
    std::vector<Degradeur> GetAllAvailableDegradeurs() const;
    std::vector<Degradeur> GetAvailableDegradeurs(const int& max_um_plexi) const;
    std::vector<DegradeurSet> GetBestDegradeurCombinations(const int& des_um_plexi, const int& max_um_plexi_diff) const;

    void SaveCollimator(const Collimator& collimator);
    void SaveCollimateurItems(const Collimator &collimateur, const QString fraiseuse_program);
    Collimator GetCollimator(int dossier) const;

    void SaveCompensateur(const Compensateur& compensateur);
    void SaveCompensateurItems(const Compensateur& compensateur, const QString fraiseuse_program);
    Compensateur GetCompensateur(int dossier) const;

    QStringList GetFraiseuseTreatmentTypes() const;
    std::map<QDateTime, int> GetFraiseuseTreatmentPrograms(QString program_type) const;
    QString GetTreatmentFraiseuseProgram(int dossier, QString program_type) const;
    void SaveTreatmentFraiseuseProgram(int dossier, QString type, QDateTime timestamp, QString text);

    QStringList GetFraiseuseMiscTypes() const;
    std::map<QDateTime, QString> GetFraiseuseMiscPrograms(QString program_type) const;
    QString GetMiscFraiseuseProgram(QString id, QString program_type) const;
    void SaveMiscFraiseuseProgram(QString id, QString type, QDateTime timestamp, QString text);

    Defaults GetDefaults() const;

    MCNPXData GetMCNPXData() const;
    MCNPXData GetTheoreticalBraggData() const;

    TimedStampedDataSeries GetBeamTransmissionSeries(QDate from, QDate to) const;


    bool CheckConnection();
    bool CheckReadWriteAccess();


signals:
    void Notification(QString channel, QSqlDriver::NotificationSource, QVariant message);

    void SIGNAL_ConnectionClosed();
    void SIGNAL_ConnectionOpened();
    void SIGNAL_OpeningConnection();
    void SIGNAL_FailedOpeningConnection();

private:
    std::list<DosimetryRecord> GetDosimetryRecords(const int& dosimetry_id) const;
    void SaveDosimetrySummary(const int& dossier, const Dosimetry& dosimetry);
    void SaveDosimetryRecords(const int& dosimetry_id, const std::list<DosimetryRecord>& records);
    Seance GetSeance(int seance_id) const;
    std::list<SeanceRecord> GetSeanceRecords(const int& seance_id) const;
    bool TreatmentFinished(const int& dossier);

    QSqlDatabase *db_;
    QString conn_name_;
    QTimer disconnect_timer_;
    int disconnect_on_idle_timeout_ = 0;
};


#endif // PROTONTHERAPIE_PTRepo_H
