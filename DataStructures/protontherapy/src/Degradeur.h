#ifndef PROTONTHERAPIE_DEGRADEUR_H
#define PROTONTHERAPIE_DEGRADEUR_H

#include <iostream>
#include <QString>
#include <QStringList>
#include <vector>

class Degradeur {
public:
    Degradeur(const QString& id, const int& um_plexi, const bool& available);
    Degradeur();
    QString id() const { return id_; }
    double mm_plexi() const;
    int um_plexi() const { return um_plexi_; }
    bool available() const { return available_; }

private:
    QString id_;
    int um_plexi_;
    bool available_;
};

class DegradeurSet {
public:
    DegradeurSet();
    DegradeurSet(const Degradeur& degradeur);
    DegradeurSet(const Degradeur& degradeur1, const Degradeur& degradeur2);
    int um_plexi_total() const;
    QStringList ids() const;
    QStringList mm_plexis() const;
    QStringList um_plexis() const;
    std::vector<Degradeur> GetIndividualDegradeurs() const;
    bool single() const { return ((int)degradeurs_.size()) == 1; }

private:
    std::vector<Degradeur> degradeurs_;
};


std::ostream& operator<< (std::ostream &out, const Degradeur &degradeur);
bool operator==(const Degradeur &d1, const Degradeur& d2);
bool operator==(const DegradeurSet &d1, const DegradeurSet& d2);


#endif // PROTONTHERAPIE_DEGRADEUR_H
