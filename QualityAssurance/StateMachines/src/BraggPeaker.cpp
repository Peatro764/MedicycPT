#include "BraggPeaker.h"

#include "Axis.h"

BraggPeaker::BraggPeaker(std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon, QA_HARDWARE hardware)
    : dev_factory_(dev_factory), abort_beacon_(abort_beacon), hardware_(hardware) {
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
}

BraggPeaker::~BraggPeaker() {}

void BraggPeaker::ConnectSignals() {
}

void BraggPeaker::SetupTimer() {
    timer_.setSingleShot(true);
    timer_.setInterval(60000);
    QObject::connect(&timer_, &QTimer::timeout, this, [=]() { abort_beacon_->Activate();
                                                              emit abort_beacon_->BroadcastedMessage("BraggPeaker timeout");});
}

void BraggPeaker::SetupAxisScanners() {
    axis_scanners_[Axis::Z] = std::shared_ptr<AxisScan>(new AxisScan(Axis::Z, dev_factory_, abort_beacon_));
    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::Ready, this, [=](bool yes) { if (yes) emit ZReady(); });
    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::Measurement , this, [=](double pos, BeamSignal signal) { (void)pos; AddMeasurement(signal); });
    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::Finished, this, [=]() { emit Finished(); });
    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) {
        axis_position_[Axis::Z] = act_pos; dpos_[Axis::Z] = act_pos - des_pos; });

    if (hardware_ == QA_HARDWARE::SCANNER3D) {
        axis_scanners_[Axis::X] = std::shared_ptr<AxisScan>(new AxisScan(Axis::X, dev_factory_, abort_beacon_));
        axis_scanners_[Axis::Y] = std::shared_ptr<AxisScan>(new AxisScan(Axis::Y, dev_factory_, abort_beacon_));
        QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::Ready, this, [=](bool yes) { if (yes) emit XReady(); });
        QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::Ready, this, [=](bool yes) { if (yes) emit YReady(); });
        QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) {
            axis_position_[Axis::X] = act_pos;  dpos_[Axis::X] = act_pos - des_pos; });
        QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) {
            axis_position_[Axis::Y] = act_pos; dpos_[Axis::Y] = act_pos - des_pos; });
    }
}

