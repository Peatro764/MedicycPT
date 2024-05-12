#ifndef QUALITYASSURANCE_MEASUREMENTCURRENTS_H
#define QUALITYASSURANCE_MEASUREMENTCURRENTS_H

#include <QDateTime>

class MeasurementCurrents {
public:
    MeasurementCurrents() {}
    MeasurementCurrents(QDateTime timestamp, double stripper, double cf9, double chambre1, double chambre2, double diode)
        : timestamp_(timestamp), stripper_(stripper), cf9_(cf9), chambre1_(chambre1), chambre2_(chambre2), diode_(diode) {}
    QDateTime timestamp() const { return timestamp_; }
    double stripper() const { return stripper_; }
    double cf9() const { return cf9_; }
    double chambre1() const { return chambre1_; }
    double chambre2() const { return chambre2_; }
    double diode() const { return diode_; }

private:
    friend bool operator==(const MeasurementCurrents& c1, const MeasurementCurrents& c2);
    friend bool operator!=(const MeasurementCurrents& c1, const MeasurementCurrents& c2);

    QDateTime timestamp_ = QDateTime();
    double stripper_ = 0.0;
    double cf9_ = 0.0;
    double chambre1_ = 0.0;
    double chambre2_ = 0.0;
    double diode_ = 0.0;
};

#endif
