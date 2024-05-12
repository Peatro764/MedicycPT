#ifndef BEAMCHARACTERISTICS_CLOCK_H
#define BEAMCHARACTERISTICS_CLOCK_H

#include "LabJackChannels.h"
#include "LabJack.h"
#include "ClockConfig.h"

#include <memory>

class Clock {

public:
    Clock(std::shared_ptr<LabJack> labjack, ClockConfig config);
    ~Clock();
    void Configure();
    void Start();
    void Stop();
    int Read();
    ClockConfig config() const { return config_; }
    double StepLengthUs() const { return static_cast<double>(config_.divisor() / 80000000); }
    double RollOverTimeUs() const { return (static_cast<double>(config_.roll_value()) * StepLengthUs()); }

private:
    std::shared_ptr<LabJack> labjack_;
    ClockConfig config_;
    bool configured_ = false;
};

#endif
