#include <QtTest/QtTest>

class TestTreatmentType : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void Comparisons();
    void Constructor();
    void SeanceDoses();
    void Dose();
    void DoseEBR();

    void AddSeance();
    void AddBoost();
    void StandardSummary();
    void BoostSummary();
    void TotalSummary();
};


