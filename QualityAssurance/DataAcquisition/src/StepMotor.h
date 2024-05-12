#ifndef BEAMCHARACTERISTICS_STEPMOTOR_H
#define BEAMCHARACTERISTICS_STEPMOTOR_H

#include "IMotor.h"

#include <memory>
#include <QTimer>

#include "LabJack.h"
#include "Range.h"
#include "Clock.h"
#include "MotorConfig.h"

class StepMotor : public IMotor
{
    Q_OBJECT
    Q_INTERFACES(IMotor)

signals:

public:
    StepMotor(std::shared_ptr<LabJack> labjack, std::shared_ptr<Clock> clock, const MotorConfig& config, const Range& range);
    ~StepMotor();
    void Configure();
    void SetDefaultSpeed();
    void SetFastSpeed();
    double ActPos();
    double RefPos() { return ref_step_ * config_.DistancePerStep(); }
    void SetPosDir();
    void SetNegDir();
    bool IsPosDir();
    void MoveDistance(double distance);
    void MoveToPosition(double position);
    void MoveSingleStep();
    void FullStep(bool yes);
    void MoveToZero();
    void MoveToPreset();
    void Stop();
    void SetCurrentToZeroPos();
    void SetCurrentPos(double position);
    void SetToZeroPos(double pos);
    double StepSize() const { return config_.DistancePerStep(); }
    bool IsZeroPos() const;

private slots:

private:
    void DoSteps();
    void StepFinished();
    int SleepMSec(double pos) const;
    void CheckInRange(double pos);

    std::shared_ptr<LabJack> labjack_;
    std::shared_ptr<Clock> clock_;
    MotorConfig config_;
    Range range_;
    int ref_step_ = 0;
    int act_step_ = 0;
    bool configured_ = false;
    QTimer step_finished_;
    bool pos_dir_ = true;
    int steps_to_go_ = 0;
    bool fast_speed_ = false;
};

#endif
