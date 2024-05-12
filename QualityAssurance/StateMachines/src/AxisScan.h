#ifndef QUALITYASSURANCE_AXISSCAN_H
#define QUALITYASSURANCE_AXISSCAN_H

#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QDebug>

#include "Beacon.h"
#include "DevFactory.h"
#include "Range.h"
#include "IMotor.h"
#include "BeamSignal.h"

class AxisScan : public QObject
{
    Q_OBJECT
public:    
    AxisScan(Axis axis, std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon);
    ~AxisScan();

signals:
    void Ready(bool yes);
    void Measurement(double pos, BeamSignal signal);
    void MeasurementAll(double pos, std::vector<double> values);
    void Finished();
    void CurrentPosition(double act_pos);

    void MoveToStart();
    void MovementFinished(double act_pos, double des_pos);
    void Start();

public slots:
    void Init(double start, double end, double step, int n_measurements, double max_std_err, bool fixed_nmb_measurements);
    void InvertDirection();

private slots:
    void StateMachineMessage(QString message) { qDebug() << "AxisScan Axis " << AxisToString(axis_) << " " << message; }
    void DoMeasurement();
    void MoveStep();
    void Stop();

private:
    void SetupTimer();
    void ConnectSignals();
    void SetupStateMachine();
    bool AtEndOfRange(double pos);

    std::shared_ptr<IMotor> motor();

    double start_ = 0.0;
    double end_ = 0.0;
    double step_ = 0.0;
    int n_measurements_ = 5;
    double max_std_err_ = 50.0;
    bool fixed_nmb_measurements_ = false;

    Axis axis_ = Axis::UNK;
    std::shared_ptr<DevFactory> dev_factory_;
    Beacon* abort_beacon_;
    QStateMachine sm_;
    std::shared_ptr<IMotor> motor_;
    QTimer timer_;
    double pos_ = 0.0;
    double des_pos_ = 0.0;
};


#endif
