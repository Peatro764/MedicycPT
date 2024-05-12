#ifndef BEAMCHARACTERISTICS_ACTUATOR_H
#define BEAMCHARACTERISTICS_ACTUATOR_H

#include <memory>

#include "LabJackChannels.h"
#include "LabJack.h"

enum class ActuatorType { AOUT0, AOUT1 };

class Actuator : public QObject
{
    Q_OBJECT

public:
    Actuator(std::shared_ptr<LabJack> labjack, t7::AOUT ch);
    ~Actuator();
    void Write(double voltage);

private:
    std::shared_ptr<LabJack> labjack_;
    t7::AOUT ch_;
};

#endif
