#ifndef BEAMCHARACTERISTICS_SENSORCONFIG_H
#define BEAMCHARACTERISTICS_SENSORCONFIG_H

#include "LabJackChannels.h"

enum class SensorType { DIODE, CHAMBRE, POSITION_X, POSITION_Y, POSITION_Z };

class SensorConfig {

public:
    SensorConfig(int single_ended_channel,
                 double range,
                 int resolution_index,
                 int settling_us,
                 t7::AIN ch,
                 double unit_per_volt,
                 double unit_offset);
    int SingleEndedChannel() const { return single_ended_channel_; }
    double Range() const { return range_; }
    int ResolutionIndex() const { return resolution_index_; }
    int SettlingUs() const { return settling_us_; }
    t7::AIN Ch() const { return ch_; }
    double UnitPerVolt() const { return unit_per_volt_; }
    double UnitOffset() const { return unit_offset_; }

private:
    int single_ended_channel_ = 199;
    double range_ = 10.0;
    int resolution_index_ = 8;
    int settling_us_ = 0;
    t7::AIN ch_;
    double unit_per_volt_ = 1.0;
    double unit_offset_ = 0.0;
};

#endif
