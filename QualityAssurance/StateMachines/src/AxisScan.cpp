#include "AxisScan.h"

#include "Axis.h"
#include "ServoMotor.h"
#include "StepMotor.h"

AxisScan::AxisScan(Axis axis, std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon)
    : axis_(axis), dev_factory_(dev_factory), abort_beacon_(abort_beacon) {
    qDebug() << "AxisScan";
    SetupTimer();
    SetupStateMachine();
    ConnectSignals();
}

AxisScan::~AxisScan() {
    qDebug() << "~AxisScan";
}

void AxisScan::ConnectSignals() {}

void AxisScan::SetupTimer() {
    timer_.setSingleShot(true);
    timer_.setInterval(20000);
    QObject::connect(&timer_, &QTimer::timeout, this, [=]() { abort_beacon_->Activate(); abort_beacon_->BroadcastedMessage("AxisScan timeout");});
}

void AxisScan::SetupStateMachine() {
       QState *sSuperState = new QState();
       QState *sIdling = new QState(sSuperState);
       QState *sMovingToStart = new QState(sSuperState);
       QState *sReady = new QState(sSuperState);
       QState *sMeasuring = new QState(sSuperState);
       QState *sMovingStep = new QState(sSuperState);

//       QObject::connect(sIdling, &QState::entered, this, [=]() { StateMachineMessage("Idling entered"); });
//       QObject::connect(sIdling, &QState::exited, this, [=]() { StateMachineMessage("Idling exited"); });
//       QObject::connect(sMovingToStart, &QState::entered, this, [=]() { StateMachineMessage("MovingToStart entered"); });
//       QObject::connect(sMovingToStart, &QState::exited, this, [=]() { StateMachineMessage("MovingToStart exited"); });
//       QObject::connect(sReady, &QState::entered, this, [=]() { StateMachineMessage("Ready entered"); });
//       QObject::connect(sReady, &QState::exited, this, [=]() { StateMachineMessage("Ready exited"); });
//       QObject::connect(sMeasuring, &QState::entered, this, [=]() { StateMachineMessage("Measuring entered"); });
//       QObject::connect(sMeasuring, &QState::exited, this, [=]() { StateMachineMessage("Measuring exited"); });
//       QObject::connect(sMovingStep, &QState::entered, this, [=]() { StateMachineMessage("MovingStep entered"); });
//       QObject::connect(sMovingStep, &QState::exited, this, [=]() { StateMachineMessage("MovingStep exited"); });

       // SuperState
       sSuperState->setInitialState(sIdling);
       sSuperState->addTransition(abort_beacon_, &Beacon::Activated, sSuperState);
       sSuperState->addTransition(this, &AxisScan::MoveToStart, sMovingToStart);

       // Idling state
       // no actions

       // MovingToStart
       QObject::connect(sMovingToStart, &QState::entered, this, [=](){ timer_.start(180000); });
       QObject::connect(sMovingToStart, &QState::exited, this, [=](){ timer_.stop(); });
       sMovingToStart->addTransition(this, &AxisScan::MovementFinished, sReady);

       // Ready state
       QObject::connect(sReady,  &QState::entered, this, [=]() { emit Ready(true); });
       QObject::connect(sReady,  &QState::entered, this, [=]() { motor()->SetDefaultSpeed(); });
       QObject::connect(sReady,  &QState::exited, this, [=]() { emit Ready(false); });
       sReady->addTransition(this, &AxisScan::Start, sMeasuring);

       // Measuring state
       QObject::connect(sMeasuring, &QState::entered, this, [=](){ timer_.start(5000); });
       QObject::connect(sMeasuring, &QState::exited, this, [=](){ timer_.stop(); });
       QObject::connect(sMeasuring, &QState::entered, this, &AxisScan::DoMeasurement);
       sMeasuring->addTransition(this, &AxisScan::Measurement, sMovingStep);

       // MovingStep state
       QObject::connect(sMovingStep, &QState::entered, this, [=](){ timer_.start(5000); });
       QObject::connect(sMovingStep, &QState::entered, this, [=](){ timer_.stop(); });
       QObject::connect(sMovingStep, &QState::entered, this, &AxisScan::MoveStep);
       sMovingStep->addTransition(this, &AxisScan::MovementFinished, sMeasuring);
       sMovingStep->addTransition(this, &AxisScan::Finished, sReady);

       sm_.addState(sSuperState);
       sm_.setInitialState(sSuperState);
       sm_.start();
}

