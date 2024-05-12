#include "Clock.h"

#include <QDebug>

Clock::Clock(std::shared_ptr<LabJack> labjack, ClockConfig config)
    : labjack_(labjack), config_(config) {}

Clock::~Clock() {}

void Clock::Configure() {
    labjack_->ConfigureClock(config_.clock(), config_.divisor(), config_.roll_value());
    configured_ = true;
}

void Clock::Start() {
    labjack_->StartClock(config_.clock());
}

void Clock::Stop() {
    labjack_->StopClock(config_.clock());
}

int Clock::Read() {
    return labjack_->ReadClock(config_.clock());
}
