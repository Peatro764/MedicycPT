#include "Calc.h"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <QDebug>

namespace calc {
    double Mean(const std::vector<double> &data) {
        if (data.empty()) {
            return 0.0;
        } else {
            return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        }
    }

    double Median(std::vector<double> data) {
        const int size = static_cast<int>(data.size());
        if (size == 0) {
            return 0.0;
        } else if (size == 1) {
            return data.at(0);
        } else {
            std::sort(data.begin(), data.end());
            if ((size % 2) == 0) {
                return 0.5 * (data.at(size/2) + data.at(size/2 - 1));
            } else {
                return data.at(size/2);
            }
        }
    }

    double StdDev(const std::vector<double>& data) {
        if (data.size() < 2) {
            return 0.0;
        } else {
            double mean = Mean(data);
            std::vector<double> diff(data.size());
            std::transform(data.begin(), data.end(), diff.begin(), [mean](double x) { return x - mean; });
            double sqrSum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
            return std::sqrt(sqrSum / (data.size() - 1));
        }
    }

    std::vector<double> Square(std::vector<double> data) {
        std::transform(data.begin(), data.end(), data.begin(), [](double e) { return std::pow(e, 2); });
        return data;
    }

    double TStatistic(const std::vector<double>& sample1, std::vector<double>& sample2) {
        if (sample1.size() < size_t(2) || sample2.size() < size_t(2)) {
            return 0;
        }
        const int sn1 = static_cast<int>(sample1.size());
        const int sn2 = static_cast<int>(sample2.size());
        const double sd1 = StdDev(sample1);
        const double sd2 = StdDev(sample2);
        const double sm1 = Mean(sample1);
        const double sm2 = Mean(sample2);
        const double dof = static_cast<double>(sn1 + sn2 - 2);
        const double sp = std::sqrt(((sn1-1) * sd1 * sd1 + (sn2-1) * sd2 * sd2) / dof); // pooled variance
        const double t_stat = (sm1 - sm2) / (sp * std::sqrt(1.0 / sn1 + 1.0 / sn2));
        return t_stat;
    }

    // returns <a, b> as in y = a + b*x
    std::pair<double, double> LinearFit(const std::vector<double>& x, const std::vector<double>& y) {
        if (x.size() != y.size() || x.size() == 0) {
            qWarning() << "Calc::LinearFit Different size vectors " << x.size() << " " << y.size();
            throw std::runtime_error("Different size vectors");
        }

        const double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
        const double sum_y = std::accumulate(y.begin(), y.end(), 0.0);
        const double xy = std::inner_product(std::begin(x), std::end(x), std::begin(y), 0.0);
        const double xx = std::inner_product(std::begin(x), std::end(x), std::begin(x), 0.0);
        const double n = static_cast<double>(x.size());
        const double b = (n * xy - sum_x * sum_y) / (n * xx - sum_x * sum_x);
        const double a = (sum_y - b * sum_x) / n;
        return std::make_pair(a, b);
    }

    double PearsonCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
        const double cog = CenterOfGravity(x, y);
        const double x_mean = Mean(x);
        const double y_mean = Mean(y);
        const double x_sq_mean = Mean(Square(x));
        const double y_sq_mean = Mean(Square(y));
        const double pearson =  (cog - x_mean * y_mean) / std::sqrt((x_sq_mean - std::pow(x_mean, 2)) * (y_sq_mean - std::pow(y_mean, 2)));
        return pearson;
    }

    double CenterOfGravity(const std::vector<double>& x, const std::vector<double>& y) {
        if (x.size() != y.size() || x.empty()) {
            throw std::runtime_error("Cannot compute center of gravity of empty or different sized vectors");
        }
        double sum(0.0);
        for (int idx = 0; idx < static_cast<int>(x.size()); ++idx) {
            sum += x.at(idx) * y.at(idx);
        }
        return (sum / static_cast<double>(x.size()));
    }

    std::vector<double> Filter(const std::vector<double>& data, double n_stddev) {
        const double mean = Mean(data);
        const double stddev = StdDev(data);
        std::vector<double> filtered;
        auto it = std::copy_if(data.begin(), data.end(), std::back_inserter(filtered),
                               [&](double i){ return std::abs(i - mean) < n_stddev*stddev; });
        (void)it;
        return filtered;
    }

    bool AlmostEqual(double a, double b, double maxRelDiff = 0.001) {
        double diff = fabs(a - b);
        a = fabs(a);
        b = fabs(b);
        double largest = (b > a) ? b : a;
        return (diff <= largest * maxRelDiff);
    }

    bool AlmostEqual(const std::vector<double> &a, const std::vector<double> &b, double maxRelDiff = 0.001) {
        if (a.size() != b.size()) {
            return false;
        }
        for (int idx = 0; idx < (int)a.size(); ++idx) {
            if (!AlmostEqual(a.at(idx), b.at(idx), maxRelDiff)) return false;
        }
        return true;
    }

    double DoseToUM(const double& debit, const double& dose) {
        if (debit < 0.0001 || dose < 0.0) {
            qWarning() << QString("Util::DoseToUM Non valid input, returning 0");
            return 0.0;
        } else {
            return (100.0 * dose / debit);
        }
    }

    double UMToDose(const double& debit, const int& um) {
        if (debit < 0.0001 || um < 0) {
            qWarning() << QString("Util::UMToDose Non valid input, returning 0");
            return 0.0;
        } else {
            return (double)(((double)um) * debit / 100.0);
        }
    }

    double UM2MM(int um) {
        return static_cast<double>(um) / 1000.0;
    }

    int MM2UM(double mm) {
        return static_cast<int>(std::round(1000.0 * mm));
    }
}
