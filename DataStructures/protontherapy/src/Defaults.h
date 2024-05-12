#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <QString>

class Defaults
{
public:
    Defaults(QString chambre, double temperature, double pressure,
             double dref, double duration_factor, double stripper_response_correction, double seance_duration);

    QString GetChambre() { return chambre_; }
    double GetTemperature() { return temperature_; }
    double GetPressure() { return pressure_; }
    double GetDRef() { return dref_; }
    double GetDurationFactor() { return duration_factor_; }
    double GetStripperResponseCorrection() { return stripper_response_correction_; }
    double GetSeanceDuration() { return seance_duration_; }

private:
    QString chambre_;
    double temperature_;
    double pressure_;
    double dref_;
    double duration_factor_;
    double stripper_response_correction_;
    double seance_duration_;
};

#endif // DEFAULTS_H