void BraggPeaker::SetupStateMachine() {
    QState *sSuperState = new QState();

    QState *sDataAcquisition = new QState(sSuperState);
    QState *sIdling = new QState(sDataAcquisition);
    QState *sMovingToStart = new QState(sDataAcquisition);
    QState *sMeasuring = new QState(sDataAcquisition);
    QState *sMovingToZero = new QState(sDataAcquisition);

    QObject::connect(sDataAcquisition, &QState::entered, this, [=]() { StateMachineMessage("DataAcquisition entered"); });
    QObject::connect(sDataAcquisition, &QState::exited, this, [=]() { StateMachineMessage("DataAcquisition exited"); });
    QObject::connect(sIdling, &QState::entered, this, [=]() { StateMachineMessage("Idling entered"); });
    QObject::connect(sIdling, &QState::exited, this, [=]() { StateMachineMessage("Idling exited"); });
    QObject::connect(sMovingToStart, &QState::entered, this, [=]() { StateMachineMessage("MovingToStart entered"); });
    QObject::connect(sMovingToStart, &QState::exited, this, [=]() { StateMachineMessage("MovingToStart exited"); });
    QObject::connect(sMeasuring, &QState::entered, this, [=]() { StateMachineMessage("Measuring entered"); });
    QObject::connect(sMeasuring, &QState::exited, this, [=]() { StateMachineMessage("Measuring exited"); });
    QObject::connect(sMovingToZero, &QState::entered, this, [=]() { StateMachineMessage("MovingToZero entered"); });
    QObject::connect(sMovingToZero, &QState::exited, this, [=]() { StateMachineMessage("MovingToZero exited"); });

    // SuperState
    sSuperState->addTransition(this,  &BraggPeaker::StopStateMachine, sSuperState);
    sSuperState->setInitialState(sDataAcquisition);

    // DataAcquisition
    sDataAcquisition->addTransition(abort_beacon_, &Beacon::Activated, sDataAcquisition);
    sDataAcquisition->addTransition(this, &BraggPeaker::MoveToStart, sMovingToStart);
    sDataAcquisition->addTransition(this, &BraggPeaker::MovingToZero, sMovingToZero);
    sDataAcquisition->setInitialState(sIdling);

    // Idling state
    QObject::connect(sIdling, &QState::entered, this, [=]() { is_idle_ = true; });
    QObject::connect(sIdling, &QState::exited, this, [=]() { is_idle_ = false; });
    QTimer *tIdle = new QTimer;
    tIdle->setSingleShot(false);
    tIdle->setInterval(500);
    QObject::connect(tIdle, &QTimer::timeout, this, &BraggPeaker::ReadSensorValues);
    QObject::connect(sIdling, &QState::entered, tIdle, qOverload<>(&QTimer::start));
    QObject::connect(sIdling, &QState::exited, tIdle, &QTimer::stop);

    // MovingToStart
    QObject::connect(sMovingToStart, &QState::entered, this, [=]() { timer_.start(60000); });
    QObject::connect(sMovingToStart, &QState::exited, this, [=]() { timer_.stop(); });
    sMovingToStart->addTransition(this, &BraggPeaker::ZReady, sMeasuring);

    // Measuring state
    QObject::connect(sMeasuring, &QState::entered, this, [=](){ timer_.start(180000); });
    QObject::connect(sMeasuring, &QState::exited, this, [=](){ timer_.stop(); });
    //QObject::connect(sMeasuring, &QState::entered, this, &BraggPeaker::DoBraggPeak); // TODO REMOVE IF BELOW WORKS
    QObject::connect(sMeasuring, &QState::entered, this, [=](){ QTimer::singleShot(2000, this, &BraggPeaker::DoBraggPeak); });
    sMeasuring->addTransition(this, &BraggPeaker::ZReady, sMovingToZero);

    // MovingToZero
    QObject::connect(sMovingToZero, &QState::entered, this,  [=](){ timer_.start(180000); });
    QObject::connect(sMovingToZero, &QState::exited, this, [=](){ timer_.stop(); });
    //QObject::connect(sMovingToZero, &QState::entered, this, &BraggPeaker::MoveToZero); // TODO REMOVE IF BELOW WORKS
    QObject::connect(sMovingToZero, &QState::entered, this, [=](){ QTimer::singleShot(2000, this, &BraggPeaker::MoveToZero); });
    sMovingToZero->addTransition(this, &BraggPeaker::ZReady, sIdling);

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void BraggPeaker::Start(const Range& range, double step, int n_measurements, double max_std_err, bool fixed_nmb_measurements) {
    try {
        emit MoveToStart();
        step_ = std::abs(step);
        scan_range_ = range;
        if (step_ < 0.00001) {
            throw std::runtime_error("Step size must be larger than 0");
        }
        axis_scanners_[Axis::Z]->Init(range.start(), range.end(), step_, n_measurements, max_std_err, fixed_nmb_measurements);
    }
    catch (std::exception& exc) {
        qDebug() << "BraggPeaker::Init Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void BraggPeaker::RepositionX() {
    try {
        axis_scanners_[Axis::X]->Init(x_pos_, x_pos_, 0.1, 1, 100, false);
    }
    catch (std::exception& exc) {
        qDebug() << "BraggPeaker::RepositionX Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void BraggPeaker::RepositionY() {
    try {
        axis_scanners_[Axis::Y]->Init(y_pos_, y_pos_, 0.1, 1, 100, false);
    }
    catch (std::exception& exc) {
        qDebug() << "BraggPeaker::RepositionY Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void BraggPeaker::MoveToZero() {
    try {
        axis_scanners_[Axis::Z]->Init(scan_range_.start(), scan_range_.start(), step_, 1, 100, false);
    }
    catch (std::exception& exc) {
        qDebug() << "BraggPeaker::MoveToZero Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void BraggPeaker::DoBraggPeak() {
    qDebug() << "BraggPeaker::DoBraggPeak";
    emit axis_scanners_[Axis::Z]->Start();
}

void BraggPeaker::AddMeasurement(BeamSignal signal) {
    Point p = CurrentPosition();
    MeasurementPoint m(p, signal);
    emit Measurement(m);
}

Point BraggPeaker::CurrentPosition() const {
    return Point(axis_position_.at(Axis::X),
                 axis_position_.at(Axis::Y),
                 axis_position_.at(Axis::Z));
}

void BraggPeaker::ReadSensorValues() {
    if (dev_factory_->IsConnected()) {
        try {
            auto chambre = dev_factory_->GetSensor(SensorType::CHAMBRE);
            emit SIGNAL_ChambreVoltage(chambre->RawValue());
            auto diode = dev_factory_->GetSensor(SensorType::DIODE);
            emit SIGNAL_DiodeVoltage(diode->RawValue());
        } catch (std::exception& exc) {
            qWarning() << "BeamProfiler Failed reading chambre or diode voltage: " << exc.what();
        }
    }
}
