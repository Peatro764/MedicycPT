#ifndef QUALITYASSURANCE_BEAMPROFILER_H
#define QUALITYASSURANCE_BEAMPROFILER_H

#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QDebug>

#include "Beacon.h"
#include "DevFactory.h"
#include "Range.h"
#include "IMotor.h"
#include "BeamSignal.h"
#include "AxisScan.h"
#include "MeasurementPoint.h"

#include "BeamProfile.h"

class BeamProfiler : public QObject
{
    Q_OBJECT
public:    
    BeamProfiler(std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon, QA_HARDWARE hardware);
    ~BeamProfiler();
    Axis GetActiveAxis() const { return active_axis_; }
    bool IsIdle() const { return is_idle_; }
    bool IsFinished() const { return is_finished_; }

signals:
    void SIGNAL_ChambreVoltage(double value);
    void SIGNAL_DiodeVoltage(double value);
    void Measurement(Axis axis, MeasurementPoint p);
    void AxisIdle(Axis axis);
    void AxisFinished(Axis axis);
    void ScanFinished(Axis axis);
    void AllAxesReady();
    void ZAxisReady();
    void PassiveAxisReady();
    void ActiveAxisReady();
    void MoveToStart();
    void MovingToZero();
    void Acknowledged();
    void StopStateMachine();

public slots:
    void Start(Axis axis, const Range& range, double step, int n_measurements, double max_std_err, bool fixed_nmb_measurements);

private slots:
    void StateMachineMessage(QString message) { qDebug() << QDateTime::currentDateTime().toString() << " BeamProfiler " << " " << message; }
    void DoProfile();
    void RepositionZAxis();
    void RepositionActiveAxis();
    void MoveToZero(Axis axis);
    void ReadSensorValues();

private:
    void SetupTimer();
    void ConnectSignals();
    void SetupStateMachine();
    void SetupAxisScanners();
    void AddMeasurement(Axis axis, BeamSignal signal);
    Point CurrentPosition() const;

    Axis GetPassiveAxis(Axis active_axis);
    void CheckAxisReady(Axis axis, bool yes);

    Axis active_axis_ = Axis::X;
    Axis passive_axis_ = Axis::Y;
    std::map<Axis, double> axis_position_;
    std::map<Axis, double> dpos_;
    std::map<Axis, std::shared_ptr<AxisScan>> axis_scanners_;
    std::shared_ptr<DevFactory> dev_factory_;
    Beacon* abort_beacon_;
    QStateMachine sm_;
    QTimer timer_;
    double step_ = 0.0;
    Range scanning_range_;
    int n_measurements_ = 5;
    double max_std_err_ = 100;
    bool fixed_nmb_measurements_ = false;
    QA_HARDWARE hardware_ = QA_HARDWARE::UNK;
    double z_pos_ = 0.0;
    bool is_idle_ = true;
    bool is_finished_ = true;
};


#endif
