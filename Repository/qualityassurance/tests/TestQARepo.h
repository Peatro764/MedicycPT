#include <QtTest/QtTest>

#include "QARepo.h"

class TestQARepo : public QObject {
    Q_OBJECT
private slots:
    // functions executed by QtTest before and after test suite
    void initTestCase();
    void cleanupTestCase();

    // functions executed by QtTest before and after each test
    void init();
    void cleanup();

    void SaveAndGetBeamProfile();
    void GetBeamProfileSeries();
    void GetLastBeamProfile();

    void GetLastBraggPeak();
    void SaveAndGetBraggPeak();
    void GetBraggPeakSeries();
    void GetSOBPSeries();

    void SaveAndGetSOBP();
    void SaveAndGetCube();
    void SaveAndGetCubeKeys();

 private:
    void AbortUnitTests(QString msg);
    bool LoadSqlResourceFile(QString file);
    std::vector<MeasurementPoint> GetExampleProfile();
    BeamProfile GetBeamProfile(Axis axis, QDateTime timestamp, int smoothing, double noise);
    BraggPeak GetBraggPeak(QDateTime timestamp);
    SOBP GetSOBP(int dossier, QDateTime timestamp);
    std::vector<MeasurementPoint> GetMeasurementPoints();

    QString dbHostName_;
    QString dbDatabaseName_;
    int dbPort_;
    QString dbUserName_;
    QString dbPassword_;
    QString dbConnName_;
};
