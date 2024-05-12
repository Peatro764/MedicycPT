#ifndef BEAMCHARACTERISTICS_MOTORCONFIG_H
#define BEAMCHARACTERISTICS_MOTORCONFIG_H

#include "LabJackChannels.h"

#include <functional>
#include <QDebug>
#include <math.h>

class MotorConfig {

public:
    MotorConfig(t7::DIO full_or_half_step,
                    t7::DIO direction,
                    t7::DIO preset,
                    t7::PULSEOUT pulse,
                    int high_to_low_transition_count,
                    int low_to_high_transition_count,
                    int n_pulses_per_step,
                    double distance_per_step,
                    double offset,
                    std::vector<double> msec_per_step_constants_default,
                    std::vector<double> msec_per_step_constants_fast,
                    bool invert_direction)
        : full_or_half_step_(full_or_half_step),
          direction_(direction),
          preset_(preset),
          pulse_(pulse),
          low_to_high_transition_count_(low_to_high_transition_count),
          high_to_low_transition_count_(high_to_low_transition_count),
          n_pulses_per_step_(n_pulses_per_step),
          distance_per_step_(distance_per_step),
          offset_(offset),
          msec_per_step_constants_default_(msec_per_step_constants_default),
          msec_per_step_constants_fast_(msec_per_step_constants_fast),
          invert_direction_(invert_direction) {}
    ~MotorConfig() {}

    t7::DIO FullOrHalfStep() const { return full_or_half_step_; }
    t7::DIO Direction() const { return direction_; }
    t7::DIO Preset() const { return preset_; }
    t7::PULSEOUT Pulse() const { return pulse_; }
    int LowToHighTransitionCount() const { return low_to_high_transition_count_; }
    int HighToLowTransitionCount() const { return high_to_low_transition_count_; }
    int NPulsesPerStep() const { return n_pulses_per_step_; }
    double DistancePerStep() const { return distance_per_step_; }
    double Offset() const { return offset_; }
    int MSecPerStep(bool fast_speed, double pos) const {
        if (fast_speed) {
            return MSecPerStepFast(pos);
        } else {
            return MSecPerStepDefault(pos);
        }
    }
    bool InvertDirection() const { return invert_direction_; }

private:
    int MSecPerStepDefault(double pos) const {
        int msec(0);
        for (int idx = 0; idx < (int)msec_per_step_constants_default_.size(); ++idx) {
            msec += static_cast<int>(std::round(msec_per_step_constants_default_.at(idx) * std::pow(pos, idx)));
        }
        return std::max(10, msec);
    }
    int MSecPerStepFast(double pos) const {
        int msec(0);
        for (int idx = 0; idx < (int)msec_per_step_constants_fast_.size(); ++idx) {
            msec += static_cast<int>(std::round(msec_per_step_constants_fast_.at(idx) * std::pow(pos, idx)));
        }
        return std::max(1, msec);
    }

    t7::DIO full_or_half_step_;
    t7::DIO direction_;
    t7::DIO preset_;
    t7::PULSEOUT pulse_;
    double pulse_fraction_ = 0.5;
    int low_to_high_transition_count_ = 0;
    int high_to_low_transition_count_ = 100;
    int n_pulses_per_step_ = 10; // this is the value for the profile and the cuve
    double distance_per_step_ = 0.1; // this is the value for the profile and the cuve
    double offset_ = 0.0;
    std::vector<double> msec_per_step_constants_default_;
    std::vector<double> msec_per_step_constants_fast_;
    bool invert_direction_ = false;
};

#endif
