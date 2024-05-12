#ifndef UTIL_CALC_H
#define UTIL_CALC_H

#include <vector>

namespace calc {
    double Mean(const std::vector<double> &data);
    double Median(std::vector<double> data);
    double StdDev(const std::vector<double> &data);

    // returns <a, b> as in y = a + b*x
    std::pair<double, double> LinearFit(const std::vector<double>& x, const std::vector<double>& y);
    double PearsonCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    double CenterOfGravity(const std::vector<double>& x, const std::vector<double>& y);
    std::vector<double> Square(std::vector<double> data);
    double TStatistic(const std::vector<double>& sample1, std::vector<double>& sample2);

    std::vector<double> Filter(const std::vector<double>& data, double n_stddev);
    bool AlmostEqual(double a, double b, double maxRelDiff);
    bool AlmostEqual(const std::vector<double> &a, const std::vector<double> &b, double maxRelDiff);

    double DoseToUM(const double& debit, const double& dose);
    double UMToDose(const double& debit, const int& um);

    double UM2MM(int um);
    int MM2UM(double mm);
}

#endif
