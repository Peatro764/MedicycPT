#include <QtTest/QtTest>

#include "PTRepo.h"

class TestPTRepo : public QObject {
    Q_OBJECT
private slots:
    // functions executed by QtTest before and after test suite
    void initTestCase();
    void cleanupTestCase();

    // functions executed by QtTest before and after each test
    void init();
    void cleanup();

    // tests
    void DossierExist();

    void GetChambreUsingId();
    void GetChambreUsingNameReturnsLast();

    void PatientExist();

    void GetPatient();
    void GetPatients();
    void SavePatient();

    void GetDosimetryByTimestamp();
    void DosimetryIsRetrievedWithCorrectChambre();
    void SavedAndRetrievedPatientDosimetryAreEqual();
    void SavedAndRetrievedPatientDosimetryPrecision();
    void LastPatientDosimetryInDossierIsReturned();
    void LastReferenceBeforePatientDosimetryIsReturned();
    void FirstReferenceAfterPatientDosimetryIsReturned();
    void LastTopDeJourDosimetryOnDayIsReturned();
    void LastTopDeJourDosimetryOnDayIsReturnedShuffled();
    void GetDebitsSinceDate();
    void GetLastMeasuredDosimetry();
    void GetDossierForDosimetry();
    void GetMeasuredDosimetry();
    void GetMCDosimetry();
    void SaveMonteCarloDosimetry();
    void SaveMonteCarloDosimetry_ErrorCases();

    void CreateAndRetrieveSingleEmptySeance();
    void CreateAndRetrieveSeanceCheckPrecision();
    void CreateAndRetrieveSingleNonEmptySeanceMultipleValuesInRecords();
    void CreateAndRetrieveSingleNonEmptySeance();
    void CreateAndRetrieveMultipleNonEmptySeances();
    void SeancesForCorrectDossierIsReturned();
    void GetActiveSeanceWhenTreatmentNotStarted();
    void GetActiveSeanceWhenTreatmentStarted();
    void GetActiveSeanceWhenTreatmentFinished();
    void GetActiveSeanceErrorCases();
    void GetSeanceRecord();
    void GetSeanceRecordById();
    void GetLastSeanceRecord_OneRecord();
    void GetLastSeanceRecord_MultipleRecords();
    void GetLastSeanceRecord_ErrorCases();
    void GetSeanceRecordId();
    void UpdateDeliveredDose();
    void UpdatePlannedDose();
    void UpdateEstimatedDose();
    void DeleteSeance();

    void SetAndGetDossierComments();

    void SetAndGetActiveTreatment();
    void SetAndGetActiveTreatment_ErrorCases();
    void SetAndGetActiveTreatmentStarted();

    void SaveAndGetBaliseLevels();
    void SaveAndGetBaliseLevels_ErrorCases();

    void SaveAndGetBaliseRecord();
    void SaveAndGetBaliseRecord_ErrorCases();
    void GetBaliseRecords();
    void SaveAndGetBaliseCalibration();
    void GetBaliseConfiguration();

    void SaveAndGetXRayRecords();

    void CreateTreatmentSuccessCases();
    void CreateTreatmentNonValidDegradeur();
    void CreateTreatmentRollbackWhenFails();
    void CreateTreatmentThatAlreadyExistsFails();
    void CreateAndRetrieveTreatmentAllDosimetriesCorrectlyDone();
    void CreateAndRetrieveTreatmentCorrectReferenceTaken();
    void CreateAndRetrieveTreatmentSomeDosimetriesDone();
    void UpdateTreatment();
    void UpdateTreatmentType();
    void GetTreatmentType();

    void AddSeanceToExistingTreatment();

    void InsertAndRetrieveDoubles();

    void GetDegradeurSet();
    void ChangeDegradeurSet();
    void GetBestDegradeurCombinations();

    void GetModulateursFullfillingCriteria();
    void SaveModulateur();
    void UpdateModulateur();
    void ModulateurExist();
    void SaveModulateurItems();
    void GetModulateurById();
    void GetModulateurByDossier();
    void ChangeModulateur();

    void SaveAndGetCollimator();

    void SaveAndGetTreatmentFraiseuseProgram();
    void GetFraiseuseTreatmentPrograms();
    void SaveAndGetMiscFraiseuseProgram();
    void GetFraiseuseMiscPrograms();

    void SaveAndGetCompensateur();
    void CreateAndRetrieveMCNPXData();
    void GetTreatmentTypes();

    void CheckReadWriteAccess();
    void GetNonFinishedDossiers();

    void GetBeamTransmission();

 private:
    void AbortUnitTests(QString msg);
    bool LoadSqlResourceFile(QString file);
    bool InsertDegradeur(int id, int um_plexi, bool available);
    bool InsertModulateur(int id, double mod100, double mod98, double parcours, int degradeur, int thickness, double weight);
    QString dbHostName_;
    int dbPort_;
    QString dbDatabaseName_;
    QString dbUserName_;
    QString dbPassword_;
    QString dbConnName_;
};
