#ifndef PROTONTHERAPIE_CHAMBRE_H
#define PROTONTHERAPIE_CHAMBRE_H

#include <QString>
#include <iostream>
#include <QDateTime>

class Chambre {
public:
    Chambre(int id, QDateTime timestamp, QString name, double nd, double kqq0, double yoffset, double zoffset);
    Chambre();
    int id() const { return id_; }
    QDateTime timestamp() const { return timestamp_; }
    QString name() const { return name_; }
    double nd() const { return nd_; }
    double kqq0() const { return kqq0_; }
    double yoffset() const { return yoffset_; }
    double zoffset() const { return zoffset_; }
private:
    int id_;
    QDateTime timestamp_;
    QString name_;
    double nd_;
    double kqq0_;
    double yoffset_;
    double zoffset_;
};

std::ostream& operator<< (std::ostream &out, const Chambre &chambre);
bool operator==(const Chambre &c1, const Chambre& c2);
bool operator!=(const Chambre &c1, const Chambre& c2);

#endif // PROTONTHERAPIE_CHAMBRE_H
