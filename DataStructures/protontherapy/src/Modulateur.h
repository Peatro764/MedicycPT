#ifndef PROTONTHERAPIE_MODULATEUR_H
#define PROTONTHERAPIE_MODULATEUR_H

#include <iostream>
#include <QString>
#include <map>

class Modulateur {
public:

    Modulateur(const QString& id,
               const double& mod_100, const double& mod_98, const double& parcours,
               const int& um_degradeur, const std::map<int, double>& um_plexi_weight,
               const bool& available, QString opt_alg, QString data_set, int n_sectors,
               double mod_step, double input_parcours, double input_mod, double decalage,
               double conv_criteria);
    Modulateur(const QString& id,
               const double& mod_100, const double& mod_98, const double& parcours,
               const int& um_degradeur, const std::map<int, double>& um_plexi_weight,
               const bool& available);

    Modulateur();
    QString id() const { return id_; }
    static std::map<int, double> GetSectorAngularSizes(const std::map<int, double>& weights, int n_sectors);
    std::map<int, double> GetAngularSizes() const;

    int steps() const;
    int RangeShift() const;
    const std::map<int, double>& um_plexi_weight() const { return um_plexi_weight_; }
    double mod100() const { return mod_100_; }
    double mod98() const { return mod_98_; }
    double parcours() const { return parcours_; }
    bool available() const { return available_; }
    QString opt_alg() const { return opt_alg_; }
    QString data_set() const { return data_set_; }
    int n_sectors() const { return n_sectors_; }
    double mod_step() const { return mod_step_; }
    double input_parcours() const { return input_parcours_; }
    double input_mod() const { return input_mod_; }
    double decalage() const { return decalage_; }
    double conv_criteria() const { return conv_criteria_; }

    static QString program_type;

private:
    QString id_;
    double mod_100_;
    double mod_98_;
    double parcours_;
    int um_degradeur_;
    std::map<int, double> um_plexi_weight_;
    bool available_;

    QString opt_alg_;
    QString data_set_;
    int n_sectors_;
    double mod_step_;
    double input_parcours_;
    double input_mod_;
    double decalage_;
    double conv_criteria_;
};

std::ostream& operator<< (std::ostream &out, const Modulateur &modulateur);
bool operator==(const Modulateur &m1, const Modulateur& m2);
bool operator!=(const Modulateur &m1, const Modulateur& m2);

#endif // PROTONTHERAPIE_MODULATEUR_H
