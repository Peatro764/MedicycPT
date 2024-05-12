#include <QtTest/QtTest>

#include "SeanceRecord.h"

class TestSeance : public QObject {
    Q_OBJECT
private:
    SeanceRecord GetSeanceRecord(const int& um_del, const double& debit, double dose_est);
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Comparisons();
    void Getters();
    void AddSeanceRecords();
    void GetDoseDelivered();
    void GetDoseEstimated();
};


