#ifndef BEAMCHARACTERISTICS_MOCKMOTOR_H
#define BEAMCHARACTERISTICS_MOCKMOTOR_H

#include "IMotor.h"

#include <QDebug>

class MockMotor : public IMotor
{
    Q_OBJECT
    Q_INTERFACES(IMotor)

signals:
    void MovementStarted(double act_pos);
    void MovementFinished(double act_pos);
    void CurrentPosition(double act_pos);

public:
    MockMotor();
    ~MockMotor();
    void Configure();
    double ActPos();
    double RefPos();
    void SetDefaultSpeed();
    void SetFastSpeed();
    void SetPosDir() { qDebug() << "MockMotor::SetPosDir"; pos_dir_ = true; }
    void SetNegDir() { qDebug() << "MockMotor::SetNegDir"; pos_dir_ = false; }
    bool IsPosDir() { return pos_dir_; }
    void MoveDistance(double mm);
    void MoveToPosition(double mm);
    void MoveSingleStep();
    void FullStep(bool yes);
    void MoveToZero();
    void MoveToPreset();
    void Stop();
    void SetCurrentToZeroPos() { act_pos_ = 0; }
    void SetCurrentPos(double position) {};
    double StepSize() const { return 0.1; }
    bool IsZeroPos() const;

private:
    bool pos_dir_ = true;
    int act_pos_ = 0;

};

#endif
