#include "SensorConfig.h"

#include <QDebug>

SensorConfig::SensorConfig(int single_ended_channel,
                           double range,
                           int resolution_index,
                           int settling_us,
                           t7::AIN ch,
                           double unit_per_volt,
                           double unit_offset)
    : single_ended_channel_(single_ended_channel),
      range_(range),
      resolution_index_(resolution_index),
      settling_us_(settling_us),
      ch_(ch),
      unit_per_volt_(unit_per_volt),
      unit_offset_(unit_offset) {}
