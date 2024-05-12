#ifndef PROTONTHERAPIE_LABJACK_H
#define PROTONTHERAPIE_LABJACK_H

#include <QObject>

#include "LabJackChannels.h"

using namespace t7;

class LabJack : public QObject
{
    Q_OBJECT
public:    
    LabJack();
    ~LabJack();

signals:

public slots:
    bool IsConnected() const { return connected_; }
    void Connect(QString ip);
    void ConnectUSB();
    void Disconnect();
    void LoadConstantsFile();
    void LoadConfigurationFile();
    void ReadConfiguration();

    void SetDefaultToCurrent();
    void SetCurrentToDefault();

    uint32_t ReadDIODirections();
    void ConfigureDIO(DIO ch, bool output);
    bool ReadDIO(DIO ch);
    void WriteDIO(DIO ch, bool high);

    void ConfigureRapidAINScan(AIN ch, int n_samples, int scan_rate);
    void DisableRapidAINScan(AIN ch);
    void ConfigureAIN(AIN ch, int single_ended_channel = 199, double range = 10.0, int resolution_index = 8, int settling_us = 0);
    double ReadAIN(AIN ch);
    void ReadAINS(AIN ch1, AIN ch2, double *value_ch1, double *value_ch2);
    std::vector<double> ReadAINS(AIN ch1, AIN ch2, int n_measurements);
    void MultiReadAIN(AIN ch, int n_times);
    double ReadRapidAINScan(AIN ch);

    void WriteAOUT(AOUT ch, double voltage);

    void ConfigureClock(CLOCK clock, int divisor = 8, int rollvalue = 10000);
    void StartClock(CLOCK clock);
    void StopClock(CLOCK clock);
    int ReadClock(CLOCK clock);

    void ConfigurePulseOut(PULSEOUT ch, CLOCK clock, int clockCount_highToLow, int clockCount_lowToHigh, int n_pulses);
    void StartPulseOut(PULSEOUT ch);
    void StopPulseOut(PULSEOUT ch);
    int ReadPulseOutCurrent(PULSEOUT ch);
    int ReadPulseOutTarget(PULSEOUT ch);
    void UpdatePulseOut(PULSEOUT ch, int pulses);
    void DisablePulseOut(PULSEOUT ch);

private slots:

private:
    int device_handle = 0;
    bool connected_ = false;

};


#endif
