#include "Chambre.h"
#include "Calc.h"

 Chambre::Chambre(int id, QDateTime timestamp, QString name, double nd, double kqq0, double yoffset, double zoffset)
     : id_(id), timestamp_(timestamp), name_(name), nd_(nd), kqq0_(kqq0), yoffset_(yoffset), zoffset_(zoffset) {}

 Chambre::Chambre() : id_(-1), timestamp_(QDateTime()), name_(""), nd_(0.0), kqq0_(0.0), yoffset_(0.0), zoffset_(0.0) {}

 std::ostream& operator<< (std::ostream &out, const Chambre &chambre) {
     out << "name " << chambre.name().toStdString() << " "
         << "timestamp " << chambre.timestamp().toString().toStdString() << " "
         << "nd " << chambre.nd() << " "
         << "kqq0 " << chambre.kqq0() << " "
         << "yoffset " << chambre.yoffset() << " "
         << "zoffset " << chambre.zoffset() << std::endl;
     return out;
 }

 bool operator==(const Chambre &c1, const Chambre& c2) {
     return (c1.id() == c2.id() &&
             c1.timestamp() == c2.timestamp() &&
             c1.name() == c2.name() &&
             calc::AlmostEqual(c1.kqq0(), c2.kqq0(), 0.0001) &&
             calc::AlmostEqual(c1.nd(), c2.nd(), 0.0001) &&
             calc::AlmostEqual(c1.yoffset(), c2.yoffset(), 0.0001) &&
             calc::AlmostEqual(c1.zoffset(), c2.zoffset(), 0.0001));
 }

 bool operator!=(const Chambre &c1, const Chambre& c2) {
     return !(c1 == c2);
 }


