#include "MockMotor.h"

#include <QDebug>

MockMotor::MockMotor() {

}

MockMotor::~MockMotor() {}

void MockMotor::Configure() {

}

void MockMotor::SetDefaultSpeed() {

}

void MockMotor::SetFastSpeed() {

}

double MockMotor::ActPos() {
    return static_cast<double>(act_pos_);
}

double MockMotor::RefPos() {
    return 0.0;
}

void MockMotor::MoveDistance(double mm) {
    act_pos_ += mm;
    qDebug() << "MockMotor::MoveDistance New pos: " << act_pos_;
}

void MockMotor::MoveToPosition(double mm) {
    act_pos_ = mm;
    qDebug() << "MockMotor::MoveToPosition New pos: " << act_pos_;
}

void MockMotor::MoveSingleStep() {
    act_pos_ += pos_dir_ ? 1 : -1;
    qDebug() << "MockMotor::MoveSingleStep New pos: " << act_pos_;
}

void MockMotor::FullStep(bool yes) {
    qDebug() << "MockMotor::FullStep " << yes;
}

void MockMotor::MoveToZero() {
    act_pos_ = 0;
    qDebug() << "MockMotor::MoveToZero New pos: " << act_pos_;
}

void MockMotor::MoveToPreset() {
    act_pos_ = 0;
}

void MockMotor::Stop() {
    qDebug() << "MockMotor::Stop: " << act_pos_;
}

bool MockMotor::IsZeroPos() const {
    return true;
}


