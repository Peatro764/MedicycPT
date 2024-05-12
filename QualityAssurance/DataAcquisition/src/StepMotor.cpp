#include "StepMotor.h"

#include <QDebug>
#include <algorithm>

#include <unistd.h>

StepMotor::StepMotor(std::shared_ptr<LabJack> labjack, std::shared_ptr<Clock> clock, const MotorConfig &config, const Range& range)
    : labjack_(labjack), clock_(clock), config_(config), range_(range) {
    step_finished_.setSingleShot(true);
    QObject::connect(&step_finished_, &QTimer::timeout, this, &StepMotor::StepFinished);
}

StepMotor::~StepMotor() {}

void StepMotor::Configure() {
    qDebug() << "StepMotor::Configure";
    const bool output = true;
    labjack_->ConfigureDIO(config_.FullOrHalfStep(), output);
    labjack_->ConfigureDIO(config_.Preset(), output);
    labjack_->ConfigureDIO(config_.Direction(), output);
    labjack_->ConfigurePulseOut(config_.Pulse(),
                                clock_->config().clock(),
                                config_.HighToLowTransitionCount(),
                                config_.LowToHighTransitionCount(),
                                config_.NPulsesPerStep());
    MoveToPreset();
    FullStep(true);
    pos_dir_ = false; // required so that the first setposdir is done
    SetPosDir();
    configured_ = true;
}

void StepMotor::SetFastSpeed() {
    fast_speed_ = true;
}

void StepMotor::SetDefaultSpeed() {
    fast_speed_ = false;
}

void StepMotor::SetPosDir() {
    if (!pos_dir_) {
        labjack_->WriteDIO(config_.Direction(), !config_.InvertDirection());
        pos_dir_ = true;
    }
}

void StepMotor::SetNegDir() {
    if (pos_dir_) {
        labjack_->WriteDIO(config_.Direction(), config_.InvertDirection());
        pos_dir_ = false;
    }
}

bool StepMotor::IsPosDir() {
    const bool is_pos = labjack_->ReadDIO(config_.Direction());
    if (config_.InvertDirection()) {
        return !is_pos;
    } else {
        return is_pos;
    }
}

double StepMotor::ActPos() {
    return act_step_ * config_.DistancePerStep() + config_.Offset();
}

void StepMotor::MoveSingleStep() {
    if (!steps_to_go_) {
        emit MovementStarted(ActPos());
    }
    steps_to_go_ = 1;
    DoSteps();
}

void StepMotor::DoSteps() {
    try {
        CheckInRange(this->ActPos());
        labjack_->StartPulseOut(config_.Pulse());
        step_finished_.start(config_.MSecPerStep(fast_speed_, ActPos()));
    }
    catch (std::exception& exc) {
        qDebug() << "StepMotor::DoSteps " << exc.what();
        Stop();
        emit MovementAborted(exc.what());
    }
}

void StepMotor::StepFinished() {
    act_step_ += (pos_dir_ ? 1 : -1);
    emit CurrentPosition(ActPos());
    if (--steps_to_go_ > 0) {
        DoSteps();
    } else {
        emit MovementFinished(ActPos());
    }
}

void StepMotor::MoveDistance(double distance) {
    if (!steps_to_go_) {
        emit MovementStarted(ActPos());
    }
    distance < 0.0 ? SetNegDir() : SetPosDir();
    const int steps = static_cast<int>(std::round(distance / config_.DistancePerStep()));
    steps_to_go_ = std::abs(steps);
    if (!steps_to_go_) {
        emit MovementFinished(ActPos());
    } else {
        DoSteps();
    }
}

void StepMotor::MoveToPosition(double position) {
    CheckInRange(position);
    double distance = position - this->ActPos();
    MoveDistance(distance);
}

void StepMotor::CheckInRange(double pos) {
    if (!range_.IsInside(pos)) {
        QString message = "Desired position (" + QString::number(pos, 'f', 2) + ") outside allowed range: " + range_.toString();
        throw std::runtime_error(message.toStdString().c_str());
    }
}

void StepMotor::MoveToZero() {
    MoveDistance(-this->ActPos() + config_.Offset());
}

void StepMotor::FullStep(bool yes) {
    labjack_->WriteDIO(config_.FullOrHalfStep(), yes);
}

void StepMotor::MoveToPreset() {
    labjack_->WriteDIO(config_.Preset(), false);
    usleep(20000);
    labjack_->WriteDIO(config_.Preset(), true);
    usleep(20000);
    labjack_->WriteDIO(config_.Preset(), false);
    usleep(20000);
}

void StepMotor::Stop() {
    steps_to_go_ = 0;
}

void StepMotor::SetCurrentToZeroPos() {
    if (steps_to_go_ == 0) {
        act_step_ = 0;
        emit CurrentPosition(ActPos());
    } else {
        qWarning() << "StepMotor::SetCurrentToZeroPos Cannot do this while axis is moving";
    }
}

void StepMotor::SetCurrentPos(double position) {
    if (!range_.IsInside(position)) {
        QString message = "Desired position (" + QString::number(position, 'f', 2) + ") outside allowed range: " + range_.toString();
        throw std::runtime_error(message.toStdString().c_str());
    }
    act_step_ = static_cast<int>(std::round((position - config_.Offset()) / config_.DistancePerStep()));
    emit CurrentPosition(ActPos());
}

void StepMotor::SetToZeroPos(double pos) {
    if (steps_to_go_ == 0) {
        act_step_ -= (pos / config_.DistancePerStep());
        emit CurrentPosition(ActPos());
    } else {
        qWarning() << "StepMotor::SetToZeroPos Cannot do this while axis is moving";
    }

}

bool StepMotor::IsZeroPos() const {
    return (act_step_ == 0);
}