void AxisScan::Init(double start, double end, double step, int n_measurements, double max_std_err, bool fixed_nmb_measurements) {
    try {
        emit (MoveToStart());
        if (std::abs(step) < 0.00001) {
            throw std::runtime_error("Step size must be larger than 0");
        }
        des_pos_ = start;
        start_ = start;
        end_ = end;
        step_ = start < end ? std::abs(step) : -std::abs(step);
        n_measurements_ = n_measurements;
        max_std_err_ = max_std_err;
        fixed_nmb_measurements_ = fixed_nmb_measurements;
        if (motor()->IsZeroPos()) motor()->MoveToPreset();
        motor()->SetFastSpeed();
        motor()->MoveToPosition(start_);
    }
    catch (std::exception& exc) {
        qDebug() << "AxisScan::Init Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void AxisScan::InvertDirection() {
    step_ = -step_;
}

void AxisScan::DoMeasurement() {
    try {
        //        const double reference = dev_factory_->GetSensor(SensorType::CHAMBRE)->InterpretedValue(n_measurements_);
        //        const double diode = dev_factory_->GetSensor(SensorType::DIODE)->InterpretedValue(n_measurements_);
        const std::vector<double> values = dev_factory_->ReadMultipleSensors(SensorType::CHAMBRE, SensorType::DIODE,
                                                                             n_measurements_, max_std_err_, fixed_nmb_measurements_);
        emit Measurement(pos_, BeamSignal(values.at(1), values.at(0)));
    }
    catch (std::exception& exc) {
        qWarning() << "LineStateMachine::DoMeasurement Exception caught: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(QString("AxisScan: ") + exc.what());
    }
}

void AxisScan::MoveStep() {
    try {
        if (AtEndOfRange(des_pos_ + step_)){
            emit Finished();
        } else {
            des_pos_ += step_;
//            qDebug() << "AxisScan::MoveStep Moving to: " << QString::number(des_pos_, 'f', 3);
            motor()->MoveToPosition(des_pos_);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "AxisScan::MoveStep Exception caught: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(QString("AxisScan: ") + exc.what());
    }
}

bool AxisScan::AtEndOfRange(double pos) {
    const double abs_half_step(std::abs(step_) / 2.0);
    return (pos <= (start_ - abs_half_step) && step_ < 0.0) || (pos >= (end_ + abs_half_step) && step_ > 0.0);
}

std::shared_ptr<IMotor> AxisScan::motor() {
    if (!motor_) {
        motor_ = dev_factory_->GetMotor(axis_);
        QObject::connect(motor_.get(),  &IMotor::CurrentPosition, this, [=](double pos) { emit(CurrentPosition(pos)); });
        QObject::connect(motor_.get(),  &IMotor::CurrentPosition, this, [=](double pos) { pos_ = pos; });
        QObject::connect(motor_.get(),  &IMotor::MovementFinished, this, [=](double pos) { emit(MovementFinished(pos, des_pos_)); });
        QObject::connect(motor_.get(),  &IMotor::MovementAborted, abort_beacon_, &Beacon::Activate);
        QObject::connect(motor_.get(),  &IMotor::MovementAborted, abort_beacon_, &Beacon::BroadcastMessage);
        QObject::connect(abort_beacon_, &Beacon::Activated, this, &AxisScan::Stop);
    }
    return motor_;
}

void AxisScan::Stop() {
    try {
        motor()->Stop();
    }
    catch (std::exception& exc) {
        qWarning() << "AxisScan::Stop: " << exc.what();
    }
}
