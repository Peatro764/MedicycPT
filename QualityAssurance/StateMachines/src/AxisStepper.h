#ifndef QUALITYASSURANCE_AXISSTEPPER_H
#define QUALITYASSURANCE_AXISSTEPPER_H

#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QDebug>

#include "Beacon.h"
#include "DevFactory.h"
#include "Range.h"
#include "IMotor.h"
#include "BeamSignal.h"

class AxisStepper : public QObject
{
    Q_OBJECT
public:    
    AxisStepper(Axis axis, std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon);
    ~AxisStepper();

signals:
    void Ready(bool yes);
    void CurrentPosition(double pos);
    void Finished();

    void MoveToStart();
    void MoveStep();
    void MovementFinished(double act_pos, double des_pos);

public slots:
    void Init(double start, double end, double step);
    void InvertDirection();

private slots:
    void StateMachineMessage(QString message) { qDebug() << "AxisStepper Axis " << AxisToString(axis_) << " " << message; }
    void MoveStepSlot();
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

    Axis axis_ = Axis::UNK;
    std::shared_ptr<DevFactory> dev_factory_;
    Beacon* abort_beacon_;
    QStateMachine sm_;
    std::shared_ptr<IMotor> motor_;
    QTimer timer_;
    double des_pos_ = 0.0;
};


#endif
