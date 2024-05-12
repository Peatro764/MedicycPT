#include "Modulateur.h"
#include "Calc.h"

QString Modulateur::program_type = "MODULATEUR";

Modulateur::Modulateur(const QString& id,
                        const double& mod_100, const double& mod_98, const double& parcours,
                        const int& um_degradeur, const std::map<int, double>& um_plexi_weight,
                        const bool& available, QString opt_alg, QString data_set, int n_sectors,
                        double mod_step, double input_parcours, double input_mod, double decalage,
                        double conv_criteria)
    : id_(id),
      mod_100_(mod_100), mod_98_(mod_98), parcours_(parcours),
      um_degradeur_(um_degradeur), um_plexi_weight_(um_plexi_weight),
      available_(available), opt_alg_(opt_alg), data_set_(data_set),
      n_sectors_(n_sectors), mod_step_(mod_step), input_parcours_(input_parcours),
      input_mod_(input_mod), decalage_(decalage), conv_criteria_(conv_criteria) {}

Modulateur::Modulateur(const QString& id,
                       const double& mod_100, const double& mod_98, const double& parcours,
                       const int& um_degradeur, const std::map<int, double>& um_plexi_weight,
                       const bool& available)
    : id_(id),
      mod_100_(mod_100), mod_98_(mod_98), parcours_(parcours),
      um_degradeur_(um_degradeur), um_plexi_weight_(um_plexi_weight),
      available_(available), opt_alg_("Unknown"), data_set_("Unknown"),
      n_sectors_(0), mod_step_(0.0), input_parcours_(0.0),
      input_mod_(0.0), decalage_(0.0), conv_criteria_(0.0) {}

Modulateur::Modulateur()
    : id_("None"), mod_100_(0.0), mod_98_(0.0), parcours_(0.0),
      um_degradeur_(0), available_(false), opt_alg_("Unknown"), data_set_("Unknown"),
      n_sectors_(0), mod_step_(0.0), input_parcours_(0.0),
      input_mod_(0.0), decalage_(0.0), conv_criteria_(0.0) {
    um_plexi_weight_[0] = 1.0;
}

int Modulateur::steps() const  {
    if (um_plexi_weight_.empty()) {
        return 0;
    } else {
       return (int)(um_plexi_weight_.size() - 1);
    }
}

int Modulateur::RangeShift() const {
    return um_degradeur_;
}

std::ostream& operator<< (std::ostream &out, const Modulateur &modulateur) {
     out << "id " << modulateur.id().toStdString() << " "
         << "mod100 " << modulateur.mod100() << " "
         << "mod98 " << modulateur.mod98() << " "
         << "parcours " << modulateur.parcours() << " "
         << "um_degradeur " << modulateur.RangeShift() << " "
         << "available " << modulateur.available() << std::endl;
     auto map = modulateur.um_plexi_weight();
     for (auto it = map.cbegin(); it != map.cend(); ++it) {
         out << "um plexi " << it->first << " weight " << it->second << std::endl;
     }
     return out;
 }

 bool operator==(const Modulateur &m1, const Modulateur& m2) {
     return (m1.id() == m2.id() &&
             calc::AlmostEqual(m1.mod100(), m2.mod100(), 0.001) &&
             calc::AlmostEqual(m1.mod98(), m2.mod98(), 0.001) &&
             calc::AlmostEqual(m1.parcours(), m2.parcours(), 0.001) &&
             m1.RangeShift() == m2.RangeShift() &&
             m1.um_plexi_weight() == m2.um_plexi_weight() &&
             m1.available() == m2.available() &&
             m1.opt_alg() == m2.opt_alg() &&
             m1.data_set() == m2.data_set() &&
             m1.n_sectors() == m2.n_sectors() &&
             calc::AlmostEqual(m1.mod_step(), m2.mod_step(), 0.0001) &&
             calc::AlmostEqual(m1.input_parcours(), m2.input_parcours(), 0.0001) &&
             calc::AlmostEqual(m1.input_mod(), m2.input_mod(), 0.0001) &&
             calc::AlmostEqual(m1.decalage(), m2.decalage(), 0.0001) &&
             calc::AlmostEqual(m1.conv_criteria(), m2.conv_criteria(), 0.0001));

 }

 bool operator!=(const Modulateur &m1, const Modulateur& m2) {
     return !(m1 == m2);
 }

std::map<int, double> Modulateur::GetAngularSizes() const {
    return Modulateur::GetSectorAngularSizes(um_plexi_weight(), n_sectors());
}


 std::map<int, double> Modulateur::GetSectorAngularSizes(const std::map<int, double> &weights, int n_sectors) {
     if (weights.empty()) {
         throw std::runtime_error("Cannot create a modulator without a single layer");
     }
     std::map<int, double> angular_size;
     double sum_weights(0.0);
     for (auto& w : weights) {
         sum_weights += w.second;
     }

     double sector_angle(360.0 / static_cast<double>(n_sectors));
     double rolling_sum_weights(0.0);
     for (auto it = weights.rbegin(); it != weights.rend(); ++it) {
         rolling_sum_weights += it->second;
         angular_size.emplace(it->first, (rolling_sum_weights / sum_weights) * sector_angle);
     }
     return angular_size;
 }
