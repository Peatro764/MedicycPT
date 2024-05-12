#include "Degradeur.h"
#include "Calc.h"

Degradeur::Degradeur(const QString& id, const int& um_plexi, const bool& available)
    : id_(id), um_plexi_(um_plexi), available_(available) {}

Degradeur::Degradeur()
    : id_("None"), um_plexi_(0), available_(false) {}


double Degradeur::mm_plexi() const {
    return ((double)um_plexi_ / 1000.0);
}

DegradeurSet::DegradeurSet(const Degradeur &degradeur) {
    degradeurs_.push_back(degradeur);
}

DegradeurSet::DegradeurSet() {
    degradeurs_.push_back(Degradeur());
}

DegradeurSet::DegradeurSet(const Degradeur &degradeur1, const Degradeur &degradeur2) {
    degradeurs_.push_back(degradeur1);
    degradeurs_.push_back(degradeur2);
}

int DegradeurSet::um_plexi_total() const {
    int um_plexi_total(0.0);
    for (auto deg : degradeurs_) {
        um_plexi_total += deg.um_plexi();
    }
    return um_plexi_total;
}

QStringList DegradeurSet::mm_plexis() const {
    QStringList mm_plexis;
    for (auto d : degradeurs_) {
        mm_plexis << QString::number((double)d.um_plexi() / 1000.0, 'f', 1);
    }
    return mm_plexis;
}

QStringList DegradeurSet::um_plexis() const {
    QStringList um_plexis;
    for (auto d : degradeurs_) {
        um_plexis << QString::number(d.um_plexi());
    }
    return um_plexis;
}

QStringList DegradeurSet::ids() const {
    QStringList ids;
    for (auto d : degradeurs_) {
        ids << d.id();
    }
    return ids;
}

std::vector<Degradeur> DegradeurSet::GetIndividualDegradeurs() const {
    return degradeurs_;
}

std::ostream& operator<< (std::ostream &out, const Degradeur &degradeur) {
    out << "id " << degradeur.id().toStdString() << " "
        << "um_plexi " << degradeur.um_plexi() << " "
        << "available " << degradeur.available() << std::endl;
    return out;
}

bool operator==(const Degradeur &d1, const Degradeur& d2) {
    return (d1.id() == d2.id() &&
            d1.um_plexi() == d2.um_plexi());
}

bool operator==(const DegradeurSet &d1, const DegradeurSet& d2) {
    return (d1.GetIndividualDegradeurs() == d2.GetIndividualDegradeurs());
}



