#ifndef BEAMCHARACTERISTICS_CLOCKCONFIG_H
#define BEAMCHARACTERISTICS_CLOCKCONFIG_H

#include "LabJackChannels.h"

#include <QDebug>

class ClockConfig {

public:
    ClockConfig(t7::CLOCK clock,
                int divisor,
                int roll_value)
        : clock_(clock),
          divisor_(divisor),
          roll_value_(roll_value) {}
    ~ClockConfig() {}

    t7::CLOCK clock() const { return clock_; }
    int divisor() const { return divisor_; }
    int roll_value() const { return roll_value_; }

private:
    t7::CLOCK clock_;
    int divisor_;
    int roll_value_;
};

#endif
