#ifndef BEAMCHARACTERISTICS_IMOTOR_H
#define BEAMCHARACTERISTICS_IMOTOR_H

#include <QObject>

class IMotor : public QObject
{
    Q_OBJECT

signals:
    void MovementStarted(double act_pos);
    void MovementFinished(double act_pos);
    void MovementAborted(QString msg);
    void CurrentPosition(double act_pos);

public:
    virtual ~IMotor() {}
    virtual void Configure() = 0;
    virtual void SetDefaultSpeed() = 0;
    virtual void SetFastSpeed() = 0;
    virtual double ActPos() = 0;
    virtual double RefPos() = 0;
    virtual void SetPosDir() = 0;
    virtual void SetNegDir() = 0;
    virtual bool IsPosDir() = 0;
    virtual void MoveDistance(double distance) = 0;
    virtual void MoveToPosition(double position) = 0;
    virtual void MoveSingleStep() = 0;
    virtual void MoveToZero() = 0;
    virtual void FullStep(bool yes) = 0;
    virtual void MoveToPreset() = 0;
    virtual void Stop() = 0;
    virtual void SetCurrentToZeroPos() = 0;
    virtual void SetCurrentPos(double position) = 0;
    virtual void SetToZeroPos(double pos) = 0;
    virtual double StepSize() const = 0;
    virtual bool IsZeroPos() const = 0;
private:

};

Q_DECLARE_INTERFACE(IMotor, "IMotor")

#endif
