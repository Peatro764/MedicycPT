#ifndef BEAMCHARACTERISTICS_POSITIONSENSOR_H
#define BEAMCHARACTERISTICS_POSITIONSENSOR_H

#include "ISensor.h"

#include <memory>

#include "SensorConfig.h"
#include "LabJack.h"

class PositionSensor : public QObject, public ISensor
{
    Q_OBJECT
    Q_INTERFACES(ISensor)

public:
    PositionSensor(std::shared_ptr<LabJack> labjack, const SensorConfig& config);
    ~PositionSensor();
    void Configure();
    void ConfigureRapidScan(int samples, int rate);
    void DisableRapidScan();
    double ReadRapidScan();
    double RawValue();
    double InterpretedValue();
    double InterpretedValue(int n_measurements);
    double InterpretedValue(int n_measurements, double n_sigma_filter);
    std::vector<double> MultipleRawValue(int n);
     t7::AIN ch() { return config_.Ch(); }

private:
    std::shared_ptr<LabJack> labjack_;
    SensorConfig config_;
    bool configured_ = false;
};

#endif