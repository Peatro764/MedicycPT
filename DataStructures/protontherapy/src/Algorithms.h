#ifndef ALGORITHMS_H
#define ALGORITHMS_H

namespace algorithms {
    double CalculateIntegratedDose(double doserate_to_H_alpha, double doserate_to_H_beta, double sv_per_coulomb,
                                   double charge, double pat_debit);
    double CalculateDebit(double H_to_doserate_alpha, double H_to_doserate_beta,
                          double charge, double dose, double ref_debit, double tdj_debit);
    double CalculateHoverD(double charge, double dose, double ref_debit, double tdj_debit);
}

#endif

