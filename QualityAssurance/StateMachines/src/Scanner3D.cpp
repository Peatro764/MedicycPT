#include "Scanner3D.h"

#include "Axis.h"

Scanner3D::Scanner3D(std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon)
    : dev_factory_(dev_factory), abort_beacon_(abort_beacon) {
    qDebug() << "Scanner3D::Scanner3D Start";
    axis_position_[Axis::X] = 0.0;
    axis_position_[Axis::Y] = 0.0;
    axis_position_[Axis::Z] = 0.0;
    dpos_[Axis::X] = 0.0;
    dpos_[Axis::Y] = 0.0;
    dpos_[Axis::Z] = 0.0;
    SetupTimer();
    SetupStateMachine();
    ConnectSignals();
    SetupAxisScanners();
    qDebug() << "Scanner3D::Scanner3D End";
}

Scanner3D::~Scanner3D() {}

void Scanner3D::ConnectSignals() {
}

void Scanner3D::SetupTimer() {
    timer_.setSingleShot(true);
    timer_.setInterval(60000);
    QObject::connect(&timer_, &QTimer::timeout, this, [=]() { abort_beacon_->Activate(); abort_beacon_->BroadcastedMessage("Scanner3D timeout");});
}

void Scanner3D::SetupAxisScanners() {
    x_scanner_ = std::shared_ptr<AxisScan>(new AxisScan(Axis::X, dev_factory_, abort_beacon_));
    y_stepper_ = std::shared_ptr<AxisStepper>(new AxisStepper(Axis::Y, dev_factory_, abort_beacon_));
    z_stepper_ = std::shared_ptr<AxisStepper>(new AxisStepper(Axis::Z, dev_factory_, abort_beacon_));

    QObject::connect(x_scanner_.get(), &AxisScan::Ready, this,  [=](bool yes) { yes ? ++axes_ready_ : --axes_ready_; CheckAllAxesReady(); });
    QObject::connect(y_stepper_.get(), &AxisStepper::Ready, this, [=](bool yes) { yes ? ++axes_ready_ : --axes_ready_; CheckAllAxesReady(); });
    QObject::connect(z_stepper_.get(), &AxisStepper::Ready, this, [=](bool yes) { yes ? ++axes_ready_ : --axes_ready_; CheckAllAxesReady(); });

    QObject::connect(x_scanner_.get(), &AxisScan::Finished, x_scanner_.get(), &AxisScan::InvertDirection);
    QObject::connect(x_scanner_.get(), &AxisScan::Finished, y_stepper_.get(), &AxisStepper::MoveStep);
    QObject::connect(y_stepper_.get(), &AxisStepper::Finished, y_stepper_.get(), &AxisStepper::InvertDirection);
    QObject::connect(y_stepper_.get(), &AxisStepper::Finished, z_stepper_.get(), &AxisStepper::MoveStep);
    QObject::connect(z_stepper_.get(), &AxisStepper::Finished, this, &Scanner3D::Finished);

    QObject::connect(x_scanner_.get(), &AxisScan::Measurement , this, [=](double pos, BeamSignal signal) { (void)pos; AddMeasurement(signal); });

    QObject::connect(x_scanner_.get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) {
        axis_position_[Axis::X] = act_pos; dpos_[Axis::X] = act_pos - des_pos; });
    QObject::connect(y_stepper_.get(), &AxisStepper::MovementFinished, this, [=](double act_pos, double des_pos) {
        axis_position_[Axis::Y] = act_pos; dpos_[Axis::Y] = act_pos - des_pos;});
    QObject::connect(z_stepper_.get(), &AxisStepper::MovementFinished, this, [=](double act_pos, double des_pos) {
        axis_position_[Axis::Z] = act_pos; dpos_[Axis::Z] = act_pos - des_pos;});
}

void Scanner3D::SetupStateMachine() {
       QState *sSuperState = new QState();
       QState *sIdling = new QState(sSuperState);
       QState *sMovingToPosition = new QState(sSuperState);
       QState *sMeasuring = new QState(sSuperState);

       QObject::connect(sIdling, &QState::entered, this, [=]() { StateMachineMessage("Idling entered"); });
       QObject::connect(sIdling, &QState::exited, this, [=]() { StateMachineMessage("Idling exited"); });
       QObject::connect(sMovingToPosition, &QState::entered, this, [=]() { StateMachineMessage("MovingToPosition entered"); });
       QObject::connect(sMovingToPosition, &QState::exited, this, [=]() { StateMachineMessage("MovingToPosition exited"); });
       QObject::connect(sMeasuring, &QState::entered, this, [=]() { StateMachineMessage("Measuring entered"); });
       QObject::connect(sMeasuring, &QState::exited, this, [=]() { StateMachineMessage("Measuring exited"); });

       // SuperState
       sSuperState->setInitialState(sIdling);
       sSuperState->addTransition(this,  &Scanner3D::StopStateMachine, sSuperState);
       sSuperState->addTransition(abort_beacon_, &Beacon::Activated, sSuperState);
       sSuperState->addTransition(this, &Scanner3D::MoveToStart, sMovingToPosition);

       // Idling state
       // no actions

       // MovingToPosition
       QObject::connect(sMovingToPosition, &QState::entered, this, [=]() { timer_.start(30000); });
       QObject::connect(sMovingToPosition, &QState::exited, this, [=]() { timer_.stop(); });
       sMovingToPosition->addTransition(this, &Scanner3D::AllAxesReady, sMeasuring);

       // Measuring state
       QObject::connect(sMeasuring, &QState::entered, this, [=]() { timer_.start(60000); });
       QObject::connect(sMeasuring, &QState::exited, this, [=]() { timer_.stop(); });
       QObject::connect(sMeasuring, &QState::entered, this, [=]() { x_scanner_->Start(); });
       sMeasuring->addTransition(this, &Scanner3D::AllAxesReady, sMeasuring);
       sMeasuring->addTransition(this, &Scanner3D::Finished, sIdling);

       sm_.addState(sSuperState);
       sm_.setInitialState(sSuperState);
       sm_.start();
}

void Scanner3D::Start(std::map<Axis, Range> ranges,
                      std::map<Axis, double> steps,
                      int n_measurements, double max_std_err, bool fixed_nmb_measurements) {
    try {
        emit (MoveToStart());
        x_scanner_->Init(ranges[Axis::X].start(), ranges[Axis::X].end(), steps[Axis::X], n_measurements, max_std_err, fixed_nmb_measurements);
        y_stepper_->Init(ranges[Axis::Y].start(), ranges[Axis::Y].end(), steps[Axis::Y]);
        z_stepper_->Init(ranges[Axis::Z].start(), ranges[Axis::Z].end(), steps[Axis::Z]);
    }
    catch (std::exception& exc) {
        qDebug() << "Scanner3D::Init Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void Scanner3D::AddMeasurement(BeamSignal signal) {
    Point p = CurrentPosition();
    MeasurementPoint m(p, signal);
    emit Measurement(m);
}

void Scanner3D::CheckAllAxesReady() {
    if (axes_ready_ == 3) {
        emit (AllAxesReady());
    }
}

Point Scanner3D::CurrentPosition() const {
    return Point(axis_position_.at(Axis::X),
                 axis_position_.at(Axis::Y),
                 axis_position_.at(Axis::Z));
}
