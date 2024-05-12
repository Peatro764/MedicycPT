#ifndef REPOSITORY_DBUTIL_H
#define REPOSITORY_DBUTIL_H

#include <vector>
#include <QStringList>
#include <map>

#include "Degradeur.h"
#include "DepthDose.h"
#include "DepthDoseCurve.h"
#include "Modulateur.h"
#include "Degradeur.h"
#include "qcustomplot.h"

namespace dbutil {
    QStringList DecodeDegradeurArray(QString encDegrId);
    QString EncodeDegradeurArray(const std::vector<Degradeur>& degradeurs);
    std::vector<double> DecodeDoubleArray(QString array);
    QString EncodeDoubleArray(const std::vector<double>& values);
    std::vector<int> DecodeIntArray(QString array);
    QString EncodeIntArray(const std::vector<int>& values);
    std::vector<DepthDose> DecodeDepthDoseArray(QString array);
    std::map<int, double> DecodeIntDoubleMap(QString map);
    QString EncodeIntDoubleMap(const std::map<int, double>& map);
    QString QCPCurveDataContainerToPSQLPath(const QCPCurveDataContainer& data);
    QCPCurveDataContainer PSQLPathToQCPCurveDataContainer(const QString& psql_path);
    QCPCurveData PSQLPointToQCPCurveData(const QString& psql_point);
    QString QCPCurveDataToPSQLPoint(const QCPCurveData& data);
}

#endif // DBUTIL_H
