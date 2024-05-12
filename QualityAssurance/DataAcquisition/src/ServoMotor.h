#ifndef BEAMCHARACTERISTICS_SERVOMOTOR_H
#define BEAMCHARACTERISTICS_SERVOMOTOR_H

#include <memory>

#include "StepMotor.h"
#include "Sensor.h"
#include "Range.h"

class ServoMotor : public IMotor
{
    Q_OBJECT
    Q_INTERFACES(IMotor)

signals:

private slots:
    void DoMovement();

public:
    ServoMotor(std::shared_ptr<IMotor> step_motor, std::shared_ptr<ISensor> position_sensor, const Range& range);
    ~ServoMotor();
    void Configure();
    void SetDefaultSpeed();
    void SetFastSpeed();
    double ActPos();
    double RefPos();
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
    double StepSize() const { return step_motor_->StepSize(); }
    void CheckThatAxisIsMoving(double act_pos, double old_pos);
    bool IsZeroPos() const;

private:
    std::shared_ptr<IMotor> step_motor_;
    std::shared_ptr<ISensor> position_sensor_;
    double target_position_ = 0.0;
    Range range_;
    double prev_pos_ = -9999.0;
    int n_no_movement_steps_ = 0;
    bool enable_servo_loop_ = false;
};

#endif
