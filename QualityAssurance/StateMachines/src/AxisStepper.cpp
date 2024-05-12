#include "AxisStepper.h"

#include "Axis.h"
#include "ServoMotor.h"
#include "StepMotor.h"

AxisStepper::AxisStepper(Axis axis, std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon)
    : axis_(axis), dev_factory_(dev_factory), abort_beacon_(abort_beacon) {
    SetupTimer();
    SetupStateMachine();
    ConnectSignals();
}

AxisStepper::~AxisStepper() {}

void AxisStepper::ConnectSignals() {}

void AxisStepper::SetupTimer() {
    timer_.setSingleShot(true);
    timer_.setInterval(20000);
    QObject::connect(&timer_, &QTimer::timeout, this, [=]() { abort_beacon_->Activate(); abort_beacon_->BroadcastedMessage("AxisStepper timeout");});
}

void AxisStepper::SetupStateMachine() {
       QState *sSuperState = new QState();
       QState *sIdling = new QState(sSuperState);
       QState *sMovingToStart = new QState(sSuperState);
       QState *sReady = new QState(sSuperState);
       QState *sMovingStep = new QState(sSuperState);

//       QObject::connect(sIdling, &QState::entered, this, [=]() { StateMachineMessage("Idling entered"); });
//       QObject::connect(sIdling, &QState::exited, this, [=]() { StateMachineMessage("Idling exited"); });
//       QObject::connect(sMovingToStart, &QState::entered, this, [=]() { StateMachineMessage("MovingToStart entered"); });
//       QObject::connect(sMovingToStart, &QState::exited, this, [=]() { StateMachineMessage("MovingToStart exited"); });
//       QObject::connect(sReady, &QState::entered, this, [=]() { StateMachineMessage("Ready entered"); });
//       QObject::connect(sReady, &QState::exited, this, [=]() { StateMachineMessage("Ready exited"); });
//       QObject::connect(sMovingStep, &QState::entered, this, [=]() { StateMachineMessage("MovingStep entered"); });
//       QObject::connect(sMovingStep, &QState::exited, this, [=]() { StateMachineMessage("MovingStep exited"); });

       // SuperState
       sSuperState->setInitialState(sIdling);
       sSuperState->addTransition(abort_beacon_, &Beacon::Activated, sSuperState);
       sSuperState->addTransition(this, &AxisStepper::MoveToStart, sMovingToStart);

       // Idling state
       // no actions

       // MovingToStart
       QObject::connect(sMovingToStart, &QState::entered, this,  [=](){ timer_.start(60000); });
       QObject::connect(sMovingToStart, &QState::exited, this,  [=](){ timer_.stop(); });
       sMovingToStart->addTransition(this, &AxisStepper::MovementFinished, sReady);

       // Ready state
       QObject::connect(sReady,  &QState::entered, this, [=]() { emit Ready(true); });
       QObject::connect(sReady,  &QState::exited, this, [=]() { emit Ready(false); });
       sReady->addTransition(this, &AxisStepper::MoveStep, sMovingStep);

       // MovingStep state
       QObject::connect(sMovingStep, &QState::entered, this, [=](){ timer_.start(5000); });
       QObject::connect(sMovingStep, &QState::exited, this, [=](){ timer_.stop(); });
       QObject::connect(sMovingStep, &QState::entered, this, &AxisStepper::MoveStepSlot);
       sMovingStep->addTransition(this, &AxisStepper::MovementFinished, sReady);
       sMovingStep->addTransition(this, &AxisStepper::Finished, sReady);

       sm_.addState(sSuperState);
       sm_.setInitialState(sSuperState);
       sm_.start();
}

void AxisStepper::Init(double start, double end, double step) {
    try {
        qDebug() << "AxisStepper::Init Start " << start << " end " << end << " step " << step;
        emit (MoveToStart());
        if (step < 0.00001) {
            throw std::runtime_error("Step size must be larger than 0");
        }
        des_pos_ = start;
        start_ = start;
        end_ = end;
        step_ = start < end ? std::abs(step) : -std::abs(step);
        motor()->MoveToPreset(); // if the motor was disconnected and connected, this is needed before you can move
        motor()->MoveToPosition(start_);
    }
    catch (std::exception& exc) {
        qDebug() << "AxisStepper::Init Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void AxisStepper::MoveStepSlot() {
    try {
        if (AtEndOfRange(des_pos_ + step_)) {
            emit Finished();
        } else {
            des_pos_ += step_;
            qDebug() << "AxisStepper::MoveStep Moving to: " << QString::number(des_pos_, 'f', 3);
            motor()->MoveToPosition(des_pos_);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "AxisStepper::MoveStep Exception caught: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(QString("AxisStepper: ") + exc.what());
    }
}

bool AxisStepper::AtEndOfRange(double pos) {
    const double abs_half_step(std::abs(step_) / 2.0);
    return (pos <= (start_ - abs_half_step) && step_ < 0.0) || (pos >= (end_ + abs_half_step) && step_ > 0.0);
}

void AxisStepper::InvertDirection() {
    step_ = -step_;
}

std::shared_ptr<IMotor> AxisStepper::motor() {
    if (!motor_) {
        motor_ = dev_factory_->GetMotor(axis_);
        QObject::connect(motor_.get(),  &IMotor::CurrentPosition, this, [=](double pos) { emit(CurrentPosition(pos)); });
        QObject::connect(motor_.get(),  &IMotor::MovementFinished, this, [=](double pos) { emit(MovementFinished(pos, des_pos_)); });
        QObject::connect(motor_.get(),  &IMotor::MovementAborted, abort_beacon_, &Beacon::Activate);
        QObject::connect(motor_.get(),  &IMotor::MovementAborted, abort_beacon_, &Beacon::BroadcastMessage);
        QObject::connect(abort_beacon_, &Beacon::Activated, this, &AxisStepper::Stop);
    }
    return motor_;
}

void AxisStepper::Stop() {
    try {
        motor()->Stop();
    }
    catch (std::exception& exc) {
        qWarning() << "AxisStepper::Stop: " << exc.what();
    }
}
