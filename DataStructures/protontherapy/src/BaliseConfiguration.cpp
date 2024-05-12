#include "BaliseCalibration.h"

#include "Calc.h"

BaliseCalibration::BaliseCalibration() {}

BaliseCalibration::BaliseCalibration(QDateTime timestamp,
                                     double svPerCoulomb,
                                     double doseRateToH_alpha,
                                     double doseRateToH_beta,
                                     double HToDoseRate_alpha,
                                     double HToDoseRate_beta)
    : timestamp_(timestamp),
      svPerCoulomb_(svPerCoulomb),
      doseRateToH_alpha_(doseRateToH_alpha),
      doseRateToH_beta_(doseRateToH_beta),
      HToDoseRate_alpha_(HToDoseRate_alpha),
      HToDoseRate_beta_(HToDoseRate_beta) {}

BaliseCalibration::~BaliseCalibration() {}

bool operator==(const BaliseCalibration &b1, const BaliseCalibration& b2) {
    return (b1.timestamp() == b2.timestamp() &&
            calc::AlmostEqual(b1.svPerCoulomb(), b2.svPerCoulomb(), 0.0001) &&
            calc::AlmostEqual(b1.DoseRateToH_alpha(), b2.DoseRateToH_alpha(), 0.0001) &&
            calc::AlmostEqual(b1.DoseRateToH_beta(), b2.DoseRateToH_beta(), 0.0001) &&
            calc::AlmostEqual(b1.HToDoseRate_alpha(), b2.HToDoseRate_alpha(), 0.0001) &&
            calc::AlmostEqual(b1.HToDoseRate_beta(), b2.HToDoseRate_beta(), 0.0001));
}

bool operator!=(const BaliseCalibration &b1, const BaliseCalibration& b2) {
    return !(b1 == b2);
}


