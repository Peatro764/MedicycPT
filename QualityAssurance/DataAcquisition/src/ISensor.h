#ifndef BEAMCHARACTERISTICS_ISENSOR_H
#define BEAMCHARACTERISTICS_ISENSOR_H

#include <QObject>
#include <vector>

#include "LabJackChannels.h"

class ISensor {

public:
    virtual ~ISensor() {}
    virtual void Configure() = 0;
    virtual double RawValue() = 0;
    virtual void ConfigureRapidScan(int samples, int rate) = 0;
    virtual void DisableRapidScan() = 0;
    virtual double ReadRapidScan() = 0;
    virtual double InterpretedValue() = 0;
    virtual double InterpretedValue(int n_measurements) = 0;
    virtual double InterpretedValue(int n_measurements, double n_sigma_filter) = 0;
    virtual std::vector<double> MultipleRawValue(int n) = 0;
    virtual t7::AIN ch() = 0;

private:

};

Q_DECLARE_INTERFACE(ISensor, "ISensor")

#endif
