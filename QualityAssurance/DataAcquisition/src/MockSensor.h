#ifndef BEAMCHARACTERISTICS_MOCKSENSOR_H
#define BEAMCHARACTERISTICS_MOCKSENSOR_H

#include "ISensor.h"

#include <queue>

class MockSensor : public QObject, public ISensor
{
    Q_OBJECT
    Q_INTERFACES(ISensor)

public:
    MockSensor(std::queue<double> values);
    ~MockSensor();
    void Configure();
    double RawValue();
    double InterpretedValue();
    double InterpretedValue(int n_measurements);
    double InterpretedValue(int n_measurements, double n_sigma_filter);
    std::vector<double> MultipleRawValue(int n);

private:
    std::queue<double> values_;
};

#endif
