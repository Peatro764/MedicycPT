#include "Debit.h"
#include "Calc.h"

Debit::Debit(const double &mean, const double &std_dev,
             const QDateTime &timestamp, const bool &monte_carlo, const Chambre& chambre)
    : mean_(mean), std_dev_(std_dev), timestamp_(timestamp), monte_carlo_(monte_carlo), chambre_(chambre) {}

bool operator==(const Debit &d1, const Debit& d2) {
    return (calc::AlmostEqual(d1.mean(), d2.mean(), 0.0001) &&
            calc::AlmostEqual(d1.std_dev(), d2.std_dev(), 0.0001) &&
            d1.timestamp() == d2.timestamp() &&
            d1.monte_carlo() == d2.monte_carlo() &&
            d1.chambre() == d2.chambre());
}



