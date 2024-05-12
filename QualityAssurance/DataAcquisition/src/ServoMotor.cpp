#include "ServoMotor.h"

#include <QDebug>

ServoMotor::ServoMotor(std::shared_ptr<IMotor> step_motor, std::shared_ptr<ISensor> position_sensor,
                       const Range& range)
    : step_motor_(step_motor), position_sensor_(position_sensor), range_(range) {
    QObject::connect(dynamic_cast<StepMotor*>(step_motor_.get()), &StepMotor::MovementAborted, this, [=](QString msg){ emit MovementAborted(msg); });
    QObject::connect(dynamic_cast<StepMotor*>(step_motor_.get()), &StepMotor::MovementFinished, this, &ServoMotor::DoMovement);
}

ServoMotor::~ServoMotor() {}

void ServoMotor::Configure() {
    step_motor_->Configure();
    position_sensor_->Configure();
}

void ServoMotor::SetDefaultSpeed() {
    step_motor_->SetDefaultSpeed();
}

void ServoMotor::SetFastSpeed() {
    step_motor_->SetFastSpeed();
}

double ServoMotor::ActPos() {
    return position_sensor_->InterpretedValue();
}

double ServoMotor::RefPos() {
    return step_motor_->RefPos();
}

void ServoMotor::SetPosDir() {
    step_motor_->SetPosDir();
}

void ServoMotor::SetNegDir() {
    step_motor_->SetNegDir();
}

bool ServoMotor::IsPosDir() {
    return step_motor_->IsPosDir();
}

void ServoMotor::MoveSingleStep() {
    step_motor_->MoveSingleStep();
}

void ServoMotor::MoveDistance(double distance) {
    this->MoveToPosition(distance + this->ActPos());
}

void ServoMotor::MoveToPosition(double position) {
    if (!range_.IsInside(position)) {
        QString message = "Desired position (" + QString::number(position, 'f', 2) + ") outside allowed range: " + range_.toString();
        throw std::runtime_error(message.toStdString().c_str());
    }
    emit MovementStarted(this->ActPos());
    target_position_ = position;
//    QObject::connect(dynamic_cast<StepMotor*>(step_motor_.get()), &StepMotor::MovementFinished, this, &ServoMotor::DoMovement);
    enable_servo_loop_ = true;
    DoMovement();
}

void ServoMotor::DoMovement() {
    try { // unless catched here, this could go uncatched throughout the application
        if (!enable_servo_loop_) {
            return;
        }
        const double act_pos = this->ActPos();
        emit CurrentPosition(act_pos);
        const double distance = target_position_ - act_pos;
//        qDebug() << "ServoMotor::DoMovement act_pos " << act_pos << " target " << target_position_ << " dist " << distance;
        if (std::abs(distance) < 0.8 * this->StepSize()) {
            enable_servo_loop_ = false;
            emit MovementFinished(act_pos);
            return;
        }
        CheckThatAxisIsMoving(act_pos, prev_pos_);
        prev_pos_ = act_pos;
//        qDebug() << "act_pos " << act_pos << " target " << target_position_;
        if (distance < -1.5*this->StepSize()) {
            SetNegDir();
        } else if (distance > 1.5*this->StepSize()) {
            SetPosDir();
        } else {}
        step_motor_->MoveSingleStep();
    }
    catch (std::exception& exc) {
        enable_servo_loop_ = false;
        qDebug() << "ServoMotor::DoMovement Exception thrown: " << exc.what();
        emit MovementAborted(exc.what());
    }
}

void ServoMotor::CheckThatAxisIsMoving(double act_pos, double old_pos) {
    if (std::abs(act_pos - old_pos) < 0.5 * this->StepSize()) {
        n_no_movement_steps_++;
        if (n_no_movement_steps_ > 5) {
            n_no_movement_steps_ = 0;
            throw std::runtime_error("Axis moved but position sensor did not change");
        }
    } else {
        n_no_movement_steps_ = 0;
    }
}

void ServoMotor::MoveToZero() {
    this->MoveToPosition(0.0);
}

void ServoMotor::FullStep(bool yes) {
    step_motor_->FullStep(yes);
}

void ServoMotor::MoveToPreset() {
    step_motor_->MoveToPreset();
}

void ServoMotor::Stop() {
    target_position_ = this->ActPos();
    enable_servo_loop_ = false;
}

void ServoMotor::SetCurrentToZeroPos() {
    qWarning() << "ServoMotor::SetCurrentToZeroPos Not implemented for servo motors";
}

void ServoMotor::SetCurrentPos(double position) {
    (void)position;
    qWarning() << "ServoMotor::SetCurrentPos Not implemented for servo motors";
}

void ServoMotor::SetToZeroPos(double pos) {
    (void)pos;
}

bool ServoMotor::IsZeroPos() const {
    return step_motor_->IsZeroPos();
}
