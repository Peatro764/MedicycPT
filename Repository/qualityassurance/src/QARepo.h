#ifndef REPOSITORY_QAREPO_H
#define REPOSITORY_QAREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>

#include "SensorConfig.h"
#include "MotorConfig.h"
#include "ClockConfig.h"
#include "BeamProfile.h"
#include "MeasurementPoint.h"
#include "BeamProfileSeries.h"
#include "BeamProfileResults.h"
#include "BraggPeak.h"
#include "BraggPeakSeries.h"
#include "SOBPSeries.h"
#include "CuveCube.h"

class ModAcc {
private:
    int dossier_;
    int modulator_id_;
    double rs_mm_plexi_;
    QString comment_;
public:
    int dossier() const { return dossier_; }
    int modulator_id() const { return modulator_id_; }
    double rs_mm_plexi() const { return rs_mm_plexi_; }
    QString comment() const { return comment_; }
    ModAcc(int dossier, int modulator_id, double rs_mm_plexi, QString comment) :
        dossier_(dossier),
        modulator_id_(modulator_id),
        rs_mm_plexi_(rs_mm_plexi),
        comment_(comment) {}
};

class QARepo : public QObject {
  Q_OBJECT
public:
    QARepo(const QString& conn_name,
                 const QString& hostname,
                 const int& port,
                 const QString& db_name,
                 const QString& username,
                 const QString& password);
    QARepo(const QString& conn_name,
                 const QString& hostname,
                 const int& port,
                 const QString& db_name,
                 const QString& username,
                 const QString& password,
                 const int& disconnect_on_idle_timeout);
    ~QARepo();

    bool Connect();
    void Disconnect();

    SensorConfig GetSensorConfig(Axis axis) const;
    SensorConfig GetSensorConfig(SensorType type) const;
    MotorConfig GetMotorConfig(Axis axis, QA_HARDWARE hardware) const;
    ClockConfig GetClockConfig(QA_HARDWARE) const;

    void UpdateBeamProfile(const BeamProfile& profile);
    void SaveBeamProfile(const BeamProfile& profile);
    BeamProfile GetBeamProfile(Axis axis, const QDateTime& timestamp) const;
    BeamProfile GetLastBeamProfile(Axis axis) const;
    std::vector<BeamProfile> GetBeamProfiles(Axis axis, const QDate& from, const QDate& to) const;
    BeamProfileSeries GetBeamProfileSeries(Axis axis, QDate from, QDate to) const;

    void SaveBraggPeak(const BraggPeak& braggpeak);
    BraggPeak GetBraggPeak(const QDateTime& timestamp) const;
    BraggPeak GetLastBraggPeak() const;
    std::vector<BraggPeak> GetBraggPeaks(QDate from, QDate to) const;
    BraggPeakSeries GetBraggPeakSeries(QDate from, QDate to) const;

    void SaveSOBP(const SOBP& sobp);
    SOBP GetSOBP(const QDateTime& timestamp) const;
    SOBP GetLastSOBP() const;
    std::vector<SOBP> GetSOBPs() const;
    std::vector<int> GetSOBPModulateurs() const;
    std::map<QDateTime, ModAcc> GetSOBPSForModulateur(int modulateur_id) const;
    std::map<QDateTime, ModAcc> GetSOBPSForModulateur(int modulateur_id, QDate from, QDate to) const;
    SOBP GetSOBPForDossier(int dossier) const;
    SOBPSeries GetSOBPSeries(int modulator, QDate from, QDate to) const;

    void SaveCube(const CuveCube& cube);
    CuveCube GetCube(const QDateTime& timestamp) const;
    std::map<QDateTime, QString> GetCubeKeys() const;

    bool CheckConnection();

signals:
    void SIGNAL_ConnectionClosed();
    void SIGNAL_ConnectionOpened();
    void SIGNAL_OpeningConnection();
    void SIGNAL_FailedOpeningConnection();

private:
    SensorConfig GetSensorConfig(QString dev_name) const;

    void SaveMeasurementPoints(QString measurement_type, int measurement_id, const std::vector<MeasurementPoint>& points);
    std::vector<MeasurementPoint> GetMeasurementPoints(QString measurement_type, int measurement_id) const;

    MeasurementCurrents GetMeasurementCurrents(QString measurement_type, int measurement_id) const;
    void SaveMeasurementCurrents(QString measurement_type, int measurement_id, const MeasurementCurrents& currents);
    std::vector<double> DecodeDoubleArray(QString array) const;  // TODO make common dbutils with ptrepo

    QSqlDatabase *db_;
    QString conn_name_;
    QTimer disconnect_timer_;
    int disconnect_on_idle_timeout_ = 0;
};

#endif
