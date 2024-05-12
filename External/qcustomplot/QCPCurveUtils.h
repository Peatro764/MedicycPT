#ifndef EXTERNAL_QCPCURVEUTILS_H
#define EXTERNAL_QCPCURVEUTILS_H

#include "qcustomplot.h"

#include <vector>

namespace qcpcurveutils {
    QCPCurveDataContainer Discretize(const QCPCurveDataContainer& curve, const double& max_error, const double& err_key_scaling);
    void ScaleValueAxis(QCPCurveDataContainer& curve, const double& scale_factor);
    void SetMaxValue(QCPCurveDataContainer& curve, const double& max_value);
    QCPCurveDataContainer CutAt(QCPCurveDataContainer& curve, const double &cut_level, bool keep_above);
    QCPCurveDataContainer StepCurve(const QCPCurveDataContainer& curve, const double& min_step_height);
    double MinValue(QCPCurveDataContainer& data);
    double MaxValue(QCPCurveDataContainer& data);
    std::vector<double> ValuesInKeyRange(const QCPCurveDataContainer& curve, int idx1, int idx2);
    QCPCurveData MinElement(QCPCurveDataContainer& data);
    QCPCurveData MinElement(QCPCurveDataContainer& data, int start_idx, int end_idx);
    QCPCurveData MaxElement(QCPCurveDataContainer& data);
    QCPCurveData MaxElement(QCPCurveDataContainer& data, int start_idx, int end_idx);
    QCPCurveDataContainer Smooth(const QCPCurveDataContainer& data, int elements);
    double ValueAtKey(const QCPCurveDataContainer& data, double key);
    double LeftMostKeyForValue(const QCPCurveDataContainer& data, double value);
    double RightMostKeyForValue(const QCPCurveDataContainer& data, double value);
    bool AlmostEqual(double a, double b, double maxRelDiff);
}

 bool operator==(const QCPCurveData &c1, const QCPCurveData &c2);
 bool operator!=(const QCPCurveData &c1, const QCPCurveData &c2);
 bool operator==(const QCPCurveDataContainer &c1, const QCPCurveDataContainer &c2);
 bool operator!=(const QCPCurveDataContainer &c1, const QCPCurveDataContainer &c2);

#endif
