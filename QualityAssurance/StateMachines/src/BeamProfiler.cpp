#include "BeamProfiler.h"

#include "Axis.h"

BeamProfiler::BeamProfiler(std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon, QA_HARDWARE hardware)
    : dev_factory_(dev_factory), abort_beacon_(abort_beacon), hardware_(hardware) {
    qDebug() << "BeamProfiler";
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

BeamProfiler::~BeamProfiler() {
    qDebug() << "~BeamProfiler";
}

void BeamProfiler::ConnectSignals() {
}

void BeamProfiler::SetupTimer() {
    timer_.setSingleShot(true);
    timer_.setInterval(60000);
    QObject::connect(&timer_, &QTimer::timeout, this, [=]() { abort_beacon_->Activate(); abort_beacon_->BroadcastedMessage("BeamProfiler timeout");});
}

void BeamProfiler::SetupAxisScanners() {
    axis_scanners_[Axis::X] = std::shared_ptr<AxisScan>(new AxisScan(Axis::X, dev_factory_, abort_beacon_));
    axis_scanners_[Axis::Y] = std::shared_ptr<AxisScan>(new AxisScan(Axis::Y, dev_factory_, abort_beacon_));
    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::Ready, this, [=](bool yes) { CheckAxisReady(Axis::X, yes); });
    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::Ready, this, [=](bool yes) { CheckAxisReady(Axis::Y, yes); });
    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::Finished, this, [=]() { emit ScanFinished(Axis::X); });
    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::Finished, this, [=]() { emit ScanFinished(Axis::Y); });
    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::Measurement , this, [=](double pos, BeamSignal signal) { (void)pos; AddMeasurement(Axis::X, signal); });
    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::Measurement , this, [=](double pos, BeamSignal signal) { (void)pos; AddMeasurement(Axis::Y, signal); });

    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) {
        axis_position_[Axis::X] = act_pos; dpos_[Axis::X] = act_pos - des_pos; });
    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) {
        axis_position_[Axis::Y] = act_pos; dpos_[Axis::Y] = act_pos - des_pos; });

    if (hardware_ == QA_HARDWARE::SCANNER3D) {
        axis_scanners_[Axis::Z] = std::shared_ptr<AxisScan>(new AxisScan(Axis::Z, dev_factory_, abort_beacon_));
        QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::Ready, this, [=](bool yes) { CheckAxisReady(Axis::Z, yes); });
        QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) {
            axis_position_[Axis::Z] = act_pos; dpos_[Axis::Z] = act_pos - des_pos; });
    }
}

void BeamProfiler::SetupStateMachine() {
       QState *sSuperState = new QState();

       QState *sDataAcquisition = new QState(sSuperState);
       QState *sIdling = new QState(sDataAcquisition);
       QState *sMovingToStart = new QState(sDataAcquisition);
       QState *sReposPassiveAxis = new QState(sMovingToStart);
       QState *sReposActiveAxis = new QState(sMovingToStart);
       QState *sMeasuring = new QState(sDataAcquisition);
       QState *sMovingToZero = new QState(sDataAcquisition);
       QState *sFinished = new QState(sDataAcquisition);

       QObject::connect(sDataAcquisition, &QState::entered, this, [=]() { StateMachineMessage("DataAcquisition entered"); });
       QObject::connect(sDataAcquisition, &QState::exited, this, [=]() { StateMachineMessage("DataAcquisition exited"); });
       QObject::connect(sIdling, &QState::entered, this, [=]() { StateMachineMessage("Idling entered"); });
       QObject::connect(sIdling, &QState::exited, this, [=]() { StateMachineMessage("Idling exited"); });
       QObject::connect(sMovingToStart, &QState::entered, this, [=]() { StateMachineMessage("MovingToStart entered"); });
       QObject::connect(sMovingToStart, &QState::exited, this, [=]() { StateMachineMessage("MovingToStart exited"); });
       QObject::connect(sReposPassiveAxis, &QState::entered, this, [=]() { StateMachineMessage("ReposPassiveAxis entered"); });
       QObject::connect(sReposPassiveAxis, &QState::exited, this, [=]() { StateMachineMessage("ReposPassiveAxis exited"); });
       QObject::connect(sReposActiveAxis, &QState::entered, this, [=]() { StateMachineMessage("ReposActiveAxis entered"); });
       QObject::connect(sReposActiveAxis, &QState::exited, this, [=]() { StateMachineMessage("ReposActiveAxis exited"); });
       QObject::connect(sMovingToZero, &QState::entered, this, [=]() { StateMachineMessage("MovingToZero entered"); });
       QObject::connect(sMovingToZero, &QState::exited, this, [=]() { StateMachineMessage("MovingToZero exited"); });
       QObject::connect(sMeasuring, &QState::entered, this, [=]() { StateMachineMessage("Measuring entered"); });
       QObject::connect(sMeasuring, &QState::exited, this, [=]() { StateMachineMessage("Measuring exited"); });
       QObject::connect(sFinished, &QState::entered, this, [=]() { StateMachineMessage("Finished entered"); });
       QObject::connect(sFinished, &QState::exited, this, [=]() { StateMachineMessage("Finished exited"); });

       // SuperState
       sSuperState->addTransition(this,  &BeamProfiler::StopStateMachine, sSuperState);
       sSuperState->setInitialState(sDataAcquisition);

       // DataAcquisition
       sDataAcquisition->addTransition(abort_beacon_, &Beacon::Activated, sDataAcquisition);
       sDataAcquisition->addTransition(this, &BeamProfiler::MoveToStart, sMovingToStart);
       sDataAcquisition->addTransition(this, &BeamProfiler::MovingToZero, sMovingToZero);
       sDataAcquisition->setInitialState(sIdling);

       // Idling state
       QObject::connect(sIdling, &QState::entered, this, [=]() { is_idle_ = true; });
       QObject::connect(sIdling, &QState::entered, this, [=]() { emit AxisIdle(active_axis_); });
       QObject::connect(sIdling, &QState::exited, this, [=]() { is_idle_ = false; });
       QTimer *tIdle = new QTimer;
       tIdle->setSingleShot(false);
       tIdle->setInterval(500);
       QObject::connect(tIdle, &QTimer::timeout, this, &BeamProfiler::ReadSensorValues);
       QObject::connect(sIdling, &QState::entered, tIdle, qOverload<>(&QTimer::start));
       QObject::connect(sIdling, &QState::exited, tIdle, &QTimer::stop);

       // MovingToStart
       QObject::connect(sMovingToStart, &QState::entered, this,  [=](){ timer_.start(300000); });
       QObject::connect(sMovingToStart, &QState::exited, this, [=](){ timer_.stop(); });
       sMovingToStart->setInitialState(sReposPassiveAxis);
       QObject::connect(sReposPassiveAxis, &QState::entered, this, [=](){ MoveToZero(passive_axis_); });
       QObject::connect(sReposActiveAxis, &QState::entered, this,  &BeamProfiler::RepositionActiveAxis);
       sReposPassiveAxis->addTransition(this, &BeamProfiler::PassiveAxisReady, sReposActiveAxis);
       sReposActiveAxis->addTransition(this, &BeamProfiler::ActiveAxisReady, sMeasuring);

       // Measuring state
       QObject::connect(sMeasuring, &QState::entered, this, [=](){ timer_.start(1800000); });
       QObject::connect(sMeasuring, &QState::exited, this, [=](){ timer_.stop(); });
       QObject::connect(sMeasuring, &QState::entered, this, &BeamProfiler::DoProfile);
       sMeasuring->addTransition(this, &BeamProfiler::Acknowledged, sMovingToZero);

       // MovingToZero
       QObject::connect(sMovingToZero, &QState::entered, this,  [=](){ timer_.start(300000); });
       QObject::connect(sMovingToZero, &QState::exited, this, [=](){ timer_.stop(); });
       QObject::connect(sMovingToZero, &QState::entered, this, [=]() { MoveToZero(active_axis_); });
       sMovingToZero->addTransition(this, &BeamProfiler::ActiveAxisReady, sFinished);

       // Finished state
       QObject::connect(sFinished, &QState::entered, this, [=]() { is_finished_ = true; });
       QObject::connect(sFinished, &QState::exited, this, [=]() { is_finished_ = false; });
       QObject::connect(sFinished, &QState::entered, this, [=](){ emit AxisFinished(active_axis_); });
       sFinished->addTransition(sFinished, &QState::entered, sIdling);

       sm_.addState(sSuperState);
       sm_.setInitialState(sSuperState);
       sm_.start();
}

