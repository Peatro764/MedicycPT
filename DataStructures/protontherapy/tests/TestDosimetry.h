#include <QtTest/QtTest>

#include "Dosimetry.h"

class TestDosimetry : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    Chambre GetChambre();

    void Constructors();
    void Setters();
    void Comparisons();
    void AddRecord();
    void DeleteSingleRecord();
    void DeleteRecords();
    void GetFtp();
    void GetDebit();
    void GetDebitMean();
    void GetDebitStdDev();
    void GetDebits();
    void GetRollingDebitMeans();
    void GetRollingDebitStdDevs();
    void GetExpectedCharge();
    void MeasurementConditionsNotGiven();
private:
    double maxRelDiff;

    std::vector<double> debit1_;
    std::vector<double> debit2_;
    std::vector<double> debit3_;

    Dosimetry* dosimetry1_;
    Dosimetry* dosimetry2_;
    Dosimetry* dosimetry3_;
};


