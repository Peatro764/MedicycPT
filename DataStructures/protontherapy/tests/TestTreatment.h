#include <QtTest/QtTest>

#include "SeanceRecord.h"

class TestTreatment : public QObject {
    Q_OBJECT
private:
    SeanceRecord GetSeanceRecord(const int& um_del, const double& debit, const double& est_dose);
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Constructors();
    void GetActiveSeanceNumber();
    void GetActiveSeance();
    void GetSeanceByIndex();
    void GetRemainingDoseActiveSeance();
    void GetDesiredDoseActiveSeance();
    void GetPastSeances();
    void GetFutureSeances();
    void BeamCalibrationDone();
    void GetBeamCalibrationMeasuredPatientDosimetry();
    void GetBeamCalibrationMonteCarloPatientDosimetry();
    void GetUMPrevuActiveSeance();
    void GetPrescribedDose();
    void GetDeliveredDose();
    void GetEstimatedDeliveredDose();
    void GetSeanceConfig();
    void CloseDossier();
};


