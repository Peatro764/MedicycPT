#ifndef PROTONTHERAPIE_DEBIT_H
#define PROTONTHERAPIE_DEBIT_H

#include <QDateTime>

#include "Chambre.h"

class Debit {
public:
    Debit(const double& mean, const double& std_dev,
          const QDateTime& timestamp, const bool& monte_carlo, const Chambre& chambre);

    double mean() const { return mean_; }
    double std_dev() const { return std_dev_; }
    QDateTime timestamp() const { return timestamp_; }
    bool monte_carlo() const { return monte_carlo_; }
    Chambre chambre() const { return chambre_; }
private:
    const double mean_;
    const double std_dev_;
    const QDateTime timestamp_;
    const bool monte_carlo_;
    const Chambre chambre_;
};

bool operator==(const Debit &d1, const Debit& d2);


#endif // PROTONTHERAPIE_DEBIT_H
