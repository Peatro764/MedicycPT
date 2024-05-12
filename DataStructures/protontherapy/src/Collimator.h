#ifndef COLLIMATOR_H
#define COLLIMATOR_H

#include <QSettings>
#include <QString>
#include <vector>
#include <QTextStream>
#include <QPolygon>

#include "Coordinate.h"
#include "Patient.h"

class Collimator
{

public:
    Collimator(int dossier, const Patient& patient, const std::vector<Coordinate>& internal_edges);
    Collimator(int dossier, const Patient& patient, const QString& internal_edges_path_format);
    Collimator(QTextStream &stream);
    Collimator();
    bool IsValid() const;
    int Dossier() const { return dossier_; }
    Patient patient() const { return patient_; }
    std::vector<Coordinate> InternalEdges() const { return internal_edges_; }
    QString InternalEdgesPathFormat() const;
    QPolygon InternalEdgesPylogonFormat() const;

    void SetDossier(int dossier) { dossier_ = dossier; }
    void SetLastName(QString last_name);
    void SetFirstName(QString first_name);

    static QString program_type;

private:
    void ParseInternalEdgesPathFormat(const QString& path_format);
    void ParseStream(QTextStream &stream);
    int dossier_ = 0;
    Patient patient_;
    std::vector<Coordinate> internal_edges_;
};

#endif // COLLIMATOR_H
