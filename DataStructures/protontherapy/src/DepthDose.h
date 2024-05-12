#ifndef DEPTHDOSE_H
#define DEPTHDOSE_H

#include <vector>

class DepthDose
{
public:
    DepthDose(const double& depth, const double& dose);
    double depth() const { return depth_; }
    double dose() const { return dose_; }
private:
    double depth_;
    double dose_;
};

DepthDose operator+(const DepthDose& d1, const DepthDose& d2);
DepthDose operator-(const DepthDose& d);
DepthDose operator-(const DepthDose& d1, const DepthDose& d2);
DepthDose operator*(const DepthDose& d, const double& p);
DepthDose operator*(const double& p, const DepthDose& d);
bool operator==(const DepthDose& d1, const DepthDose& d2);
bool operator!=(const DepthDose& d1, const DepthDose& d2);

#endif // DEPTHDOSE_H
