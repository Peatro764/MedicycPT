#include <QtTest/QtTest>

#include "SeanceRecord.h"

class TestSeanceRecord : public QObject {
    Q_OBJECT
private:
    SeanceRecord GetSeanceRecord(const int& um_del, const double& debit);
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Equality();
    void Constructor();
    void GetDoseDelivered();
    void GetTotalUMDelivered();
    void GetTotalDuration();
    void PartOf();
    void Subtract();
};


