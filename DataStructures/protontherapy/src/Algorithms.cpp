#include "Algorithms.h"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <QDebug>

namespace algorithms {

double CalculateIntegratedDose(double doserate_to_H_alpha, double doserate_to_H_beta, double sv_per_coulomb,
                               double charge, double pat_debit) {
    if (pat_debit < 0.0001) {
        qDebug() << "CalculateIntegratedDose pat_debit out of range: " << pat_debit;
        return 0.0;
    }

    if (charge < 1.0e-14) {
        qDebug() << "CalculateIntegratedDose charge out of range: " << charge;
        return 0.0;
    }

    if (sv_per_coulomb < 0.1) {
        qDebug() << "CalculateIntegratedDose sv per coulomb out of range: " << sv_per_coulomb;
        return 0.0;
    }

    if (doserate_to_H_alpha < 1.0e-15) {
        qDebug() << "CalculateIntegratedDose doserate_to_H_alpha out of range: " << doserate_to_H_alpha;
        return 0.0;
    }

    const double HoverD = doserate_to_H_alpha * pow(pat_debit, doserate_to_H_beta);
    return (charge * sv_per_coulomb / HoverD);
}

double CalculateDebit(double H_to_doserate_alpha, double H_to_doserate_beta,
                      double charge, double dose, double ref_debit, double tdj_debit) {
    if (tdj_debit < 0.0001 || ref_debit < 0.0001) {
        qDebug() << "CalculateDebit tdj_debit and/or ref_debit out of range: tdj_debit " << tdj_debit << " ref_debit " << ref_debit;
        return 0.0;
    }

    if (charge < 1.0e-14) {
        qDebug() << "CalculateDebit charge out of range: " << charge;
        return 0.0;
    }

    if (H_to_doserate_alpha < 1.0e-15) {
        qDebug() << "CalculateDebit H_to_doserate_alpha out of range: " << H_to_doserate_alpha;
        return 0.0;
    }

    if (dose < 0.001) {
        qDebug() << "CalculateDebit dose out of range: " << dose;
        return 0.0;
    }

    const double corr = (charge / dose);// * (ref_debit / tdj_debit);
    const double debit = H_to_doserate_alpha * pow(corr, H_to_doserate_beta);
    return debit;
}

double CalculateHoverD(double charge, double dose, double ref_debit, double tdj_debit) {
    if (charge < 1.0e-14) {
        qDebug() << "CalculateHoverD charge out of range: " << charge;
        return 0.0;
    }

    if (dose < 0.1) {
        qDebug() << "CalculateHoverD dose out of range: " << dose;
        return 0.0;
    }
    return (charge * ref_debit / (dose * tdj_debit));
}


}
