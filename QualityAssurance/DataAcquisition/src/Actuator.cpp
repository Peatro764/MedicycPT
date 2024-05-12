#include "Actuator.h"

#include <QDebug>

Actuator::Actuator(std::shared_ptr<LabJack> labjack, t7::AOUT ch)
    : labjack_(labjack), ch_(ch) {}

Actuator::~Actuator() {}

void Actuator::Write(double voltage) {
    labjack_->WriteAOUT(ch_, voltage);
}