void BeamProfiler::ReadSensorValues() {
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

void BeamProfiler::Start(Axis axis, const Range& range, double step, int n_measurements, double max_std_err, bool fixed_nmb_measurements) {
    try {
        step_ = std::abs(step);
        if (step_ < 0.00001) {
            throw std::runtime_error("Step size must be larger than 0");
        }
        active_axis_ = axis;
        passive_axis_ = GetPassiveAxis(active_axis_);
        scanning_range_ = range;
        n_measurements_ = n_measurements;
        max_std_err_ = max_std_err;
        fixed_nmb_measurements_ = fixed_nmb_measurements;
        emit (MoveToStart());
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfiler::Init Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void BeamProfiler::DoProfile() {
    qDebug() << "BeamProfiler::DoProfile";
    axis_scanners_[active_axis_]->Start();
}

void BeamProfiler::RepositionZAxis() {
    try {
        axis_scanners_[Axis::Z]->Init(z_pos_, z_pos_, 0.1, 1, 100, false);
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfiler::RepositionZAxis Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void BeamProfiler::RepositionActiveAxis() {
    try {
        axis_scanners_[active_axis_]->Init(scanning_range_.start(), scanning_range_.end(), step_, n_measurements_, max_std_err_, fixed_nmb_measurements_);
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfiler::RepositionActiveAxis Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void BeamProfiler::MoveToZero(Axis axis) {
    try {
        axis_scanners_[axis]->Init(0.0, 0.0, step_, n_measurements_, max_std_err_, fixed_nmb_measurements_);
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfiler::MoveToZero Exception thrown: " << exc.what();
        abort_beacon_->Activate();
        abort_beacon_->BroadcastMessage(exc.what());
    }
}

void BeamProfiler::AddMeasurement(Axis axis, BeamSignal signal) {
    Point p = CurrentPosition();
    MeasurementPoint m(p, signal);
    emit (Measurement(axis, m));
}

Axis BeamProfiler::GetPassiveAxis(Axis active_axis) {
    Axis passive_axis = Axis::UNK;
    if (active_axis == Axis::X) {
        passive_axis = Axis::Y;
    } else if (active_axis == Axis::Y) {
        passive_axis = Axis::X;
    } else {
        throw std::runtime_error("Active axis must be either X or Y");
    }
    return passive_axis;
}

void BeamProfiler::CheckAxisReady(Axis axis, bool yes) {
    if (!yes) {
        return;
    }
    if (axis == active_axis_) {
        emit ActiveAxisReady();
    } else if (axis == passive_axis_) {
        emit PassiveAxisReady();
    } else if (axis == Axis::Z) {
        emit ZAxisReady();
    }
}

Point BeamProfiler::CurrentPosition() const {
    return Point(axis_position_.at(Axis::X),
                 axis_position_.at(Axis::Y),
                 axis_position_.at(Axis::Z));
}
