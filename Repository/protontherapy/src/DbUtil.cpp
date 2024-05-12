#include "DbUtil.h"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <QDebug>

namespace dbutil {

    QStringList DecodeDegradeurArray(QString encDegrId) {
        QString arrayWithoutBraces(encDegrId.remove("{").remove("}"));
        return arrayWithoutBraces.split(",", QString::SkipEmptyParts);
    }

    QString EncodeDegradeurArray(const std::vector<Degradeur>& degradeurs) {
        QString encArray("{");
        for (auto deg : degradeurs) {
            encArray == "{" ? encArray.append(deg.id()) : encArray.append("," + deg.id());
        }
        encArray.append("}");
        return encArray;
    }

    std::vector<double> DecodeDoubleArray(QString array) {
        QString arrayWithoutBraces(array.remove("{").remove("}"));
        QStringList doubleStringList(arrayWithoutBraces.split(",", QString::SkipEmptyParts));
        std::vector<double> doubleVector;
        for (QString val : doubleStringList) {
            doubleVector.push_back(val.toDouble());
        }
        return doubleVector;
    }

    QString EncodeDoubleArray(const std::vector<double>& values) {
        QString encArray("{");
        for (auto val : values) {
            encArray == "{" ? encArray.append(QString::number(val, 'f', 6)) : encArray.append("," + QString::number(val, 'f', 6));
        }
        encArray.append("}");
        return encArray;
    }

    std::vector<int> DecodeIntArray(QString array) {
        QString arrayWithoutBraces(array.remove("{").remove("}"));
        QStringList intStringList(arrayWithoutBraces.split(",", QString::SkipEmptyParts));
        std::vector<int> intVector;
        for (QString val : intStringList) {
            intVector.push_back(val.toInt());
        }
        return intVector;
    }

    QString EncodeIntArray(const std::vector<int>& values) {
        QString encArray("{");
        for (auto val : values) {
            encArray == "{" ? encArray.append(QString::number(val)) : encArray.append("," + QString::number(val));
        }
        encArray.append("}");
        return encArray;
    }

    std::vector<DepthDose> DecodeDepthDoseArray(QString array) {
        std::vector<DepthDose> depth_doses;
        QString without_braces(array.remove("{").remove("}").remove("(").remove(")").remove("\""));
        QStringList splitted = without_braces.split(",");
        if (splitted.size() % 2) {
            qWarning() << "dbutil::DecodeDepthDoseArray The string should have pairs of coordinates, but its size is odd";
            return depth_doses;
        }

        for (int idx = 0; idx < (int)(splitted.size() / 2); ++idx) {
            bool okDepth(false);
            bool okDose(false);
            depth_doses.push_back(DepthDose(splitted.at(2*idx).toDouble(&okDepth), splitted.at(2*idx + 1).toDouble(&okDose)));
            if (!okDepth || !okDose) {
                qWarning() << "dbutil::DecodeDepthDoseArray String has wrong format";
                depth_doses.clear();
                return depth_doses;
            }
        }
        return depth_doses;
    }

    std::map<int, double> DecodeIntDoubleMap(QString encoded_map) {
        std::map<int, double> decoded_map;
        QString without_braces(encoded_map.remove("{").remove("}").remove("(").remove(")").remove("\""));
        QStringList splitted = without_braces.split(",");
        if (splitted.size() % 2) {
            qWarning() << "dbutil::DecodeIntDoubleMap The string should have pairs of (int, double), but its size is odd";
            return decoded_map;
        }

        for (int idx = 0; idx < (int)(splitted.size() / 2); ++idx) {
            bool okUMPlexi(false);
            bool okWeight(false);
            decoded_map[splitted.at(2*idx).toInt(&okUMPlexi)] = splitted.at(2*idx + 1).toDouble(&okWeight);
            if (!okUMPlexi || !okWeight) {
                qWarning() << "dbutil::DecodeIntDoubleMap String has wrong format";
                decoded_map.clear();
                return decoded_map;
            }
        }
        return decoded_map;
    }


    QString EncodeIntDoubleMap(const std::map<int, double>& map) {
        auto CreatePoint = [] (const std::pair<const int, double>& p)
                -> QString { return "\"(" + QString::number(p.first) + "," + QString::number(p.second, 'f', 3) + ")\""; };

        QString encMap("{");
        for (auto item : map)  {
            encMap == "{" ? encMap.append(CreatePoint(item)) : encMap.append("," + CreatePoint(item));
        }
        encMap.append("}");
        return encMap;
    }

    QString QCPCurveDataContainerToPSQLPath(const QCPCurveDataContainer& data) {
        QString psql_path("");
        for (auto it = data.constBegin(); it < data.constEnd(); ++it) {
            QString prefix(psql_path.isEmpty() ? "" : ",");
            psql_path.append(prefix + "(" + QString::number(it->mainKey() , 'f', 4) + "," +
                        QString::number(it->mainValue(), 'f', 4) + ")");
        }
        psql_path.prepend("(").append(")");
        return psql_path;
    }

    QCPCurveDataContainer PSQLPathToQCPCurveDataContainer(const QString& psql_path) {
        QString without_braces(psql_path);
        without_braces.remove("(").remove(")");
        QStringList splitted = without_braces.trimmed().split(",");
        if (splitted.size() % 2) {
            qWarning() << "DbUtil::PSQLPathToQCPCurveDataContainer The string should have pairs of coordinates, but its size is odd";
            throw std::runtime_error("The psql path could not be deserialized, non even number of entries");
        }

        QCPCurveDataContainer data;
        for (int idx = 0; idx < (int)(splitted.size() / 2); ++idx) {
            bool okX(false);
            bool okY(false);
            data.add(QCPCurveData(idx, splitted.at(2*idx).toDouble(&okX), splitted.at(2*idx + 1).toDouble(&okY)));
            if (!okX || !okY) {
                qWarning() << "DbUtil::PSQLPathToQCPCurveDataContainer String has wrong format";
                throw std::runtime_error("The psql path could not be deserialized, non float content");
            }
        }
        return data;
    }

    QCPCurveData PSQLPointToQCPCurveData(const QString& psql_point) {
        QString without_braces(psql_point);
        without_braces.remove("(").remove(")");
        QStringList splitted = without_braces.trimmed().split(",");
        if (splitted.size() != 2) {
            qWarning() << "DbUtil::PSQLPointToQCPCurveData The string should have a pair of coordinates, but its size is not 2";
            throw std::runtime_error("The psql point could not be deserialized, wrong number of floats");
        }

        bool okKey(false);
        bool okValue(false);
        QCPCurveData data(0, splitted.at(0).toDouble(&okKey), splitted.at(1).toDouble(&okValue));
        if (!okKey || !okValue) {
            qWarning() << "DbUtil::PSQLPointToQCPCurveData String has wrong format";
            throw std::runtime_error("The psql point could not be deserialized, non float content");
        }

        return data;
    }

    QString QCPCurveDataToPSQLPoint(const QCPCurveData& data) {
        QString psql_point(QString("(") + QString::number(data.mainKey(), 'f', 4) + "," +
                           QString::number(data.mainValue(), 'f', 4) + ")");
        return psql_point;
    }

}

