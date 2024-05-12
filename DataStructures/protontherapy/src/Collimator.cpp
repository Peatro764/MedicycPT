#include "Collimator.h"

#include <QDebug>
#include <QTextStream>
#include <QPoint>

#include "Coordinate.h"

QString Collimator::program_type = "COLLIMATEUR";

Collimator::Collimator(int dossier, const Patient& patient, const std::vector<Coordinate>& internal_edges)
    : dossier_(dossier),
      patient_(patient),
      internal_edges_(internal_edges) {}

Collimator::Collimator(int dossier, const Patient& patient, const QString& internal_edges_path_format)
    : dossier_(dossier),
      patient_(patient) {
    ParseInternalEdgesPathFormat(internal_edges_path_format);
}

Collimator::Collimator(QTextStream& stream) {
    ParseStream(stream);
}

Collimator::Collimator() :
    dossier_(0),
    internal_edges_(std::vector<Coordinate>()) {}

bool Collimator::IsValid() const {
    return (!internal_edges_.empty() &&
            dossier_ > 0 &&
            !(patient_ == Patient()));
}

void Collimator::SetFirstName(QString first_name) {
    patient_ = Patient(first_name, patient_.GetLastName());
}

void Collimator::SetLastName(QString last_name) {
    patient_ = Patient(patient_.GetFirstName(), last_name);
}

void Collimator::ParseInternalEdgesPathFormat(const QString& path_format) {
    internal_edges_.clear();
    QString without_braces(path_format);
    without_braces.remove("(").remove(")");
    QStringList splitted = without_braces.trimmed().split(",");
    if (splitted.size() % 2) {
        qWarning() << "Collimator::ParseInternalEdgesPathFormat The string should have pairs of coordinates, but its size is odd";
        return;
    }

    for (int idx = 0; idx < (int)(splitted.size() / 2); ++idx) {
        bool okX(false);
        bool okY(false);
        internal_edges_.push_back(Coordinate(splitted.at(2*idx).toDouble(&okX), splitted.at(2*idx + 1).toDouble(&okY)));
        if (!okX || !okY) {
            qWarning() << "Collimator::ParseInternalEdgesPathFormat String has wrong format";
            internal_edges_.clear();
            return;
        }
    }
}

void Collimator::ParseStream(QTextStream& stream) {
    QString line;
    QString first_name("");
    QString last_name("");

    do {
        line = stream.readLine();
        line = line.remove("\"");
        QStringList words(line.split(","));
        qDebug() << "Size words: " << words.size();
        if (words.size() == 5) {
            bool okDossier(false);
            dossier_ = words.at(0).trimmed().toInt(&okDossier);
            if (!okDossier) {
                dossier_ = 0;
                qWarning() << "Collimator::ParseStream A non valid dossier number was found: " + words.at(0);
                return;
            }
            last_name = words.at(1).trimmed();
            first_name = words.at(2).trimmed();
        } else if (words.size() == 2) {
            bool okX(false);
            bool okY(false);
            Coordinate coordinate(words.at(0).toFloat(&okX), words.at(1).toFloat(&okY));
            if (okX && okY) {
                internal_edges_.push_back(coordinate);
            } else {
                internal_edges_.clear();
                qWarning() << "Collimator::ParseStream A non valid coordinate was found: " + line;
                return;
            }
        } else {
            qWarning() << "Collimator::ParseStream Ignoring line with wrong number of words: " << line;
        }
    } while(!line.isNull());

    patient_ = Patient(first_name, last_name);
}

QString Collimator::InternalEdgesPathFormat() const {
    QString path("");
    for (auto c : internal_edges_) {
        QString prefix(path.isEmpty() ? "" : ",");
        path.append(prefix + "(" + QString::number(c.x(), 'f', 4) + "," + QString::number(c.y(), 'f', 4) + ")");
    }
    path.prepend("(").append(")");
    return path;
}

QPolygon Collimator::InternalEdgesPylogonFormat() const {
    // micrometer
    QPolygon poly;
    for (auto c : internal_edges_) {
        poly << QPoint(1000.0 * c.x(), 1000.0 * c.y());
    }
    return poly;
}
