#ifndef QUALITYASSURANCE_BRAGGPEAKER_H
#define QUALITYASSURANCE_BRAGGPEAKER_H

#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QDebug>

#include "Beacon.h"
#include "DevFactory.h"
#include "Range.h"
#include "IMotor.h"
#include "BeamSignal.h"
#include "Axis.h"
#include "AxisScan.h"
#include "BraggPeak.h"

class BraggPeaker : public QObject
{
    Q_OBJECT
public:    
    BraggPeaker(std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon, QA_HARDWARE hardware);
    ~BraggPeaker();
    bool IsIdle() const { return is_idle_; }

signals:
    void Measurement(MeasurementPoint m);
    void Finished();
    void MoveToStart();
    void XReady();
    void YReady();
    void ZReady();
    void StopStateMachine();
    void MovingToZero();
    void SIGNAL_ChambreVoltage(double value);
    void SIGNAL_DiodeVoltage(double value);

public slots:
    void Start(const Range& range, double step, int n_measurements, double max_std_err, bool fixed_nmb_measurements);

private slots:
    void StateMachineMessage(QString message) { qDebug() << "BraggPeaker " << " " << message; }
    void DoBraggPeak();
    void RepositionX();
    void RepositionY();
    void MoveToZero();
    void ReadSensorValues();

private:
    void SetupTimer();
    void ConnectSignals();
    void SetupStateMachine();
    void SetupAxisScanners();
    void AddMeasurement(BeamSignal signal);

    Point CurrentPosition() const;

    std::map<Axis, double> axis_position_;
    std::map<Axis, double> dpos_;
    std::map<Axis, std::shared_ptr<AxisScan>> axis_scanners_;
    std::shared_ptr<DevFactory> dev_factory_;
    Beacon* abort_beacon_;
    QStateMachine sm_;
    QTimer timer_;
    double step_ = 0.0;
    double x_pos_ = 0.0;
    double y_pos_ = 0.0;
    Range scan_range_;
    QA_HARDWARE hardware_;
    bool is_idle_ = true;
};


#endif
