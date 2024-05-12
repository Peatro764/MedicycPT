#ifndef PROTONTHERAPIE_BALISECALIBRATION_H
#define PROTONTHERAPIE_BALISECALIBRATION_H

#include <QDateTime>

class BaliseCalibration {

public:
    BaliseCalibration();
    BaliseCalibration(QDateTime timestamp,
                      double svPerCoulomb,
                      double doseRateToH_alpha,
                      double doseRateToH_beta,
                      double HToDoseRate_alpha,
                      double HToDoseRate_beta);
    ~BaliseCalibration();
    QDateTime timestamp() const { return timestamp_; }
    double svPerCoulomb() const { return svPerCoulomb_; }
    double DoseRateToH_alpha() const { return doseRateToH_alpha_; }
    double DoseRateToH_beta() const { return doseRateToH_beta_; }
    double HToDoseRate_alpha() const { return HToDoseRate_alpha_; }
    double HToDoseRate_beta() const { return HToDoseRate_beta_; }

private:
    QDateTime timestamp_;
    double svPerCoulomb_ = 1.0;
    double doseRateToH_alpha_ = 1.0;
    double doseRateToH_beta_ = 1.0;
    double HToDoseRate_alpha_ = 1.0;
    double HToDoseRate_beta_ = 1.0;
};

 bool operator==(const BaliseCalibration &b1, const BaliseCalibration& b2);
 bool operator!=(const BaliseCalibration &b1, const BaliseCalibration& b2);

#endif
